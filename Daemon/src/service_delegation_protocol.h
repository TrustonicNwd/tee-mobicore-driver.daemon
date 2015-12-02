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

/**
 * @file   service_delegation_protocol.h
 * @brief  Delegation protocol definitions
 *
 */

#ifndef __SERVICE_DELEGATION_PROTOCOL_H__
#define __SERVICE_DELEGATION_PROTOCOL_H__



/* Instruction codes */
#define DELEGATION_INSTRUCTION_SHUTDOWN             0xF0
#define DELEGATION_INSTRUCTION_NOTIFY               0xE0

/* Partition-specific instruction codes (high-nibble encodes the partition identifier) */
#define DELEGATION_INSTRUCTION_PARTITION_CREATE     0x01
#define DELEGATION_INSTRUCTION_PARTITION_OPEN       0x02
#define DELEGATION_INSTRUCTION_PARTITION_READ       0x03
#define DELEGATION_INSTRUCTION_PARTITION_WRITE      0x04
#define DELEGATION_INSTRUCTION_PARTITION_SET_SIZE   0x05
#define DELEGATION_INSTRUCTION_PARTITION_SYNC       0x06
#define DELEGATION_INSTRUCTION_PARTITION_CLOSE      0x07
#define DELEGATION_INSTRUCTION_PARTITION_DESTROY    0x08

#define DELEGATION_NOTIFY_TYPE_ERROR                0x000000E1
#define DELEGATION_NOTIFY_TYPE_WARNING              0x000000E2
#define DELEGATION_NOTIFY_TYPE_INFO                 0x000000E3
#define DELEGATION_NOTIFY_TYPE_DEBUG                0x000000E4

typedef struct
{
   uint32_t nInstructionID;
} DELEGATION_GENERIC_INSTRUCTION;

typedef struct
{
   uint32_t nInstructionID;
   uint32_t nMessageType;
   uint32_t nMessageSize;
   char     nMessage[1];
} DELEGATION_NOTIFY_INSTRUCTION;

typedef struct
{
   uint32_t nInstructionID;
   uint32_t nSectorID;
   uint32_t nWorkspaceOffset;
} DELEGATION_RW_INSTRUCTION;

typedef struct
{
   uint32_t nInstructionID;
   uint32_t nNewSize;
} DELEGATION_SET_SIZE_INSTRUCTION;

typedef union
{
   DELEGATION_GENERIC_INSTRUCTION    sGeneric;
   DELEGATION_NOTIFY_INSTRUCTION     sNotify;
   DELEGATION_RW_INSTRUCTION         sReadWrite;
   DELEGATION_SET_SIZE_INSTRUCTION   sSetSize;
} DELEGATION_INSTRUCTION;

typedef struct
{
   uint32_t    nSyncExecuted;
   uint32_t    nPartitionErrorStates[16];
   uint32_t    nPartitionOpenSizes[16];
} DELEGATION_ADMINISTRATIVE_DATA;

#endif /* __SERVICE_DELEGATION_PROTOCOL_H__ */
