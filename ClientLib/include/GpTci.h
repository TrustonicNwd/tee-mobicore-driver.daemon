/*
 * Copyright (c) 2013-2015 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef _GP_TCI_H_
#define _GP_TCI_H_

typedef struct {
    uint32_t   a;
    uint32_t   b;
} TEE_Value;

typedef struct {
    uint32_t    sVirtualAddr;         /**< The virtual address of the Bulk buffer regarding the address space of the Trustlet, already includes a possible offset! */
    uint32_t    sVirtualLen;       /**< Length of the mapped Bulk buffer */
    uint32_t    outputSize;
} _TEEC_MemoryReferenceInternal;

typedef union {
    TEE_Value                      value;
    _TEEC_MemoryReferenceInternal   memref;
} _TEEC_ParameterInternal;

typedef enum {
    _TA_OPERATION_OPEN_SESSION =    1,
    _TA_OPERATION_INVOKE_COMMAND =  2,
    _TA_OPERATION_CLOSE_SESSION =   3,
} _TEEC_TCI_type;

typedef struct {
    _TEEC_TCI_type          type;
    uint32_t                commandId;
    uint32_t                paramTypes;
    _TEEC_ParameterInternal params[4];
    bool                    isCancelled;
} _TEEC_OperationInternal;

typedef struct {
    char header[8];// = "TCIGP000"`: version indicator (to support future format changes)
    TEEC_UUID destination;
    _TEEC_OperationInternal operation; //the data of the ongoing operation (if any)
    uint32_t ready;
    // The following fields are set by the secure world (in a future version, they may also be set by the normal world communication layer):
    uint32_t returnOrigin;
    uint32_t returnStatus;
} _TEEC_TCI;

/**
 * Termination codes
 */
#define TA_EXIT_CODE_PANIC  (300)
#define TA_EXIT_CODE_TCI    (301)
#define TA_EXIT_CODE_PARAMS (302)
#define TA_EXIT_CODE_FINISHED       (303)
#define TA_EXIT_CODE_SESSIONSTATE   (304)
#define TA_EXIT_CODE_CREATEFAILED   (305)

#endif // _GP_TCI_H_
