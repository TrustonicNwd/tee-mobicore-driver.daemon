/** @addtogroup CMP_COMMON
 * Common definitions of content management protocols (CMP) supported by the
 * content management trustlet (TlCm).
 *
 * @{
 *
 * @file
 * Common CMP global definitions.
 * Various components need access to (sub-)structures defined and used by CMP.
 * These common definitions are made available through this header file.
 *
 * Copyright © Trustonic Limited 2013.
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Trustonic Limited nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CMP_COMMON_H_
#define CMP_COMMON_H_

#include "mcContainer.h"
#include "mcUuid.h"
#include "mcVersionInfo.h"
#include "version.h"

/** Minimum TCI CMP 2.0 size. */
#define CMP_SIZE 4388

/** Default CMP MAP size. */
#define CMP_MAP_SIZE 2412

/** CMP version id. */
typedef uint32_t cmpVersionId_t;
/** CMP command id. */
typedef uint32_t cmpCommandId_t;
/** CMP response id. */
typedef uint32_t cmpResponseId_t;
/** CMP return code. */
typedef uint32_t cmpReturnCode_t;

/** Responses have bit 31 set */
#define RSP_ID_MASK (1U << 31)
#define RSP_ID(cmdId) (((uint32_t)(cmdId)) | RSP_ID_MASK)
#define IS_CMD(cmdId) ((((uint32_t)(cmdId)) & RSP_ID_MASK) == 0)
#define IS_RSP(cmdId) ((((uint32_t)(cmdId)) & RSP_ID_MASK) == RSP_ID_MASK)

/** CMP command header. */
typedef struct {
    /** Command id. */
    cmpCommandId_t commandId; 
} cmpCommandHeader_t;

/** CMP response header. */
typedef struct {
    /** Response id (must be command id | RSP_ID_MASK ). */
    cmpResponseId_t responseId;
    /** Return code. */
    cmpReturnCode_t returnCode;
} cmpResponseHeader_t;

/** Total number of bytes used for HMAC-SHA256 message authentication code. */
#define CMP_MAC_SIZE 32

/** Message authentication code. */
typedef struct {
    /** MAC value. */
    uint8_t mac[CMP_MAC_SIZE];
} cmpMac_t;

/** 64-bit random number. */
typedef struct {
    /** Random number value. */
    uint8_t data[8];
} cmpRnd8_t;

/** 256-bit random number. */
typedef struct {
    /** Random number value. */
    uint8_t data[32];
} cmpRnd32_t;

/** @defgroup MC_CMP_CMD_GET_VERSION MC_CMP_CMD_GET_VERSION
 * @{ */

/** Version tags. */
typedef enum {
    /** Version tag deprecated. */
    CMP_VERSION_TAG1 = 0x00000001,
    /** Version tag. */
    CMP_VERSION_TAG2 = 0x00000002,
} cmpVersionTag_t;

/** Version data info for deprecated version tag. */
typedef struct {
    /** Version data value. */
    cmpVersionId_t number;
} cmpVersionData1_t;

/** Version data info. */
typedef struct {
    /** Version data value. */
    mcVersionInfo_t versionInfo;
} cmpVersionData2_t;

/** Version data infos. */
typedef union {
    /** Version data info deprecated. */
    cmpVersionData1_t versionData1;
    /** Version data info. */
    cmpVersionData2_t versionData2;
} cmpVersionData_t;

/** @defgroup MC_CMP_CMD_GET_VERSION_CMD Command
 * @{ */

/** GetVersion command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
} cmpCmdGetVersion_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_GET_VERSION_RSP Response
 * @{ */

/** GetVersion response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Version tag. */
    cmpVersionTag_t tag;
    /** Version data info. */
    cmpVersionData_t data;
} cmpRspGetVersion_t;

/** @} */

/** @} */ 

/** @defgroup MC_CMP_CMD_GET_SUID MC_CMP_CMD_GET_SUID
 * @{ */

/** @defgroup MC_CMP_CMD_GET_SUID_CMD Command
 * @{ */

/** GetSuid command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
} cmpCmdGetSuid_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_GET_SUID_RSP Response
 * @{ */

/** GetSuid response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Suid. */
    mcSuid_t suid;
} cmpRspGetSuid_t;

/** @} */

/** @} */ 

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN MC_CMP_CMD_GENERATE_AUTH_TOKEN
 * @{ */

/** Block size of the encryption algorithm used for secure messaging. */
#define CMP_MSG_CRYPTO_BLOCK_SIZE 16

/** Total number of padding bytes required to encrypt data of given size. */
#define CMP_ED_PADDING(netsize) \
    (CMP_MSG_CRYPTO_BLOCK_SIZE - (netsize) % CMP_MSG_CRYPTO_BLOCK_SIZE)

/** Total number of bytes used for PSS signature in GENERATE AUTH TOKEN command. */
#define CMP_GEN_AUTH_TOKEN_PSS_SIZE 256

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN_CMD Command
 * @{ */

typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
    /** Suid. */
    mcSuid_t suid;
    /** Authentication key. */
    mcSymmetricKey_t kSocAuth;
    /** Key id. */
    uint32_t kid;
} cmpGenAuthTokenCmdSdata_t;

typedef struct {
    /** Signed data. */
    cmpGenAuthTokenCmdSdata_t sdata;
    /** Signature. */
    uint8_t pssSignature[CMP_GEN_AUTH_TOKEN_PSS_SIZE];
} cmpGenAuthTokenCmd_t;

/** GenAuthToken command. */
typedef struct {
    /** Command. */
    cmpGenAuthTokenCmd_t cmd;
} cmpCmdGenAuthToken_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN_RSP Response
 * @{ */

typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
} cmpGenAuthTokenRsp_t;

/** GenAuthToken response. */
typedef struct {
    /** Response. */
    cmpGenAuthTokenRsp_t rsp;
    /** AuthToken container. */
    mcSoAuthTokenCont_t soAuthCont;
} cmpRspGenAuthToken_t;

/** @} */ 

/** @} */ 

#endif // CMP_COMMON_H_

/** @} */
