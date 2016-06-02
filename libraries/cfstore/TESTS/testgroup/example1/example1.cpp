/** @file example1.cpp
 *
 * mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Test cases to add and delete KVs in the CFSTORE.
 */


/* EXAMPLE1 Notes
 * ==============
 *
 * The example test does the following CFSTORE operations:
 * - initialises
 * - creates a key-value pair (KV).
 * - writes the data for the KV
 * - closes KV.
 * - flushes the KV to flash
 * - opens KV for reading.
 * - reads KV and checks the value blob was the same as previously written.
 * - closes KV.
 * - finds a KV (there is only 1 to find).
 * - for the KV returned, get the key name.
 * - for the KV returned, get the value length.
 * - for the KV returned, delete the KV.
 * - find another KV (which fails as there are no more keys to find).
 * - flushes the updated state to flash to store the removal of the deleted KV.
 * - uninitialises
 * - stops
 *
 * This test is coded so as to work in the following modes:
 * - flash sync mode i.e. with caps.asynchronous_ops == false
 * - flash async mode i.e. with caps.asynchronous_ops == true
 * The dual async/sync mode support with the same code is more complicated
 * than if the implementation just supported sync mode for example. However,
 * it has the benefit of being more versatile.
 *
 * To build the code for sync mode use the following yotta command:
 *      yotta --config='{"config":{"hardware":{"mtd":{"async":{"ops":0}}}}}' build
 *
 * To build the code for async mode use the following yotta command:
 *      yotta build
 *
 * The test leaves the flash in the same state as at the beginning of the test so
 * it can be run a second time on the device without flashing, and the test should
 * still work.
 *
 * todo: remove this comment when flash journal issue fixed:
 * ** KNOWN ISSUE **
 * In the following circumstances this test will fail:
 * - test flashed to target
 * - test run
 * - test run again (without flashing to target)
 * - test will fail as the test cannot create the KV, because the first run of the
 *   test stored and didnt delete the KV, and so it already exists in the cfstore
 *   and cannot be created again.
 * This issue is causes by a flash-journal bug of being unable to commit after
 * a log to delete the last KV.
 */
#if defined __MBED__ && ! defined TOOLCHAIN_GCC_ARM


#include <stdio.h>
int main()
{
    printf("Not implemented for ARM toolchain\n");
}


#else



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TARGET_LIKE_FRDM_K64F_GCC
#include <mbed-drivers/mbed.h>
#endif
#ifndef YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING
/* when built as Configuration-Store example, include greentea support otherwise omit */
#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"
#else   // YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING
// map utest types for building as stand alone example
#define control_t   void
#define CaseNext
#endif  // YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING

#include <stdio.h>
#include <string.h>
#include "cfstore_config.h"
#include <configuration-store/configuration_store.h>
#ifdef YOTTA_CFG_CONFIG_UVISOR
#include "uvisor-lib/uvisor-lib.h"
#endif /* YOTTA_CFG_CONFIG_UVISOR */


#ifndef YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING
using namespace utest::v1;
#endif


#define CFSTORE_EX1_TEST_ASSERT(Expr)                       if (!(Expr)) { printf("%s:%u: assertion failure\r\n", __FUNCTION__, __LINE__); while (1) ;}
#define CFSTORE_EX1_TEST_ASSERT_EQUAL(expected, actual)     if ((expected) != (actual)) {printf("%s:%u: assertion failure\r\n", __FUNCTION__, __LINE__); while (1) ;}
#define CFSTORE_EX1_TEST_ASSERT_NOT_EQUAL(expected, actual) if ((expected) == (actual)) {printf("%s:%u: assertion failure\r\n", __FUNCTION__, __LINE__); while (1) ;}

#define CFSTORE_EX1_TEST_ASSERT_MSG(Expr, _fmt, ...)      \
    do                                                    \
    {                                                     \
        if (!(Expr))                                      \
        {                                                 \
            printf(_fmt, __VA_ARGS__);                    \
            while (1) ;                                   \
        }                                                 \
    }while(0);

#define CFSTORE_EX1_LOG(_fmt, ...)                        \
  do                                                      \
  {                                                       \
        printf(_fmt, __VA_ARGS__);                        \
  }while(0);


const char* cfstore_ex_opcode_str[] =
{
    "UNDEFINED",
    "CFSTORE_OPCODE_CLOSE",
    "CFSTORE_OPCODE_CREATE",
    "CFSTORE_OPCODE_DELETE",
    "CFSTORE_OPCODE_FIND",
    "CFSTORE_OPCODE_FLUSH",
    "CFSTORE_OPCODE_GET_KEY_NAME",
    "CFSTORE_OPCODE_GET_STATUS",
    "CFSTORE_OPCODE_GET_VALUE_LEN",
    "CFSTORE_OPCODE_INITIALIZE",
    "CFSTORE_OPCODE_OPEN",
    "CFSTORE_OPCODE_POWER_CONTROL",
    "CFSTORE_OPCODE_READ",
    "CFSTORE_OPCODE_RSEEK",
    "CFSTORE_OPCODE_UNINITIALIZE",
    "CFSTORE_OPCODE_WRITE",
    "CFSTORE_OPCODE_MAX"
};

bool cfstore_example1_done = false;
const char* cfstore_ex_kv_name = "basement.medicine.pavement.government.trenchcoat.off.cough.off.kid.did.when.again.alleyway.friend.cap.pen.dollarbills.ten.foot.soot.put.but.anyway.say.May.DA.kid.did.toes.bows.those.hose.nose.clothes.man.blows.well.well";
const char* cfstore_ex_kv_value = "TheRollingStone";
#define CFSTORE_EX1_RSEEK_OFFSET    10   /* offset to S of Stone */

typedef enum cfstore_ex_state_t {
    CFSTORE_EX_STATE_INITIALIZING = 1,
    CFSTORE_EX_STATE_INIT_DONE,
    CFSTORE_EX_STATE_CREATING,
    CFSTORE_EX_STATE_CREATE_DONE,
    CFSTORE_EX_STATE_WRITING,
    CFSTORE_EX_STATE_WRITE_DONE,
    CFSTORE_EX_STATE_CLOSING1,
    CFSTORE_EX_STATE_CLOSE_DONE1,
    CFSTORE_EX_STATE_FLUSHING1,
    CFSTORE_EX_STATE_FLUSH_DONE1,
    CFSTORE_EX_STATE_OPENING,
    CFSTORE_EX_STATE_OPEN_DONE,
    CFSTORE_EX_STATE_READING1,
    CFSTORE_EX_STATE_READ_DONE1,
    CFSTORE_EX_STATE_RSEEKING,
    CFSTORE_EX_STATE_RSEEK_DONE,
    CFSTORE_EX_STATE_READING2,
    CFSTORE_EX_STATE_READ_DONE2,
    CFSTORE_EX_STATE_CLOSING2,
    CFSTORE_EX_STATE_CLOSE_DONE2,
    CFSTORE_EX_STATE_FINDING1,
    CFSTORE_EX_STATE_FIND_DONE1,
    CFSTORE_EX_STATE_GETTING_KEY_NAME,
    CFSTORE_EX_STATE_GET_KEY_NAME_DONE,
    CFSTORE_EX_STATE_GETTING_VALUE_LEN,
    CFSTORE_EX_STATE_GET_VALUE_LEN_DONE,
    CFSTORE_EX_STATE_DELETING,
    CFSTORE_EX_STATE_DELETE_DONE,
    CFSTORE_EX_STATE_FINDING2,
    CFSTORE_EX_STATE_FIND_DONE2,
    CFSTORE_EX_STATE_FLUSHING2,
    CFSTORE_EX_STATE_FLUSH_DONE2,
    CFSTORE_EX_STATE_UNINITIALIZING,
    CFSTORE_EX_STATE_UNINIT_DONE
} cfstore_ex_state_t;

typedef struct cfstore_example1_ctx_t
{
    ARM_CFSTORE_CAPABILITIES caps;;
    uint8_t hkey[CFSTORE_HANDLE_BUFSIZE];
    uint8_t hkey_next_buf[CFSTORE_HANDLE_BUFSIZE];
    uint8_t hkey_prev_buf[CFSTORE_HANDLE_BUFSIZE];
    ARM_CFSTORE_HANDLE hkey_next;
    ARM_CFSTORE_HANDLE hkey_prev;
    cfstore_ex_state_t state;
    ARM_CFSTORE_SIZE len;
    ARM_CFSTORE_KEYDESC kdesc;
    ARM_CFSTORE_FMODE flags;
    char value[CFSTORE_KEY_NAME_MAX_LENGTH+1];
    /* callback attributes*/
    int32_t callback_status;
    ARM_CFSTORE_HANDLE callback_handle;
} cfstore_example1_ctx_t;

static cfstore_example1_ctx_t cfstore_example1_ctx_g;

extern ARM_CFSTORE_DRIVER cfstore_driver;
ARM_CFSTORE_DRIVER *cfstore_drv = &cfstore_driver;

/* forward declarations */
static void cfstore_ex_fms_update(cfstore_example1_ctx_t* ctx);


static void cfstore_ex_callback(int32_t status, ARM_CFSTORE_OPCODE cmd_code, void *client_context, ARM_CFSTORE_HANDLE handle)
{
    (void) handle;

    cfstore_example1_ctx_t* ctx = (cfstore_example1_ctx_t*) client_context;

    /* CFSTORE_EX1_LOG("%s:entered: status=%d, cmd_code=%d (%s) handle=%p\r\n", __func__, (int) status, (int) cmd_code, cfstore_ex_opcode_str[cmd_code], handle); */
    switch(cmd_code)
    {
    case CFSTORE_OPCODE_INITIALIZE:
        ctx->state = CFSTORE_EX_STATE_INIT_DONE;
        break;
    case CFSTORE_OPCODE_CREATE:
        ctx->state = CFSTORE_EX_STATE_CREATE_DONE;
        break;
    case CFSTORE_OPCODE_WRITE:
        ctx->state = CFSTORE_EX_STATE_WRITE_DONE;
        break;
    case CFSTORE_OPCODE_CLOSE:
        switch(ctx->state)
        {
        case(CFSTORE_EX_STATE_CLOSING1):
            ctx->state = CFSTORE_EX_STATE_CLOSE_DONE1;
            break;
        case(CFSTORE_EX_STATE_CLOSING2):
            ctx->state = CFSTORE_EX_STATE_CLOSE_DONE2;
            break;
        default:
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown Close() error (line=%u)\r\n", __func__, __LINE__);
            break;
        }
        break;
    case CFSTORE_OPCODE_FLUSH:
        switch(ctx->state)
        {
            case(CFSTORE_EX_STATE_FLUSHING1):
                ctx->state = CFSTORE_EX_STATE_FLUSH_DONE1;
                break;
            case(CFSTORE_EX_STATE_FLUSHING2):
                ctx->state = CFSTORE_EX_STATE_FLUSH_DONE2;
                break;
            default:
                CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown Find() error (line=%u)\r\n", __func__, __LINE__);
                break;
        }
        break;
    case CFSTORE_OPCODE_OPEN:
        ctx->state = CFSTORE_EX_STATE_OPEN_DONE;
        break;
    case CFSTORE_OPCODE_FIND:
        switch(ctx->state)
        {
        case(CFSTORE_EX_STATE_FINDING1):
            ctx->state = CFSTORE_EX_STATE_FIND_DONE1;
            break;
        case(CFSTORE_EX_STATE_FINDING2):
            ctx->state = CFSTORE_EX_STATE_FIND_DONE2;
            break;
        default:
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown Find() error (line=%u)\r\n", __func__, __LINE__);
            break;
        }
        break;
    case CFSTORE_OPCODE_GET_KEY_NAME:
        ctx->state = CFSTORE_EX_STATE_GET_KEY_NAME_DONE;
        break;
    case CFSTORE_OPCODE_GET_VALUE_LEN:
        ctx->state = CFSTORE_EX_STATE_GET_VALUE_LEN_DONE;
        break;
    case CFSTORE_OPCODE_READ:
        switch(ctx->state)
        {
        case(CFSTORE_EX_STATE_READING1):
            ctx->state = CFSTORE_EX_STATE_READ_DONE1;
            break;
        case(CFSTORE_EX_STATE_READING2):
            ctx->state = CFSTORE_EX_STATE_READ_DONE2;
            break;
        default:
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown Read() error (line=%u)\r\n", __func__, __LINE__);
            break;
        }
        break;
    case CFSTORE_OPCODE_RSEEK:
        ctx->state = CFSTORE_EX_STATE_RSEEK_DONE;
        break;
    case CFSTORE_OPCODE_DELETE:
        ctx->state = CFSTORE_EX_STATE_DELETE_DONE;
        break;
    case CFSTORE_OPCODE_UNINITIALIZE:
        ctx->state = CFSTORE_EX_STATE_UNINIT_DONE;
        break;
    case CFSTORE_OPCODE_GET_STATUS:
    case CFSTORE_OPCODE_POWER_CONTROL:
    default:
        CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: received asynchronous notification for opcode=%d (%s)\r\b", __func__, cmd_code, cmd_code < CFSTORE_OPCODE_MAX ? cfstore_ex_opcode_str[cmd_code] : "unknown");
        break;
    }
    ctx->callback_status = status;
    ctx->callback_handle = handle;
    cfstore_ex_fms_update(ctx);
    return;
}

static void cfstore_ex_fms_update(cfstore_example1_ctx_t* ctx)
{
    int32_t ret;

    switch (ctx->state)
    {
        case CFSTORE_EX_STATE_INITIALIZING:
            CFSTORE_EX1_LOG("INITIALIZING%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_INITIALIZE can be invoked before Initialize() has returned
            ret = cfstore_drv->Initialize(cfstore_ex_callback, ctx);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Initialize() should return ret >= 0 for async/synch modes(ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_INIT_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_INIT_DONE:
            CFSTORE_EX1_LOG("INIT_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Initialize() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_INITIALIZE) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            ctx->state = CFSTORE_EX_STATE_CREATING;
            // intentional fall-through

        case CFSTORE_EX_STATE_CREATING:
            CFSTORE_EX1_LOG("CREATING%s", "\r\n");
            memset(&ctx->kdesc, 0, sizeof(ARM_CFSTORE_KEYDESC));
            ctx->kdesc.drl = ARM_RETENTION_NVM;
            ctx->len = strlen(cfstore_ex_kv_value);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_CREATE can be invoked before Create() has returned
            ret = cfstore_drv->Create(cfstore_ex_kv_name, ctx->len, &ctx->kdesc, ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Create() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_CREATE_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_CREATE_DONE:
            CFSTORE_EX1_LOG("CREATE_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Create() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_CREATE) received handle (%p) is not the hkey supplied to Create()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            ctx->state = CFSTORE_EX_STATE_WRITING;
            // intentional fall-through

        case CFSTORE_EX_STATE_WRITING:
            CFSTORE_EX1_LOG("WRITING%s", "\r\n");
            ctx->len = strlen(cfstore_ex_kv_value);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_WRITE can be invoked before Write() has returned
            ret = cfstore_drv->Write(ctx->hkey, cfstore_ex_kv_value, &ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Write() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_WRITE_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_WRITE_DONE:
            CFSTORE_EX1_LOG("WRITE_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Write() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) strlen(cfstore_ex_kv_value), "%s:Error: Write() number of octets written (i.e. completion status (%ld)) != strlen(ctx->value)(%ld)\r\n", __func__, ctx->callback_status, (int32_t) strlen(cfstore_ex_kv_value));
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) ctx->len, "%s:Error: Write() number of octets written (i.e. completion status (%ld)) != updated value of len parameter (%ld)\r\n", __func__, ctx->callback_status, (int32_t) ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_WRITE) received handle (%p) is not the hkey supplied to Write()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            ctx->state = CFSTORE_EX_STATE_CLOSING1;
            // intentional fall-through

        case CFSTORE_EX_STATE_CLOSING1:
            CFSTORE_EX1_LOG("CLOSING1%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_CLOSE can be invoked before Close() has returned
            ret = cfstore_drv->Close(ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Close() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_CLOSE_DONE1;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_CLOSE_DONE1:
            CFSTORE_EX1_LOG("CLOSE_DONE1%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Close() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_CLOSE) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            ctx->state = CFSTORE_EX_STATE_FLUSHING1;
            // intentional fall-through

        case CFSTORE_EX_STATE_FLUSHING1:
            CFSTORE_EX1_LOG("FLUSHING1%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_FLUSH can be invoked before Flush() has returned
            ret = cfstore_drv->Flush();
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Flush() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_FLUSH_DONE1;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_FLUSH_DONE1:
            CFSTORE_EX1_LOG("FLUSH_DONE1%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Flush() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_FLUSH) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            ctx->state = CFSTORE_EX_STATE_OPENING;
            // intentional fall-through

        case CFSTORE_EX_STATE_OPENING:
            CFSTORE_EX1_LOG("OPENING%s", "\r\n");
            memset(&ctx->flags, 0, sizeof(ctx->flags));
            memset(&ctx->hkey, 0, CFSTORE_HANDLE_BUFSIZE);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_OPEN can be invoked before Open() has returned
            ret = cfstore_drv->Open(cfstore_ex_kv_name, ctx->flags, ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Open() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_OPEN_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_OPEN_DONE:
            CFSTORE_EX1_LOG("OPEN_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Open() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_OPEN) received handle (%p) is not the hkey supplied to Open()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            ctx->state = CFSTORE_EX_STATE_READING1;
            // intentional fall-through

        case CFSTORE_EX_STATE_READING1:
            CFSTORE_EX1_LOG("READING1%s", "\r\n");
            ctx->len = CFSTORE_KEY_NAME_MAX_LENGTH;
            memset(ctx->value, 0, CFSTORE_KEY_NAME_MAX_LENGTH+1);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_READ can be invoked before Read() has returned
            ret = cfstore_drv->Read(ctx->hkey, ctx->value, &ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Read() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_READ_DONE1;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_READ_DONE1:
            CFSTORE_EX1_LOG("READ_DONE1%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Read() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) strlen(cfstore_ex_kv_value), "%s:Error: Read() number of octets read (i.e. completion status (%ld)) != strlen(ctx->value)(%ld)\r\n", __func__, ctx->callback_status, (int32_t) strlen(cfstore_ex_kv_value));
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) ctx->len, "%s:Error: Read() number of octets read (i.e. completion status (%ld)) != updated value of len parameter (%ld)\r\n", __func__, ctx->callback_status, (int32_t) ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_READ) received handle (%p) is not the hkey supplied to Read()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(strncmp(ctx->value, cfstore_ex_kv_value, strlen(cfstore_ex_kv_value)) == 0, "%s:Error: the read value (%s) is not as expected (%s)\r\n", __func__, ctx->value, cfstore_ex_kv_value);
            ctx->state = CFSTORE_EX_STATE_RSEEKING;
            // intentional fall-through

        case CFSTORE_EX_STATE_RSEEKING:
            CFSTORE_EX1_LOG("RSEEKING%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_READ can be invoked before Read() has returned
            ret = cfstore_drv->Rseek(ctx->hkey, CFSTORE_EX1_RSEEK_OFFSET);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Rseek() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_RSEEK_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_RSEEK_DONE:
            CFSTORE_EX1_LOG("RSEEK_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Read() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_RSEEK) received handle (%p) is not the hkey supplied to Read()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            ctx->state = CFSTORE_EX_STATE_READING2;
            // intentional fall-through

        case CFSTORE_EX_STATE_READING2:
            CFSTORE_EX1_LOG("READING2%s", "\r\n");
            ctx->len = CFSTORE_KEY_NAME_MAX_LENGTH;
            memset(ctx->value, 0, CFSTORE_KEY_NAME_MAX_LENGTH+1);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_READ can be invoked before Read() has returned
            ret = cfstore_drv->Read(ctx->hkey, ctx->value, &ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Read() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_READ_DONE2;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_READ_DONE2:
            CFSTORE_EX1_LOG("READ_DONE2%s", "\r\n");
            CFSTORE_EX1_LOG("%s: value=%s\r\n", __func__, ctx->value);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Read() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) strlen(&cfstore_ex_kv_value[CFSTORE_EX1_RSEEK_OFFSET]), "%s:Error: Read() number of octets read (i.e. completion status (%ld)) != strlen(ctx->value)(%ld)\r\n", __func__, ctx->callback_status, (int32_t) strlen(&cfstore_ex_kv_value[CFSTORE_EX1_RSEEK_OFFSET]));
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == (int32_t) ctx->len, "%s:Error: Read() number of octets read (i.e. completion status (%ld)) != updated value of len parameter (%ld)\r\n", __func__, ctx->callback_status, (int32_t) ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_READ) received handle (%p) is not the hkey supplied to Read()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(strncmp(ctx->value, &cfstore_ex_kv_value[CFSTORE_EX1_RSEEK_OFFSET], strlen(&cfstore_ex_kv_value[CFSTORE_EX1_RSEEK_OFFSET])) == 0, "%s:Error: the read value (%s) is not as expected (%s)\r\n", __func__, ctx->value, &cfstore_ex_kv_value[CFSTORE_EX1_RSEEK_OFFSET]);
            ctx->state = CFSTORE_EX_STATE_CLOSING2;
            // intentional fall-through

        case CFSTORE_EX_STATE_CLOSING2:
            CFSTORE_EX1_LOG("CLOSING2%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_CLOSE can be invoked before Close() has returned
            ret = cfstore_drv->Close(ctx->hkey);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Close() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_CLOSE_DONE2;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_CLOSE_DONE2:
            CFSTORE_EX1_LOG("CLOSE_DONE2%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Close() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_CLOSE) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            ctx->state = CFSTORE_EX_STATE_FINDING1;
            // intentional fall-through

        case CFSTORE_EX_STATE_FINDING1:
            CFSTORE_EX1_LOG("FINDING1%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_FIND can be invoked before Find() has returned
            ret = cfstore_drv->Find("*", ctx->hkey_next, ctx->hkey_prev);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Find() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_FIND_DONE1;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_FIND_DONE1:
            CFSTORE_EX1_LOG("FIND_DONE1%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == ARM_DRIVER_OK, "%s:Error: Find() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey_prev, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_FIND) received handle (%p) is not the hkey supplied to Find()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey_prev);
            ctx->state = CFSTORE_EX_STATE_GETTING_KEY_NAME;
            // intentional fall-through

        case CFSTORE_EX_STATE_GETTING_KEY_NAME:
            CFSTORE_EX1_LOG("GETTING_KEY_NAME%s", "\r\n");
            ctx->len = CFSTORE_KEY_NAME_MAX_LENGTH;
            memset(ctx->value, 0, CFSTORE_KEY_NAME_MAX_LENGTH+1);
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_GET_KEY_NAME can be invoked before GetKeyName() has returned
            ret = cfstore_drv->GetKeyName(ctx->hkey_prev, ctx->value, (uint8_t*) &ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: GetKeyName() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_GET_KEY_NAME_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_GET_KEY_NAME_DONE:
            CFSTORE_EX1_LOG("GET_KEY_NAME_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: GetKeyName() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG( ((int32_t) ctx->len == ((int32_t) strlen(cfstore_ex_kv_name)+1)), "%s:Error: GetKeyName() updated value of len parameter (%ld) != strlen(cfstore_ex_kv_name) (%ld) (\r\n", __func__, (int32_t) ctx->len, (int32_t) strlen(cfstore_ex_kv_name));
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey_prev, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_GET_KEY_NAME) received handle (%p) is not the hkey supplied to GetKeyName()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey_prev);
            CFSTORE_EX1_TEST_ASSERT_MSG(strncmp(ctx->value, cfstore_ex_kv_name, strlen(cfstore_ex_kv_name)) == 0, "%s:Error: the key name (%s) is not as expected (%s)\r\n", __func__, ctx->value, cfstore_ex_kv_name);
            ctx->state = CFSTORE_EX_STATE_GETTING_VALUE_LEN;
            // intentional fall-through

        case CFSTORE_EX_STATE_GETTING_VALUE_LEN:
            CFSTORE_EX1_LOG("GETTING_VALUE_LEN%s", "\r\n");
            ctx->len = CFSTORE_KEY_NAME_MAX_LENGTH;
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_GET_VALUE_LEN can be invoked before GetValueLen() has returned
            ret = cfstore_drv->GetValueLen(ctx->hkey_prev, &ctx->len);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: GetValueLen() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_GET_VALUE_LEN_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_GET_VALUE_LEN_DONE:
            CFSTORE_EX1_LOG("GET_VALUE_LEN_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: GetValueLen() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG((int32_t) ctx->len == (int32_t) strlen(cfstore_ex_kv_value), "%s:Error: GetValueLen() updated value of len parameter (%ld) != strlen(cfstore_ex_kv_value)(%ld) \r\n", __func__, (int32_t) ctx->len, (int32_t) strlen(cfstore_ex_kv_value));
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == ctx->hkey_prev, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_GET_VALUE_LEN) received handle (%p) is not the hkey supplied to GetValueLen()(%p)\r\n", __func__, ctx->callback_handle, ctx->hkey_prev);
            ctx->state = CFSTORE_EX_STATE_DELETING;
            // intentional fall-through

        case CFSTORE_EX_STATE_DELETING:
            CFSTORE_EX1_LOG("DELETING%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_DELETE can be invoked before Delete() has returned
            ret = cfstore_drv->Delete(ctx->callback_handle);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Close() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_DELETE_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_DELETE_DONE:
            CFSTORE_EX1_LOG("DELETE_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Delete() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_DELETE) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            CFSTORE_HANDLE_SWAP(ctx->hkey_prev, ctx->hkey_next);
            ctx->state = CFSTORE_EX_STATE_FINDING2;
            // intentional fall-through

        case CFSTORE_EX_STATE_FINDING2:
            CFSTORE_EX1_LOG("FINDING2%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_FIND can be invoked before Find() has returned
            ret = cfstore_drv->Find("*", ctx->hkey_next, ctx->hkey_prev);
            CFSTORE_EX1_TEST_ASSERT_MSG(ret == ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND, "%s:Error: Find() failed to return expected value of ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND (ret=%ld)\r\n", __func__, ret);
            if(ret == ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_FIND_DONE2;
                break;
            } else if(ret == ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_FIND_DONE2:
            CFSTORE_EX1_LOG("FIND_DONE2%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status == ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND, "%s:Error: Find() completion should have been ARM_CFSTORE_DRIVER_ERROR_KEY_NOT_FOUND (status=%ld)\r\n", __func__, ctx->callback_status);
        // todo: delete next 2 lines when fix flash journal issue mentioned below
            ctx->state = CFSTORE_EX_STATE_UNINITIALIZING;
            // intentional fall-through

        /* todo: restore this code when issue with flushing empty flash to flash journal has been fixed
            ctx->state = CFSTORE_EX_STATE_FLUSHING2;
            // intentional fall-through

        case CFSTORE_EX_STATE_FLUSHING2:
            CFSTORE_EX1_LOG("FLUSHING2%s", "\r\n");
            // note that cfstore_ex_callback() for cmd_code==CFSTORE_OPCODE_FLUSH can be invoked before Flush() has returned
            ret = cfstore_drv->Flush();
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error:2: Flush() failed (ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_FLUSH_DONE2;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_FLUSH_DONE2:
            CFSTORE_EX1_LOG("FLUSH_DONE2%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_status >= ARM_DRIVER_OK, "%s:Error: Flush() completion failed (status=%ld)\r\n", __func__, ctx->callback_status);
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_FLUSH) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            ctx->state = CFSTORE_EX_STATE_UNINITIALIZING;
            // intentional fall-through
        */

        case CFSTORE_EX_STATE_UNINITIALIZING:
            CFSTORE_EX1_LOG("UNINITIALIZING%s", "\r\n");
            ret = cfstore_drv->Uninitialize();
            CFSTORE_EX1_TEST_ASSERT_MSG(ret >= ARM_DRIVER_OK, "%s:Error: Uninitialize() should return ret >= 0 for async/synch modes(ret=%ld)\r\n", __func__, ret);
            if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == false) {
                ctx->state = CFSTORE_EX_STATE_UNINIT_DONE;
                break;
            } else if(ret >= ARM_DRIVER_OK && ctx->caps.asynchronous_ops == true) {
                // await pending notification of completion.
                break;
            }
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

        case CFSTORE_EX_STATE_UNINIT_DONE:
            CFSTORE_EX1_LOG("UNINIT_DONE%s", "\r\n");
            CFSTORE_EX1_TEST_ASSERT_MSG(ctx->callback_handle == NULL, "%s:Error: the cfstore_ex_callback(cmd_code==CFSTORE_OPCODE_UNINITIALIZE) received non-NULL handle (%p)\r\n", __func__, ctx->callback_handle);
            cfstore_example1_done = true;
            CFSTORE_EX1_LOG("***************%s", "\r\n");
            CFSTORE_EX1_LOG("*** SUCCESS ***%s", "\r\n");
            CFSTORE_EX1_LOG("***************%s", "\r\n");
            break;

        /* todo: remove these lines when the flash-journal issue is fixed */
        case CFSTORE_EX_STATE_FLUSHING2:
        case CFSTORE_EX_STATE_FLUSH_DONE2:
            CFSTORE_EX1_TEST_ASSERT_MSG(false, "%s:Error: unknown error (line=%u)\r\n", __func__, __LINE__);
            break;

    }
}

static control_t cfstore_example1_app_start(const size_t call_count)
{
    cfstore_example1_ctx_t* ctx = &cfstore_example1_ctx_g;

    (void) call_count;

    /* initialise the context */
    memset(ctx, 0, sizeof(cfstore_example1_ctx_t));
    cfstore_example1_done = false;
    ctx->hkey_next = ctx->hkey_next_buf;
    ctx->hkey_prev = ctx->hkey_prev_buf;
    ctx->callback_status = ARM_DRIVER_ERROR;
    ctx->state = CFSTORE_EX_STATE_INITIALIZING;
    ctx->caps = cfstore_drv->GetCapabilities();
    CFSTORE_EX1_LOG("%s:INITIALIZING: caps.asynchronous_ops=%lu\n", __func__, ctx->caps.asynchronous_ops);
    cfstore_ex_fms_update(ctx);

    /* main application worker loop */
    while (!cfstore_example1_done)
    {
        // do some work
        CFSTORE_EX1_LOG("%s: going to sleep!\r\n", __func__);

#if defined CFSTORE_CONFIG_MBED_OS_VERSION && CFSTORE_CONFIG_MBED_OS_VERSION == 3
        __WFE();
#endif /* CFSTORE_CONFIG_MBED_OS_VERSION == 3 */

#if defined CFSTORE_CONFIG_MBED_OS_VERSION && CFSTORE_CONFIG_MBED_OS_VERSION == 4
        /* mbedosV3++
         * todo: port __WFE()
         */
#endif /* CFSTORE_CONFIG_MBED_OS_VERSION == 4 */
        CFSTORE_EX1_LOG("%s: woke up!\r\n", __func__);
    }
    return CaseNext;
}

#ifndef YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING
/* when built as Configuration-Store example, include greentea support otherwise omit */

/* report whether built/configured for flash sync or async mode */
static control_t cfstore_example1_test_00(const size_t call_count)
{
    (void) call_count;
    CFSTORE_EX1_LOG("INITIALIZING: caps.asynchronous_ops=%lu\n", cfstore_driver.GetCapabilities().asynchronous_ops);
    return CaseNext;
}

utest::v1::status_t greentea_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(100, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Case cases[] = {
           /*          1         2         3         4         5         6        7  */
           /* 1234567890123456789012345678901234567890123456789012345678901234567890 */
        Case("EXAMPLE1_test_00", cfstore_example1_test_00),
        Case("EXAMPLE1_test_01_start", cfstore_example1_app_start),
};


/* Declare your test specification with a custom setup handler */
Specification specification(greentea_setup, cases);

#if defined CFSTORE_CONFIG_MBED_OS_VERSION && CFSTORE_CONFIG_MBED_OS_VERSION == 3
/* mbedosV3*/
void app_start(int argc __unused, char** argv __unused)
{
    /* Run the test specification */
    Harness::run(specification);
}
#endif /* CFSTORE_CONFIG_MBED_OS_VERSION == 3 */

#if defined CFSTORE_CONFIG_MBED_OS_VERSION && CFSTORE_CONFIG_MBED_OS_VERSION == 4
/* mbedosV3++*/
int main()
{
    return !Harness::run(specification);
}
#endif /* CFSTORE_CONFIG_MBED_OS_VERSION == 4 */


#else   // YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING

// stand alone Configuration-Store-Example
void app_start(int argc __unused, char** argv __unused)
{
    cfstore_example1_app_start(0);
}



#endif // YOTTA_CONFIGURATION_STORE_EXAMPLE1_VERSION_STRING


#endif // __MBED__ && ! defined TOOLCHAIN_GCC_ARM
