/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef MBED_PINNAMES_H
#define MBED_PINNAMES_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NC_VAL (int)0xFFFFFFFF

typedef enum {
    // Digital naming
    D16 = 16,
    D31 = 31,
    D45 = 45,
    D44 = 44,

    // Analog naming
    A16 = D16,
    A31 = D31,

    // LEDs
    LED_RED     = AM_BSP_GPIO_LED_RED,
    LED_BLUE    = AM_BSP_GPIO_LED_BLUE,
    LED_GREEN   = AM_BSP_GPIO_LED_GREEN,
    LED_YELLOW  = AM_BSP_GPIO_LED_YELLOW,

    // mbed original LED naming
    LED1 = AM_BSP_GPIO_LED0,
    LED2 = AM_BSP_GPIO_LED1,
    LED3 = AM_BSP_GPIO_LED2,
    LED4 = AM_BSP_GPIO_LED3,
    
    // Qwiic
    QWIIC_SCL = 39,
    QWIIC_SDA = 40,
    
    // UART
    SERIAL_TX = 48,
    SERIAL_RX = 49,
    USBTX = SERIAL_TX,
    USBRX = SERIAL_RX,
        
    // Not connected
    NC = NC_VAL
} PinName;

#ifdef __cplusplus
}
#endif

#endif
