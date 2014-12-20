/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "mbed_assert.h"
#include "analogin_api.h"

#include "cmsis.h"
#include "pinmap.h"
#include "clk_freqs.h"

#define MAX_FADC        6000000

static const PinMap PinMap_ADC[] = {
    {PTC2, ADC0_SE4b, 0},
    {PTD1, ADC0_SE5b, 0},
    {PTD5, ADC0_SE6b, 0},
    {PTD6, ADC0_SE7b, 0},
    {PTB0, ADC0_SE8,  0},
    {PTB1, ADC0_SE9,  0},
    {PTB2, ADC0_SE12, 0},
    {PTB3, ADC0_SE13, 0},
    {PTC0, ADC0_SE14, 0},
    {PTC1, ADC0_SE15, 0},
    {PTC8, ADC1_SE4b, 0}, // added these
    {PTC9, ADC1_SE5b, 0},
    {PTC10,ADC1_SE6b, 0},
    {PTC11,ADC1_SE7b, 0},    
    {NC,   NC,        0}
};

void analogin_init(analogin_t *obj, PinName pin) {
    obj->adc = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
    MBED_ASSERT(obj->adc != (ADCName)NC);

    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    uint32_t port = (uint32_t)pin >> PORT_SHIFT;
    SIM->SCGC5 |= 1 << (SIM_SCGC5_PORTA_SHIFT + port);

    // bus clk
    uint32_t PCLK = bus_frequency();
    uint32_t clkdiv;
    for (clkdiv = 0; clkdiv < 4; clkdiv++) {
        if ((PCLK >> clkdiv) <= MAX_FADC)
            break;
    }
    if (clkdiv == 4)                    //Set max div
        clkdiv = 0x7;

    ADC0->SC1[1] = ADC_SC1_ADCH(obj->adc);

    ADC0->CFG1 = ADC_CFG1_ADLPC_MASK    // Low-Power Configuration
               | ADC_CFG1_ADIV(clkdiv & 0x3)       // Clock Divide Select
               | ADC_CFG1_ADLSMP_MASK   // Long Sample Time
               | ADC_CFG1_MODE(3)       // (16)bits Resolution
               | ADC_CFG1_ADICLK(clkdiv >> 2);    // Input Clock

    ADC0->CFG2 = ADC_CFG2_MUXSEL_MASK   // ADxxb or ADxxa channels
               | ADC_CFG2_ADACKEN_MASK  // Asynchronous Clock Output Enable
               | ADC_CFG2_ADHSC_MASK    // High-Speed Configuration
               | ADC_CFG2_ADLSTS(0);    // Long Sample Time Select

    ADC0->SC2 = ADC_SC2_REFSEL(0);      // Default Voltage Reference

    ADC0->SC3 = ADC_SC3_AVGE_MASK       // Hardware Average Enable
              | ADC_SC3_AVGS(0);        // 4 Samples Averaged

    pinmap_pinout(pin, PinMap_ADC);
}

uint16_t analogin_read_u16(analogin_t *obj) {
    // start conversion
    ADC0->SC1[0] = ADC_SC1_ADCH(obj->adc);

    // Wait Conversion Complete
    while ((ADC0->SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK);

    return (uint16_t)ADC0->R[0];
}

float analogin_read(analogin_t *obj) {
    uint16_t value = analogin_read_u16(obj);
    return (float)value * (1.0f / (float)0xFFFF);
}

