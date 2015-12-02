/*
 * Copyright (c) 2013-2014 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
/**
 * service provider ID.
 */

#ifndef MC_SPID_H_
#define MC_SPID_H_

#ifdef WIN32
#define _UNUSED
#else
#define _UNUSED __attribute__((unused))
#endif

/** Service provider Identifier type. */
typedef uint32_t mcSpid_t;

/** SPID value used as free marker in root containers. */
static _UNUSED const mcSpid_t MC_SPID_FREE = 0xFFFFFFFF;

/** Reserved SPID value. */
static _UNUSED const mcSpid_t MC_SPID_RESERVED = 0;

/** SPID for system applications. */
static _UNUSED const mcSpid_t MC_SPID_SYSTEM = 0xFFFFFFFE;

/** SPID reserved for tests only */
static _UNUSED const mcSpid_t MC_SPID_RESERVED_TEST = 0xFFFFFFFD;
static _UNUSED const mcSpid_t MC_SPID_TRUSTONIC_TEST = 0x4;

/** SPID reserved for OTA development */
static _UNUSED const mcSpid_t MC_SPID_TRUSTONIC_OTA = 0x2A;

/** GP TAs - stored in the trusted storage. They all share the same */
static _UNUSED const mcSpid_t MC_SPID_GP = 0xFFFFFFFC;

/** RTM's SPID */
static _UNUSED const mcSpid_t MC_SPID_RTM = 0xFFFFFFFB;

#endif // MC_SPID_H_

