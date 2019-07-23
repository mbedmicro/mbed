/*
 * Copyright (c) 2018-2019, Arm Limited and affiliates.
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

#ifndef MBED_LINKED_LIST_H
#define MBED_LINKED_LIST_H

#include "LinkEntry.h"
#include "LinkedListBase.h"

/** \defgroup drivers-internal-api-list List
 * \ingroup drivers-internal-api
 */

/**
 * \defgroup drivers_LinkedList LinkedList class
 * \ingroup drivers-internal-api-list
 * @{
 */
template<class T>
class LinkedList: public LinkedListBase {
public:
    LinkedList() {}
    ~LinkedList() {}

    /**
     * Return the element at the head of the list
     *
     * @return The element at the head of the list or NULL if the list is empty
     */
    T *head()
    {
        return static_cast<T *>(LinkedListBase::head());
    }

    /**
     * Add an element to the tail of the list
     *
     * @param entry New element to add
     */
    void enqueue(T *entry)
    {
        LinkedListBase::enqueue(static_cast<LinkEntry *>(entry));
    }

    /**
     * Remove the element at the head of the list
     *
     * @return The element at the head of the list or NULL if the list is empty
     */
    T *dequeue()
    {
        return static_cast<T *>(LinkedListBase::dequeue());
    }
};

/** @}*/

#endif
