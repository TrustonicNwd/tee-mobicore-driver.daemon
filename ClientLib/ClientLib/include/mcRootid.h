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
 * Global definition of root ID.
 */

#ifndef MC_ROOTID_H_
#define MC_ROOTID_H_

#ifdef WIN32
#define _UNUSED
#else
#define _UNUSED __attribute__((unused))
#endif

/** Root Identifier type. */
typedef uint32_t mcRootid_t;

/** Reserved root id value 1. */
static _UNUSED const mcRootid_t MC_ROOTID_RESERVED1 = 0;

/** Reserved root id value 2. */
static _UNUSED const mcRootid_t MC_ROOTID_RESERVED2 = 0xFFFFFFFF;

/** Root id for system applications. */
static _UNUSED const mcRootid_t MC_ROOTID_SYSTEM = 0xFFFFFFFE;

/** Yet another test ROOT ID */
static _UNUSED const mcRootid_t MC_ROOTID_RESERVED3 = 0xFFFFFFFD;

/** GP TAs - used in the Trusted storage */
static _UNUSED const mcRootid_t MC_ROOTID_GP = 0xFFFFFFFC;

/** RTM's Root ID */
static _UNUSED const mcRootid_t MC_ROOTID_RTM = 0xFFFFFFFB;

#endif // MC_ROOTID_H_

