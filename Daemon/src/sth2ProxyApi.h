/*
 * Copyright (c) 2015 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef STH2_PROXY_API_H_
#define STH2_PROXY_API_H_


#include "drSecureFS_Api.h"


/**
 * Termination codes
 */
#define EXIT_ERROR ((uint32_t)(-1))

/**
 * TA UUID.
 */
#define SERVICE_DELEGATION_UUID { { 0x07, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20 } }

#endif // STH2_PROXY_API_H_
