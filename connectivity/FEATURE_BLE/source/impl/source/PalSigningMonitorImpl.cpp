/* mbed Microcontroller Library
 * Copyright (c) 2006-2020 ARM Limited
 *
 * SPDX-License-Identifier: Apache-2.0
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

#include "source/pal/PalSigningMonitor.h"
#include "source/BLEInstanceBase.h"

namespace ble {

void PalSigningMonitor::set_signing_event_handler(PalSigningMonitorEventHandler *handler)
{
#if BLE_FEATURE_GATT_CLIENT
    BLEInstanceBase::deviceInstance().getGattClientImpl().set_signing_event_handler(handler);
#endif // BLE_FEATURE_GATT_CLIENT
#if BLE_FEATURE_GATT_SERVER
    BLEInstanceBase::deviceInstance().getGattServerImpl().set_signing_event_handler(handler);
#endif // BLE_FEATURE_GATT_SERVER
}

}