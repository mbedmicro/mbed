/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, STMicroelectronics
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
#include "mbed_assert.h"
#include "serial_api.h"

#if DEVICE_SERIAL

#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include <string.h>
#include "PeripheralPins.h"

#define UART_NUM (5)

static uint32_t serial_irq_ids[UART_NUM] = {0, 0, 0, 0, 0};

static uart_irq_handler irq_handler;

UART_HandleTypeDef UartHandle;

int stdio_uart_inited = 0;
serial_t stdio_uart;

static void init_uart(serial_t *obj)
{
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);

    if (obj->uart == LPUART_1) {
        UartHandle.Init.BaudRate = obj->baudrate >> 1;
    } else {
        UartHandle.Init.BaudRate = obj->baudrate;
    }
    UartHandle.Init.WordLength = obj->databits;
    UartHandle.Init.StopBits   = obj->stopbits;
    UartHandle.Init.Parity     = obj->parity;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;

    if (obj->pin_rx == NC) {
        UartHandle.Init.Mode = UART_MODE_TX;
    } else if (obj->pin_tx == NC) {
        UartHandle.Init.Mode = UART_MODE_RX;
    } else {
        UartHandle.Init.Mode = UART_MODE_TX_RX;
    }

    // Disable the reception overrun detection
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
    UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        error("Cannot initialize UART");
    }
}

void serial_init(serial_t *obj, PinName tx, PinName rx)
{
    // Determine the UART to use (UART_1, UART_2, ...)
    UARTName uart_tx = (UARTName)pinmap_peripheral(tx, PinMap_UART_TX);
    UARTName uart_rx = (UARTName)pinmap_peripheral(rx, PinMap_UART_RX);

    // Get the peripheral name (UART_1, UART_2, ...) from the pin and assign it to the object
    obj->uart = (UARTName)pinmap_merge(uart_tx, uart_rx);
    MBED_ASSERT(obj->uart != (UARTName)NC);

    // Enable UART clock
#if defined(USART1_BASE)
    if (obj->uart == UART_1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        obj->index = 0;
    }
#endif

    if (obj->uart == UART_2) {
        __HAL_RCC_USART2_CLK_ENABLE();
        obj->index = 1;
    }

    if (obj->uart == LPUART_1) {
        __HAL_RCC_LPUART1_CLK_ENABLE();
        obj->index = 2;
    }

#if defined(USART4_BASE)
    if (obj->uart == UART_4) {
        __HAL_RCC_USART4_CLK_ENABLE();
        obj->index = 3;
    }
#endif

#if defined(USART5_BASE)
    if (obj->uart == UART_5) {
        __HAL_RCC_USART5_CLK_ENABLE();
        obj->index = 4;
    }
#endif

    // Configure the UART pins
    pinmap_pinout(tx, PinMap_UART_TX);
    pinmap_pinout(rx, PinMap_UART_RX);
    if (tx != NC) {
        pin_mode(tx, PullUp);
    }
    if (rx != NC) {
        pin_mode(rx, PullUp);
    }

    // Configure UART
    obj->baudrate = 9600;
    obj->databits = UART_WORDLENGTH_8B;
    obj->stopbits = UART_STOPBITS_1;
    obj->parity   = UART_PARITY_NONE;
    obj->pin_tx   = tx;
    obj->pin_rx   = rx;

    init_uart(obj);

    // For stdio management
    if (obj->uart == STDIO_UART) {
        stdio_uart_inited = 1;
        memcpy(&stdio_uart, obj, sizeof(serial_t));
    }
}

void serial_free(serial_t *obj)
{
    // Reset UART and disable clock
#if defined(USART1_BASE)
    if (obj->uart == UART_1) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_DISABLE();
    }
#endif

    if (obj->uart == UART_2) {
        __HAL_RCC_USART2_FORCE_RESET();
        __HAL_RCC_USART2_RELEASE_RESET();
        __HAL_RCC_USART2_CLK_DISABLE();
    }

    if (obj->uart == LPUART_1) {
        __HAL_RCC_LPUART1_FORCE_RESET();
        __HAL_RCC_LPUART1_RELEASE_RESET();
        __HAL_RCC_LPUART1_CLK_DISABLE();
    }

#if defined(USART4_BASE)
    if (obj->uart == UART_4) {
        __HAL_RCC_USART4_FORCE_RESET();
        __HAL_RCC_USART4_RELEASE_RESET();
        __HAL_RCC_USART4_CLK_DISABLE();
    }
#endif

#if defined(USART5_BASE)
    if (obj->uart == UART_5) {
        __HAL_RCC_USART5_FORCE_RESET();
        __HAL_RCC_USART5_RELEASE_RESET();
        __HAL_RCC_USART5_CLK_DISABLE();
    }
#endif

    // Configure GPIOs
    pin_function(obj->pin_tx, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
    pin_function(obj->pin_rx, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));

    serial_irq_ids[obj->index] = 0;
}

void serial_baud(serial_t *obj, int baudrate)
{
    obj->baudrate = baudrate;
    init_uart(obj);
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits)
{
    if (data_bits == 9) {
        obj->databits = UART_WORDLENGTH_9B;
    } else {
        obj->databits = UART_WORDLENGTH_8B;
    }

    switch (parity) {
        case ParityOdd:
        case ParityForced0:
            obj->parity = UART_PARITY_ODD;
            break;
        case ParityEven:
        case ParityForced1:
            obj->parity = UART_PARITY_EVEN;
            break;
        default: // ParityNone
            obj->parity = UART_PARITY_NONE;
            break;
    }

    if (stop_bits == 2) {
        obj->stopbits = UART_STOPBITS_2;
    } else {
        obj->stopbits = UART_STOPBITS_1;
    }

    init_uart(obj);
}

/******************************************************************************
 * INTERRUPTS HANDLING
 ******************************************************************************/

static void uart_irq(UARTName name, int id)
{
    UartHandle.Instance = (USART_TypeDef *)name;
    if (serial_irq_ids[id] != 0) {
        if (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC) != RESET) {
            irq_handler(serial_irq_ids[id], TxIrq);
            __HAL_UART_CLEAR_IT(&UartHandle, UART_CLEAR_TCF);
        }
        if (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET) {
            irq_handler(serial_irq_ids[id], RxIrq);
            volatile uint32_t tmpval = UartHandle.Instance->RDR; // Clear RXNE bit
        }
    }
}

#if defined(USART1_BASE)
static void uart1_irq(void)
{
    uart_irq(UART_1, 0);
}
#endif

static void uart2_irq(void)
{
    uart_irq(UART_2, 1);
}

static void lpuart1_irq(void)
{
    uart_irq(LPUART_1, 2);
}

#if defined(USART4_BASE)
static void uart4_irq(void)
{
    uart_irq(UART_4, 3);
}
#endif

#if defined(USART5_BASE)
static void uart5_irq(void)
{
    uart_irq(UART_5, 4);
}
#endif

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id)
{
    irq_handler = handler;
    serial_irq_ids[obj->index] = id;
}

void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable)
{
    IRQn_Type irq_n = (IRQn_Type)0;
    uint32_t vector = 0;

    UartHandle.Instance = (USART_TypeDef *)(obj->uart);

#if defined(USART1_BASE)
    if (obj->uart == UART_1) {
        irq_n = USART1_IRQn;
        vector = (uint32_t)&uart1_irq;
    }
#endif

    if (obj->uart == UART_2) {
        irq_n = USART2_IRQn;
        vector = (uint32_t)&uart2_irq;
    }

    if (obj->uart == LPUART_1) {
        irq_n = RNG_LPUART1_IRQn;
        vector = (uint32_t)&lpuart1_irq;
    }

#if defined(USART4_BASE)
    if (obj->uart == UART_4) {
        irq_n = USART4_5_IRQn;
        vector = (uint32_t)&uart4_irq;
    }
#endif

#if defined(USART5_BASE)
    if (obj->uart == UART_5) {
        irq_n = USART4_5_IRQn;
        vector = (uint32_t)&uart5_irq;
    }
#endif

    if (enable) {

        if (irq == RxIrq) {
            __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
        } else { // TxIrq
            __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_TC);
        }

        NVIC_SetVector(irq_n, vector);
        NVIC_EnableIRQ(irq_n);

    } else { // disable

        int all_disabled = 0;

        if (irq == RxIrq) {
            __HAL_UART_DISABLE_IT(&UartHandle, UART_IT_RXNE);
            // Check if TxIrq is disabled too
            if ((UartHandle.Instance->CR1 & USART_CR1_TCIE) == 0) all_disabled = 1;
        } else { // TxIrq
            __HAL_UART_DISABLE_IT(&UartHandle, UART_IT_TC);
            // Check if RxIrq is disabled too
            if ((UartHandle.Instance->CR1 & USART_CR1_RXNEIE) == 0) all_disabled = 1;
        }

        if (all_disabled) NVIC_DisableIRQ(irq_n);

    }
}

/******************************************************************************
 * READ/WRITE
 ******************************************************************************/

int serial_TxComplete(serial_t *obj)
{
    int status;
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);
    // Check if data is transmitted
    status = ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC) != RESET) ? 1 : 0);
    return status;
}

int serial_getc(serial_t *obj)
{
    USART_TypeDef *uart = (USART_TypeDef *)(obj->uart);
    while (!serial_readable(obj));
    return (int)(uart->RDR & (uint32_t)0xFF);
}

void serial_putc(serial_t *obj, int c)
{
    USART_TypeDef *uart = (USART_TypeDef *)(obj->uart);
    while (!serial_writable(obj));
    uart->TDR = (uint32_t)(c & (uint32_t)0xFF);
    while(!serial_TxComplete(obj));
}

int serial_readable(serial_t *obj)
{
    int status;
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);
    // Check if data is received
    status = ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET) ? 1 : 0);
    return status;
}

int serial_writable(serial_t *obj)
{
    int status;
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);
    // Check if data has moved to internal buffer
    status = ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE) != RESET) ? 1 : 0);
    return status;
}

void serial_clear(serial_t *obj)
{
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);
    __HAL_UART_CLEAR_IT(&UartHandle, UART_CLEAR_TCF);
    __HAL_UART_SEND_REQ(&UartHandle, UART_RXDATA_FLUSH_REQUEST);
}

void serial_pinout_tx(PinName tx)
{
    pinmap_pinout(tx, PinMap_UART_TX);
}

void serial_break_set(serial_t *obj)
{
    UartHandle.Instance = (USART_TypeDef *)(obj->uart);
    __HAL_UART_SEND_REQ(&UartHandle, UART_SENDBREAK_REQUEST);
}

void serial_break_clear(serial_t *obj)
{
}

#endif
