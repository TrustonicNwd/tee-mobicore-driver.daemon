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
 * Universally Unique Identifier.
 */

#ifndef MC_UUID_H_
#define MC_UUID_H_

#ifdef WIN32
#define _UNUSED
#else
#define _UNUSED __attribute__((unused))
#endif

#define UUID_TYPE

#define UUID_LENGTH 16
/** Universally Unique Identifier (UUID) according to ISO/IEC 11578. */
typedef struct {
    uint8_t value[UUID_LENGTH]; /**< Value of the UUID. */
} mcUuid_t, *mcUuid_ptr;

/** UUID value used as free marker in service provider containers. */
#define MC_UUID_FREE_DEFINE \
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

static _UNUSED const mcUuid_t MC_UUID_FREE = {
    MC_UUID_FREE_DEFINE
};

/** Reserved UUID. */
#define MC_UUID_RESERVED_DEFINE \
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

static _UNUSED const mcUuid_t MC_UUID_RESERVED = {
    MC_UUID_RESERVED_DEFINE
};

/** UUID for system applications. */
#define MC_UUID_SYSTEM_DEFINE \
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE }

static _UNUSED const mcUuid_t MC_UUID_SYSTEM = {
    MC_UUID_SYSTEM_DEFINE
};

#define MC_UUID_RTM_DEFINE \
    { 0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,       \
      0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34 }

static _UNUSED const mcUuid_t MC_UUID_RTM = {
    MC_UUID_RTM_DEFINE
};

/**
 * TODO: Replace with v5 UUID (milestone #3)
 */
#define LTA_UUID_DEFINE \
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         \
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}

#endif // MC_UUID_H_

