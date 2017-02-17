/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
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
 */

#include "flash_api.h"
#include "flash_data.h"
#include "critical.h"

// This file is automagically generated

// This is a flash algo binary blob. It is PIC (position independent code) that should be stored in RAM
static uint32_t FLASH_ALGO[] = {
    0xb5104938, 0x60084449, 0xf24c4837, 0x81c15120, 0x1128f64d, 0x880181c1, 0x0101f021, 0x48348001,
    0x44484932, 0x1280f44f, 0x21006001, 0x1201e9c0, 0x52a0f04f, 0xf44f6142, 0x61824280, 0x1020f880,
    0x62411e49, 0xf939f000, 0xd0002800, 0xbd102001, 0x47702000, 0x4a27b508, 0x9200447a, 0x02cff3c1,
    0x48234601, 0x44482300, 0xf92cf000, 0xd0002800, 0xbd082001, 0x491fb508, 0x481d4479, 0x44483920,
    0xf89ff000, 0xd10f2800, 0x4478481a, 0x38324b1a, 0x9000447b, 0x22084816, 0x6181f44f, 0xf0004448,
    0x2800f959, 0x2001d000, 0x4b12bd08, 0x4601447b, 0x3b54480f, 0x5280f44f, 0xf0004448, 0xb508b8b6,
    0x1dc94613, 0x0207f021, 0x4479490a, 0x91003972, 0x48074601, 0xf0004448, 0x2800f93d, 0x2001d000,
    0x0000bd08, 0x00000004, 0x40052000, 0x40020000, 0x00000008, 0x000000a1, 0x0000037c, 0x4604b570,
    0x25006800, 0x061b7803, 0x2370d5fc, 0x20007003, 0x280ce03a, 0xe8dfd236, 0x0a06f000, 0x1a16120e,
    0x2a26221e, 0x6826322e, 0x71f37813, 0x6826e02a, 0x71b37853, 0x6826e026, 0x71737893, 0x6826e022,
    0x713378d3, 0x6826e01e, 0x72f37913, 0x6826e01a, 0x72b37953, 0x6826e016, 0x72737993, 0x6826e012,
    0x723379d3, 0x6826e00e, 0x73f37a13, 0x6826e00a, 0x73b37a53, 0x6826e006, 0x73737a93, 0x6826e002,
    0x73337ad3, 0xb2c01c40, 0xd9c24288, 0x20806821, 0xe0037008, 0x1c416a60, 0x4780d000, 0x78006820,
    0xd5f70600, 0x78006820, 0xd5010681, 0xe0062504, 0xd50106c1, 0xe0022508, 0xd00007c0, 0x46282510,
    0xb508bd70, 0x460b2244, 0x2000f88d, 0x2100466a, 0xbd084798, 0x4614b538, 0xd002070a, 0x7080f44f,
    0x6843bd38, 0xd803428b, 0x441a6882, 0xd80c428a, 0x428b68c3, 0x6902d803, 0x428a441a, 0x2002d801,
    0x1ac9bd38, 0x0100f501, 0x1ac9e000, 0xf88d2208, 0x0c0a2000, 0x2001f88d, 0xf88d0a0a, 0xf88d2002,
    0x466a1003, 0x47a02103, 0xe92dbd38, 0x460745f8, 0x46164698, 0x2000687b, 0x428b198a, 0x68bcd803,
    0x4294441c, 0x68fbd20e, 0xd803428b, 0x441c693c, 0xd2024294, 0xe8bd2002, 0x1acc85f8, 0x0400f504,
    0xe0022500, 0xf44f1acc, 0xfbb45580, 0xfb05f1f5, 0xb1114111, 0x7080f44f, 0xfbb6e7ed, 0xfb05f1f5,
    0xb1a96111, 0xe7e62001, 0xa000f88d, 0xf88d0c20, 0x0a200001, 0x0002f88d, 0x4003f88d, 0x2103466a,
    0x46434638, 0x28004798, 0x1b76d1d5, 0xe001442c, 0x0a09f04f, 0xd1e72e00, 0x4601e7cd, 0x61082000,
    0x477061c8, 0x41fce92d, 0x9d086846, 0x1402eb01, 0xd803428e, 0x44376887, 0xd80a428f, 0x428f68c7,
    0xf8d0d804, 0x4467c010, 0xd802428f, 0xe8bd2002, 0x42a681fc, 0x6887d805, 0x42a74437, 0x1b89d301,
    0x68c6e009, 0xd90342a6, 0x44376907, 0xd3ed42a7, 0xf5011b89, 0x24100100, 0xf6f4fbb1, 0x1416fb04,
    0xf44fb114, 0xe7e27080, 0xf88d2401, 0x0c0c4000, 0x4001f88d, 0xf88d0a0c, 0xf88d4002, 0x0a111003,
    0x1004f88d, 0x2005f88d, 0x3006f88d, 0x2106466a, 0xe7cc47a8, 0x43fee92d, 0x46074616, 0x2000461c,
    0xf8dd198a, 0x074b8028, 0xf44fd003, 0xe8bd7080, 0x077383fe, 0x2001d001, 0x687be7f9, 0xd803428b,
    0x441d68bd, 0xd20c4295, 0x428b68fb, 0x693dd803, 0x4295441d, 0x2002d201, 0x1acde7e9, 0x0500f505,
    0x1acde02e, 0x2007e02c, 0x0000f88d, 0xf88d0c28, 0x0a280001, 0x0002f88d, 0x5003f88d, 0xf88d78e0,
    0x78a00004, 0x0005f88d, 0xf88d7860, 0x78200006, 0x0007f88d, 0xf88d79e0, 0x79a00008, 0x0009f88d,
    0xf88d7960, 0x7920000a, 0x000bf88d, 0x210b466a, 0x46434638, 0x28004798, 0x3508d1b9, 0x34083e08,
    0xd1d02e00, 0x0000e7b3, 0xfffffffe, 0x00000000, 0xffffffff, 0xfffffffe, 0x00000000, 0x00000000
};

static const flash_algo_t flash_algo_config = {
    .init = 0x1,
    .uninit = 0x51,
    .erase_sector = 0xab,
    .program_page = 0xbf,
    .static_base = 0x418,
    .algo_blob = FLASH_ALGO
};

static const sector_info_t sectors_info[] = {
    {0x0, 0x1000},
};

static const flash_target_config_t flash_target_config = {
    .page_size  = 0x200,
    .flash_start = 0x0,
    .flash_size = 0x100000,
    .sectors = sectors_info,
    .sector_info_count = sizeof(sectors_info) / sizeof(sector_info_t)
};

void flash_set_target_config(flash_t *obj)
{
    obj->flash_algo = &flash_algo_config;
    obj->target_config = &flash_target_config;
}
