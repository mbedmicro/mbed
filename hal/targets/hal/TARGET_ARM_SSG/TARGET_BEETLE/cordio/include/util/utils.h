/*************************************************************************************************/
/*!
 *  \file   utils.h
 *
 *  \brief  Utility functions.
 *
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: LicenseRef-PBL
 *
 * This file and the related binary are licensed under the
 * Permissive Binary License, Version 1.0 (the "License");
 * you may not use these files except in compliance with the License.
 *
 * You may obtain a copy of the License here:
 * LICENSE-permissive-binary-license-1.0.txt and at
 * https://www.mbed.com/licenses/PBL-1.0
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*************************************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

/***************************************************************************************************
** INCLUDES
***************************************************************************************************/

#include <stdarg.h>
#include <stddef.h>

#include "wsf_types.h"

#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define PRINTF_ATTRIBUTE(a, b) __attribute__((format(printf, a, b)))
#else
#define PRINTF_ATTRIBUTE(a, b)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
** DEFINES
***************************************************************************************************/

#define UTIL_MAC_ADDR_LEN 6

/*------------------------------------------------------------------------------------------------*/

#define UTIL_IS_DIGIT(c)        ((c >= '0') && (c <= '9'))
#define UTIL_IS_XDIGIT(c)      (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || \
                                ((c >= 'A') && (c <= 'F')))

/*------------------------------------------------------------------------------------------------*/

#define UTIL_DIGIT_TO_INT(c)   (((c >= '0') && (c <= '9')) ? (uint8_t)(c - '0') : 0u)
#define UTIL_XDIGIT_TO_INT(c)  (((c >= '0') && (c <= '9')) ? (uint8_t)(c - '0') : \
                                ((c >= 'a') && (c <= 'f')) ? (uint8_t)(c - 'a' + 10u) : \
                                ((c >= 'A') && (c <= 'F')) ? (uint8_t)(c - 'A' + 10u) : 0u)

/***************************************************************************************************
** FUNCTIONS
***************************************************************************************************/

/*--------------------------------------------------------------------------------------------------
** Util_VSNPrintf()
**
** DESCRIPTION: Print formatted output to string.
**
** PARAMETERS:  s       Pointer to string that will receive outoput
**              size    Maximum number of characters to store in s
**              format  Pointer to format string
**              ap      Variable arguments
**
** RETURNS:     Number of characters stored in s
**------------------------------------------------------------------------------------------------*/
int Util_VSNPrintf(char *s, size_t size, const char *format, va_list ap) PRINTF_ATTRIBUTE(3, 0);

/*--------------------------------------------------------------------------------------------------
** Util_SNPrintf()
**
** DESCRIPTION: Print formatted output to string.
**
** PARAMETERS:  s       Pointer to string that will receive outoput
**              size    Maximum number of characters to store in s
**              format  Pointer to format string
**              ap      Variable arguments
**
** RETURNS:     Number of characters stored in s
**------------------------------------------------------------------------------------------------*/
int Util_SNPrintf (char *s, size_t size, const char *format, ...) PRINTF_ATTRIBUTE(3, 4);

/*--------------------------------------------------------------------------------------------------
** Util_ParseMacAddr()
**
** DESCRIPTION: Parse MAC address from string.
**
** PARAMETERS:  s     Pointer to string to parse
**              addr  Pointer to buffer that will receive MAC address
**
** RETURNS:     Number of characters consumed from string
**------------------------------------------------------------------------------------------------*/
int32_t Util_ParseMacAddr(const char *s, uint8_t (*addr)[UTIL_MAC_ADDR_LEN]);

/*--------------------------------------------------------------------------------------------------
** Util_ParseUInt()
**
** DESCRIPTION: Parse unsigned integer from string.
**
** PARAMETERS:  s     Pointer to string to parse
**              u     Pointer to variable that will receive integer
**              base  Base of integer (between 2 and 36, inclusive) or 0, for automatic detection
**
** RETURNS:     Number of characters consumed from string
**------------------------------------------------------------------------------------------------*/
int32_t Util_ParseUInt(const char *s, uint32_t *u, uint32_t base);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H */
