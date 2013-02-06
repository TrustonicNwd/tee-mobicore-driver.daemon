/** @addtogroup CMP
 * Content Management Protocol Definitions.
 *
 * The CMP (Content Management Protocol) is based on the TCI (Trustlet Control
 * Interface) and CMP mapped interfaces. It defines commands/responses with
 * the CMTL (Content Management Trustlet).
 *
 * @{
 *
 * @file
 * CMP TCI global definitions.
 * Various components need access to (sub-)structures defined and used by CMP;
 * these common definitions are made available through this header file.
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

#ifndef CMP_H_
#define CMP_H_

#include "TlCm/cmpCommon.h"

/** Info of whole mapped memory with NWd */
typedef struct {
    void* addr;
    uint32_t len; 
} cmpMapInfo_t;

/** Info of mapped element from a CMP message with NWd */
typedef struct {
    uint32_t offset;
    uint32_t len; 
} cmpMapOffsetInfo_t;

/**
 * TCI CMP command header.
 */
typedef struct {
    /** CMP version. */
    cmpVersionId_t version;
    /** Command ID. */
    cmpCommandId_t commandId;
    /** Info of whole mapped memory with NWd */
    cmpMapInfo_t mapInfo;
    /** Info of mapped CMP command with NWd */
    cmpMapOffsetInfo_t cmpCmdMapOffsetInfo;
} cmpCommandHeaderTci_t;

/**
 * TCI CMP response header.
 */
typedef struct {
    /** CMP version. */
    cmpVersionId_t version;
    /** Response ID (must be command ID | RSP_ID_MASK ). */
    cmpResponseId_t responseId; 
    /** Required lenght (0 if lenght is enough) of mapped memory with NWd. */
    uint32_t len; 
} cmpResponseHeaderTci_t;

/** @defgroup MC_CMP_CMD_GET_VERSION
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGetVersionTci_t;

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspGetVersionTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_GET_SUID
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGetSuidTci_t;

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspGetSuidTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGenAuthTokenTci_t;

typedef struct {
    cmpResponseHeaderTci_t rspHeaderTci;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped AuthTokenCont with NWd */
    cmpMapOffsetInfo_t cmpAuthTokenContMapOffsetInfo;
} cmpRspGenAuthTokenTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped AuthTokenCont with NWd */
    cmpMapOffsetInfo_t cmpAuthTokenContMapOffsetInfo;
} cmpCmdBeginSocAuthenticationTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginSocAuthenticationTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpCmdBeginRootAuthenticationTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginRootAuthenticationTci_t;

/** @} */ 

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdBeginSpAuthenticationTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginSpAuthenticationTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE 
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdAuthenticateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspAuthenticateTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE 
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdAuthenticateTerminateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspAuthenticateTerminateTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContRegisterActivateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContRegisterActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContUnregisterTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspRootContUnregisterTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContLockByRootTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContLockByRootTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContUnlockByRootTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContUnlockByRootTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContRegisterActivateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContRegisterActivateTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContUnregisterTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspSpContUnregisterTci_t;

/** @} */ 

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContRegisterTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped RootCont with NWd */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContRegisterTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdSpContLockByRootTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContLockByRootTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdSpContUnlockByRootTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContUnlockByRootTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContActivateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContActivateTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContLockBySpTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContLockBySpTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContUnlockBySpTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContUnlockBySpTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContRegisterActivateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContRegisterActivateTci_t;

/** @} */

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContUnregisterTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspTltContUnregisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContRegisterTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped SpCont with NWd */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContRegisterTci_t;

/** @} */ 

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContActivateTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContActivateTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContLockBySpTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContLockBySpTci_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContUnlockBySpTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContUnlockBySpTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of mapped TltCont with NWd */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContPersonalizeTci_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONLIZE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of mapped CMP response with NWd */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of mapped PersonalizeData with NWd */
    cmpMapOffsetInfo_t cmpTltContPersoMapOffsetInfo;
} cmpRspTltContPersonalizeTci_t;

/** @} */

/** @} */

#endif // CMP_H_

/** @} */
