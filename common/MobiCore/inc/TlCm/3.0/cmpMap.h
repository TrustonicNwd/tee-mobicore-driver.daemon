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
 * CMP mapped global definitions.
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

#ifndef CMP_MAP_H_
#define CMP_MAP_H_

#include "TlCm/3.0/cmp.h"

/**
 * Map CMP GetVersion message.
 */
typedef union {
    cmpCmdGetVersion_t cmpCmdGetVersion;
    cmpRspGetVersion_t cmpRspGetVersion;
} cmpMapGetVersion_t;

/**
 * Map CMP GetSuid message.
 */
typedef union {
    cmpCmdGetSuid_t cmpCmdGetSuid;
    cmpRspGetSuid_t cmpRspGetSuid;
} cmpMapGetSuid_t;

/**
 * Map CMP GenAuthToken message.
 */
typedef union {
    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    cmpRspGenAuthToken_t cmpRspGenAuthToken;
} cmpMapGenAuthToken_t;

/**
 * Map CMP command header.
 */
typedef struct {
    cmpCommandId_t commandId;
    uint32_t len;
    uint32_t reserved;
} cmpCommandHeaderMap_t;

/**
 * Map CMP response header.
 */
typedef struct {
    cmpResponseId_t responseId;
    cmpReturnCode_t returnCode;
    uint32_t len;
    uint32_t reserved;
} cmpResponseHeaderMap_t;

/**
 * Map CMP message header.
 */
typedef union {
    cmpCommandHeaderMap_t cmdHeader;
    cmpResponseHeaderMap_t rspHeader;
} cmpMapHeader_t;

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
} cmpBeginSocAuthenticationCmd_t;

/** BeginSocAuthentication command. */
typedef struct {
    cmpBeginSocAuthenticationCmd_t cmd;
    /* AuthTokenCont mapping info is in TCI cmpCmdBeginSocAuthenticationTci */
} cmpCmdBeginSocAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    mcSuid_t suid;
    cmpRnd8_t rnd1;
} cmpBeginSocAuthenticationRspSdata_t;

typedef struct {
    cmpBeginSocAuthenticationRspSdata_t sdata;
    cmpMac_t mac;
} cmpBeginSocAuthenticationRsp_t;

/** BeginSocAuthentication response. */
typedef struct {
    cmpBeginSocAuthenticationRsp_t rsp;
} cmpRspBeginSocAuthentication_t;

/** @} */

/**
 * Map CMP BeginSocAuthentication message.
 */
typedef union {
    cmpCmdBeginSocAuthentication_t cmpCmdBeginSocAuthentication;
    cmpRspBeginSocAuthentication_t cmpRspBeginSocAuthentication;
} cmpMapBeginSocAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
} cmpBeginRootAuthenticationCmd_t;

/** BeginRootAuthentication command. */
typedef struct {
    cmpBeginRootAuthenticationCmd_t cmd;
    /* RootCont mapping info is in TCI cmpCmdBeginRootAuthenticationTci */
} cmpCmdBeginRootAuthentication_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    mcSuid_t suid;
    cmpRnd8_t rnd1;
} cmpBeginRootAuthenticationRspSdata_t;

typedef struct {
    cmpBeginRootAuthenticationRspSdata_t sdata;
    cmpMac_t mac;
} cmpBeginRootAuthenticationRsp_t;

/** BeginRootAuthentication response. */
typedef struct {
    cmpBeginRootAuthenticationRsp_t rsp;
} cmpRspBeginRootAuthentication_t;

/** @} */ 

/**
 * Map CMP BeginRootAuthentication message.
 */
typedef union {
    cmpCmdBeginRootAuthentication_t cmpCmdBeginRootAuthentication;
    cmpRspBeginRootAuthentication_t cmpRspBeginRootAuthentication;
} cmpMapBeginRootAuthentication_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
} cmpBeginSpAuthenticationCmd_t;

/** BeginSpAuthentication command. */
typedef struct {
    cmpBeginSpAuthenticationCmd_t cmd;
    /* RootCont mapping info is in TCI cmpCmdBeginSpAuthenticationTci */
    /* SpCont mapping info is in TCI cmpCmdBeginSpAuthenticationTci */
} cmpCmdBeginSpAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    mcSuid_t suid;
    mcSpid_t spid;
    cmpRnd8_t rnd1;
} cmpBeginSpAuthenticationRspSdata_t;

typedef struct {
    cmpBeginSpAuthenticationRspSdata_t sdata;
    cmpMac_t mac;
} cmpBeginSpAuthenticationRsp_t;

/** BeginSpAuthentication response. */
typedef struct {
    cmpBeginSpAuthenticationRsp_t rsp;
} cmpRspBeginSpAuthentication_t;

/** @} */ 

/**
 * Map CMP BeginSpAuthentication message.
 */
typedef union {
    cmpCmdBeginSpAuthentication_t cmpCmdBeginSpAuthentication;
    cmpRspBeginSpAuthentication_t cmpRspBeginSpAuthentication;
} cmpMapBeginSpAuthentication_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE 
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_CMD Command
 * @{ */

typedef struct {
    cmpRnd8_t rnd2;
    mcSuid_t suid;
    uint32_t entityId;
    cmpRnd8_t rnd1;
    cmpRnd32_t k2;
} cmpAuthMsgEdata_t;

typedef struct {
    cmpAuthMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpAuthMsgEdata_t))];
} cmpAuthCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    uint32_t edLen;
    cmpAuthCmdEd_t ed;
} cmpAuthCmdSdata_t; 

typedef struct {
    cmpAuthCmdSdata_t sdata;
    cmpMac_t mac;
} cmpAuthenticateCmd_t;

/** Authenticate command. */
typedef struct {
    cmpAuthenticateCmd_t cmd;
} cmpCmdAuthenticate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_RSP Response
 * @{ */

typedef struct {
    cmpRnd8_t rnd2;
    mcSuid_t suid;
    uint32_t entityId;
    cmpRnd8_t rnd1;
    cmpRnd32_t k1;
} cmpAuthRspEdata_t;

typedef struct {
    cmpAuthRspEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpAuthRspEdata_t))];
} cmpAuthRspEd_t;

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    uint32_t edLen;
    cmpAuthRspEd_t ed;
} cmpAuthRspSdata_t;

typedef struct {
    cmpAuthRspSdata_t sdata;
    cmpMac_t mac;
} cmpAuthenticateRsp_t;

/** Authenticate response. */
typedef struct {
    cmpAuthenticateRsp_t rsp;
} cmpRspAuthenticate_t;

/** @} */

/**
 * Map CMP Authenticate message.
 */
typedef union {
    cmpCmdAuthenticate_t cmpCmdAuthenticate;
    cmpRspAuthenticate_t cmpRspAuthenticate;
} cmpMapAuthenticate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
} cmpAuthenticateTerminateCmdSdata_t;

typedef struct {
    cmpAuthenticateTerminateCmdSdata_t sdata;
    cmpMac_t mac;
} cmpAuthenticateTerminateCmd_t;

/** AuthenticateTerminate command. */
typedef struct {
    cmpAuthenticateTerminateCmd_t cmd;
} cmpCmdAuthenticateTerminate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
} cmpAuthenticateTerminateRspSdata_t;

typedef struct {
    cmpAuthenticateTerminateRspSdata_t sdata;
    cmpMac_t mac;
} cmpAutenticateTerminateRsp_t;

/** AuthenticateTerminate response. */
typedef struct {
    cmpAutenticateTerminateRsp_t rsp;
} cmpRspAuthenticateTerminate_t;

/** @} */

/**
 * Map CMP AuthenticateTerminate message.
 */
typedef union {
    cmpCmdAuthenticateTerminate_t cmpCmdAuthenticateTerminate;
    cmpRspAuthenticateTerminate_t cmpRspAuthenticateTerminate;
} cmpMapAuthenticateTerminate_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kRootAuth;
} cmpRootRegActMsgEdata_t;

typedef struct {
    cmpRootRegActMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootRegActMsgEdata_t))];
} cmpRootRegActCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcRootid_t rootid;
    uint32_t edLen;
    cmpRootRegActCmdEd_t ed;
} cmpRootRegActCmdSdata_t;

typedef struct {
    cmpRootRegActCmdSdata_t sdata;
    cmpMac_t mac;
} cmpRootContRegisterActivateCmd_t;

/** RootContRegisterActivate command. */
typedef struct {
    cmpRootContRegisterActivateCmd_t cmd;
} cmpCmdRootContRegisterActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info is in TCI cmpRspRootContRegisterActivateTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
} cmpRootRegActRspSdata_t;

typedef struct {
    cmpRootRegActRspSdata_t sdata;
    cmpMac_t mac;
} cmpRootContRegisterActivateRsp_t;

/** RooContRegisterActivate response. */
typedef struct {
    cmpRootContRegisterActivateRsp_t rsp;
} cmpRspRootContRegisterActivate_t;

/** @} */

/**
 * Map CMP RootContRegisterActivate message.
 */
typedef union {
    cmpCmdRootContRegisterActivate_t cmpCmdRootContRegisterActivate;
    cmpRspRootContRegisterActivate_t cmpRspRootContRegisterActivate;
} cmpMapRootContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    mcSuid_t suid;
} cmpRootUnregMsgEdata_t;

typedef struct {
    cmpRootUnregMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootUnregMsgEdata_t))];
} cmpRootUnregCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    uint32_t edLen;
    cmpRootUnregCmdEd_t ed;
} cmpRootUnregCmdSdata_t;

typedef struct {
    cmpRootUnregCmdSdata_t sdata;
    cmpMac_t mac;
} cmpRootContUnregisterCmd_t;

/** RootContUnregister command. */
typedef struct {
    cmpRootContUnregisterCmd_t cmd;
} cmpCmdRootContUnregister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    mcSuid_t suid;
} cmpRootUnregRspEdata_t;

typedef struct {
    cmpRootUnregRspEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootUnregRspEdata_t))];
} cmpRootUnregRspEd_t;

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    uint32_t edLen;
    cmpRootUnregRspEd_t ed;
} cmpRootUnregRspSdata_t;

typedef struct {
    cmpRootUnregRspSdata_t sdata;
    cmpMac_t mac;
} cmpRootContUnregisterRsp_t;

/** RootContUnregister response. */
typedef struct {
    cmpRootContUnregisterRsp_t rsp;
} cmpRspRootContUnregister_t;

/** @} */ 

/**
 * Map CMP RootContUnregister message.
 */
typedef union {
    cmpCmdRootContUnregister_t cmpCmdRootContUnregister;
    cmpRspRootContUnregister_t cmpRspRootContUnregister;
} cmpMapRootContUnregister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
} cmpRootLockByRootCmdSdata_t;

typedef struct {
    cmpRootLockByRootCmdSdata_t sdata;
    cmpMac_t mac;
} cmpRootContLockByRootCmd_t;

/** RootContLockByRoot command. */
typedef struct {
    cmpRootContLockByRootCmd_t cmd;
} cmpCmdRootContLockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info is in TCI cmpRspRootContLockByRootTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
} cmpRootLockByRootRspSdata_t;

typedef struct {
    cmpRootLockByRootRspSdata_t sdata;
    cmpMac_t mac;
} cmpRootContLockByRootRsp_t;

/** RootContLockByRoot response. */
typedef struct {
    cmpRootContLockByRootRsp_t rsp;
} cmpRspRootContLockByRoot_t;

/** @} */ 

/**
 * Map CMP RootContLockByRoot message.
 */
typedef union {
    cmpCmdRootContLockByRoot_t cmpCmdRootContLockByRoot;
    cmpRspRootContLockByRoot_t cmpRspRootContLockByRoot;
} cmpMapRootContLockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
} cmpRootUnlockByRootCmdSdata_t;

typedef struct {
    cmpRootUnlockByRootCmdSdata_t sdata;
    cmpMac_t mac;
} cmpRootContUnlockByRootCmd_t;

/** RootContUnlockByRoot command. */
typedef struct {
    cmpRootContUnlockByRootCmd_t cmd;
} cmpCmdRootContUnlockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info are in TCI cmpRspRootContUnlockByRootTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
} cmpRootUnlockByRootRspSdata_t;

typedef struct {
    cmpRootUnlockByRootRspSdata_t sdata;
    cmpMac_t mac;
} cmpRootContUnlockByRootRsp_t;

/** RootContUnlockByRoot response. */
typedef struct {
    cmpRootContUnlockByRootRsp_t rsp;
} cmpRspRootContUnlockByRoot_t;

/** @} */ 

/**
 * Map CMP RootContUnlockByRoot message.
 */
typedef union {
    cmpCmdRootContUnlockByRoot_t cmpCmdRootContUnlockByRoot;
    cmpRspRootContUnlockByRoot_t cmpRspRootContUnlockByRoot;
} cmpMapRootContUnlockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kSpAuth;
} cmpSpRegActMsgEdata_t;

typedef struct {
    cmpSpRegActMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegActMsgEdata_t))];
} cmpSpRegActCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    uint32_t edLen;
    cmpSpRegActCmdEd_t ed;
} cmpSpRegActCmdSdata_t;

typedef struct {
    cmpSpRegActCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContRegisterActivateCmd_t;

/** SpContRegisterActivate command. */
typedef struct {
    cmpSpContRegisterActivateCmd_t cmd;
} cmpCmdSpContRegisterActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info is in TCI cmpRspSpContRegisterActivateTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
    /* SpCont mapping info are in TCI cmpRspSpContRegisterActivateTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpRegActRspSdata_t;

typedef struct {
    cmpSpRegActRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContRegisterActivateRsp_t;

/** SpContRegisterActivate response. */
typedef struct {
    cmpSpContRegisterActivateRsp_t rsp;
} cmpRspSpContRegisterActivate_t;

/** @} */ 

/**
 * Map CMP SpContRegisterActivate message.
 */
typedef union {
    cmpCmdSpContRegisterActivate_t cmpCmdSpContRegisterActivate;
    cmpRspSpContRegisterActivate_t cmpRspSpContRegisterActivate;
} cmpMapSpContRegisterActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
} cmpSpContUnregCmdSdata_t;

typedef struct {
    cmpSpContUnregCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnregisterCmd_t;

/** SpContUnregister command. */
typedef struct {
    cmpSpContUnregisterCmd_t cmd;
} cmpCmdSpContUnregister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info is in cmpRspSpContUnregisterTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
} cmpSpContUnregRspSdata_t;

typedef struct {
    cmpSpContUnregRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnregisterRsp_t;

/** SpContUnregister response. */
typedef struct {
    cmpSpContUnregisterRsp_t rsp;
} cmpRspSpContUnregister_t;

/** @} */ 

/**
 * Map CMP SpContUnregister message.
 */
typedef union {
    cmpCmdSpContUnregister_t cmpCmdSpContUnregister;
    cmpRspSpContUnregister_t cmpRspSpContUnregister;
} cmpMapSpContUnregister_t;
/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kSpAuth;
} cmpSpRegisterMsgEdata_t;

typedef struct {
    cmpSpRegisterMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegisterMsgEdata_t))];
} cmpSpRegisterCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    uint32_t edLen;
    cmpSpRegisterCmdEd_t ed;
} cmpSpRegisterCmdSdata_t;

typedef struct {
    cmpSpRegisterCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContRegisterCmd_t;

/** SpContRegister command. */
typedef struct {
    cmpSpContRegisterCmd_t cmd;
} cmpCmdSpContRegister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* RootCont mapping info is in TCI cmpRspSpContRegisterTci */
    uint32_t soRootContLen;
    mcSoRootCont_t soRootCont;
    /* SpCont mapping info is in TCI cmpRspSpContRegisterTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpRegisterRspSdata_t;

typedef struct {
    cmpSpRegisterRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContRegisterRsp_t;

/** SpContRegister response. */
typedef struct {
    cmpSpContRegisterRsp_t rsp;
} cmpRspSpContRegister_t;

/** @} */ 

/**
 * Map CMP SpContRegister message.
 */
typedef union {
    cmpCmdSpContRegister_t cmpCmdSpContRegister;
    cmpRspSpContRegister_t cmpRspSpContRegister;
} cmpMapSpContRegister_t;

/** @} */ 

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader; 
    mcSpid_t spid;
} cmpSpLockByRootCmdSdata_t;

typedef struct {
    cmpSpLockByRootCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContLockByRootCmd_t;

/** SpContLockByRoot command. */
typedef struct {
    cmpSpContLockByRootCmd_t cmd;
} cmpCmdSpContLockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspSpContLockByRootTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpLockByRootRspSdata_t;

typedef struct {
    cmpSpLockByRootRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContLockByRootRsp_t;

/** SpContLockByRoot response. */
typedef struct {
    cmpSpContLockByRootRsp_t rsp;
} cmpRspSpContLockByRoot_t;

/** @} */ 

/**
 * Map CMP SpContLockByRoot message.
 */
typedef union {
    cmpCmdSpContLockByRoot_t cmpCmdSpContLockByRoot;
    cmpRspSpContLockByRoot_t cmpRspSpContLockByRoot;
} cmpMapSpContLockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT 
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader; 
    mcSpid_t spid;
} cmpSpUnlockByRootCmdSdata_t;

typedef struct {
    cmpSpUnlockByRootCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnlockByRootCmd_t;

/** SpContUnlockByRoot command. */
typedef struct {
    cmpSpContUnlockByRootCmd_t cmd;
} cmpCmdSpContUnlockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspSpContUnlockByRootTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpUnlockByRootRspSdata_t;

typedef struct {
    cmpSpUnlockByRootRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnlockByRootRsp_t;

/** SpContUnlockByRoot response. */
typedef struct {
    cmpSpContUnlockByRootRsp_t rsp;
} cmpRspSpContUnlockByRoot_t;

/** @} */ 

/**
 * Map CMP SpContUnlockByRoot message.
 */
typedef union {
    cmpCmdSpContUnlockByRoot_t cmpCmdSpContUnlockByRoot;
    cmpRspSpContUnlockByRoot_t cmpRspSpContUnlockByRoot;
} cmpMapSpContUnlockByRoot_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kSpAuth;
} cmpSpActivateMsgEdata_t;

typedef struct {
    cmpSpActivateMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpActivateMsgEdata_t))];
} cmpSpActivateCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    uint32_t edLen;
    cmpSpActivateCmdEd_t ed;
} cmpSpActivateCmdSdata_t;

typedef struct {
    cmpSpActivateCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContActivateCmd_t;

/** SpContActivate command. */
typedef struct {
    cmpSpContActivateCmd_t cmd;
} cmpCmdSpContActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspSpContActivateTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpActivateRspSdata_t;

typedef struct {
    cmpSpActivateRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContActivateRsp_t;

/** SpContActivate response. */
typedef struct {
    cmpSpContActivateRsp_t rsp;
} cmpRspSpContActivate_t;

/** @} */ 

/**
 * Map CMP SpContActivate message.
 */
typedef union {
    cmpCmdSpContActivate_t cmpCmdSpContActivate;
    cmpRspSpContActivate_t cmpRspSpContActivate;
} cmpMapSpContActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader; 
    mcSpid_t spid;
} cmpSpLockBySpCmdSdata_t;

typedef struct {
    cmpSpLockBySpCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContLockBySpCmd_t;

/** SpContLockBySp command. */
typedef struct {
    cmpSpContLockBySpCmd_t cmd;
} cmpCmdSpContLockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspSpContLockBySpTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpLockBySpRspSdata_t;

typedef struct {
    cmpSpLockBySpRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContLockBySpRsp_t;

/** SpContLockBySp response. */
typedef struct {
    cmpSpContLockBySpRsp_t rsp;
} cmpRspSpContLockBySp_t;

/** @} */ 

/**
 * Map CMP SpContLockBySp message.
 */
typedef union {
    cmpCmdSpContLockBySp_t cmpCmdSpContLockBySp;
    cmpRspSpContLockBySp_t cmpRspSpContLockBySp;
} cmpMapSpContLockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader; 
    mcSpid_t spid;
} cmpSpUnlockBySpCmdSdata_t;

typedef struct {
    cmpSpUnlockBySpCmdSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnlockBySpCmd_t;

/** SpContUnlockBySp command. */
typedef struct {
    cmpSpContUnlockBySpCmd_t cmd;
} cmpCmdSpContUnlockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspSpContLockBySpTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpSpUnlockBySpRspSdata_t;

typedef struct {
    cmpSpUnlockBySpRspSdata_t sdata;
    cmpMac_t mac;
} cmpSpContUnlockBySpRsp_t;

/** SpContUnlockBySp response. */
typedef struct {
    cmpSpContUnlockBySpRsp_t rsp;
} cmpRspSpContUnlockBySp_t;

/** @} */ 

/**
 * Map CMP SpContUnlockBySp message.
 */
typedef union {
    cmpCmdSpContUnlockBySp_t cmpCmdSpContUnlockBySp;
    cmpRspSpContUnlockBySp_t cmpRspSpContUnlockBySp;
} cmpMapSpContUnlockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kSpTltEnc;
} cmpTltRegActMsgEdata_t;

typedef struct {
    cmpTltRegActMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegActMsgEdata_t))];
} cmpTltRegActCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
    mcSha256_t skSpTltEnc; 
    mcContVersion_t tltVersion;
    uint32_t edLen;
    cmpTltRegActCmdEd_t ed;
} cmpTltRegActCmdSdata_t;

typedef struct {
    cmpTltRegActCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContRegisterActivateCmd_t;

/** TltContRegisterActivate command. */
typedef struct {
    cmpTltContRegisterActivateCmd_t cmd;
} cmpCmdTltContRegisterActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspTltContRegisterActivateTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
    /* TltCont mapping info is in TCI cmpRspTltContRegisterActivateTci */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltRegActRspSdata_t;

typedef struct {
    cmpTltRegActRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContRegisterActivateRsp_t;

/** TltContRegisterActivate response. */
typedef struct {
    cmpTltContRegisterActivateRsp_t rsp;
} cmpRspTltContRegisterActivate_t;

/** @} */ 

/**
 * Map CMP TltContRegisterActivate message.
 */
typedef union {
    cmpCmdTltContRegisterActivate_t cmpCmdTltContRegisterActivate;
    cmpRspTltContRegisterActivate_t cmpRspTltContRegisterActivate;
} cmpMapTltContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
} cmpTltUnregCmdSdata_t;

typedef struct {
    cmpTltUnregCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContUnregisterCmd_t;

/** TltContUnregister command. */
typedef struct {
    cmpTltContUnregisterCmd_t cmd;
} cmpCmdTltContUnregister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspTltContUnregisterTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
} cmpTltUnregRspSdata_t;

typedef struct {
    cmpTltUnregRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContUnregisterRsp_t;

/** TltContUnregister response. */
typedef struct {
    cmpTltContUnregisterRsp_t rsp;
} cmpRspTltContUnregister_t;

/** @} */ 

/**
 * Map CMP TltContUnregister message.
 */
typedef union {
    cmpCmdTltContUnregister_t cmpCmdTltContUnregister;
    cmpRspTltContUnregister_t cmpRspTltContUnregister;
} cmpMapTltContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_CMD Command
 * @{ */

typedef struct {
    mcSymmetricKey_t kSpTltEnc;
} cmpTltRegMsgEdata_t;

typedef struct {
    cmpTltRegMsgEdata_t edata;
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegMsgEdata_t))];
} cmpTltRegCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
    mcSha256_t skSpTltEnc; 
    mcContVersion_t tltVersion;
    uint32_t edLen;
    cmpTltRegCmdEd_t ed;
} cmpTltRegCmdSdata_t;

typedef struct {
    cmpTltRegCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContRegisterCmd_t;

/** TltContRegister command. */
typedef struct {
    cmpTltContRegisterCmd_t cmd;
} cmpCmdTltContRegister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* SpCont mapping info is in TCI cmpRspTltContRegisterTci */
    uint32_t soSpContLen;
    mcSoSpCont_t soSpCont;
    /* TltCont mapping info is in TCI cmpRspTltContRegisterTci_t */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltRegRspSdata_t;

typedef struct {
    cmpTltRegRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContRegisterRsp_t;

/** TltContRegister response. */
typedef struct {
    cmpTltContRegisterRsp_t rsp;
} cmpRspTltContRegister_t;

/** @} */ 

/**
 * Map CMP TltContRegister message.
 */
typedef union {
    cmpCmdTltContRegister_t cmpCmdTltContRegister;
    cmpRspTltContRegister_t cmpRspTltContRegister;
} cmpMapTltContRegister_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
} cmpTltActCmdSdata_t;

typedef struct {
    cmpTltActCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContActivateCmd_t;

/** TltContActivate command. */
typedef struct {
    cmpTltContActivateCmd_t cmd;
} cmpCmdTltContActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* TltCont mapping info is in TCI cmpRspTltContActivateTci */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltActRspSdata_t;

typedef struct {
    cmpTltActRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContActivateRsp_t;

/** TltContActivate response. */
typedef struct {
    cmpTltContActivateRsp_t rsp;
} cmpRspTltContActivate_t;

/** @} */ 

/**
 * Map CMP TltContActivate message.
 */
typedef union {
    cmpCmdTltContActivate_t cmpCmdTltContActivate;
    cmpRspTltContActivate_t cmpRspTltContActivate;
} cmpMapTltContActivate_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
} cmpTltLockBySpCmdSdata_t;

typedef struct {
    cmpTltLockBySpCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContLockBySpCmd_t;

/** TltContLockBySp command. */
typedef struct {
    cmpTltContLockBySpCmd_t cmd;
} cmpCmdTltContLockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* TltCont mapping info is in TCI cmpRspTltContLockBySpTci */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltLockBySpRspSdata_t;

typedef struct {
    cmpTltLockBySpRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContLockBySpRsp_t;

/** TltContLockBySp response. */
typedef struct {
    cmpTltContLockBySpRsp_t rsp;
} cmpRspTltContLockBySp_t;

/** @} */ 

/**
 * Map CMP TltContLockBySp message.
 */
typedef union {
    cmpCmdTltContLockBySp_t cmpCmdTltContLockBySp;
    cmpRspTltContLockBySp_t cmpRspTltContLockBySp;
} cmpMapTltContLockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP 
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
} cmpTltUnlockBySpCmdSdata_t;

typedef struct {
    cmpTltUnlockBySpCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContUnlockBySpCmd_t;

/** TltContUnlockBySp command. */
typedef struct {
    cmpTltContUnlockBySpCmd_t cmd;
} cmpCmdTltContUnlockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* TltCont mapping info is in TCI cmpRspTltContUnlockBySpTci */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltUnlockBySpRspSdata_t;

typedef struct {
    cmpTltUnlockBySpRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContUnlockBySpRsp_t;

/** TltContUnlockBySp response. */
typedef struct {
    cmpTltContUnlockBySpRsp_t rsp;
} cmpRspTltContUnlockBySp_t;

/** @} */

/**
 * Map CMP TltContUnlockBySp message.
 */
typedef union {
    cmpCmdTltContUnlockBySp_t cmpCmdTltContUnlockBySp;
    cmpRspTltContUnlockBySp_t cmpRspTltContUnlockBySp;
} cmpMapTltContUnlockBySp_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_CMD Command
 * @{ */

typedef struct {
    mcCoDataCont_t dataCont;
} cmpTltContPersonalizeCmdEdata_t;

typedef struct {
    cmpTltContPersonalizeCmdEdata_t edata;
    uint8_t padding_[CMP_ED_PADDING(sizeof(cmpTltContPersonalizeCmdEdata_t))];
} cmpTltContPersonalizeCmdEd_t;

typedef struct {
    cmpCommandHeaderMap_t cmdHeader;
    mcSpid_t spid;
    mcUuid_t uuid;
    mcPid_t pid;
    uint32_t edLen;
    cmpTltContPersonalizeCmdEd_t ed;
} cmpTltContPersonalizeCmdSdata_t;

typedef struct {
    cmpTltContPersonalizeCmdSdata_t sdata;
    cmpMac_t mac;
} cmpTltContPersonalizeCmd_t;

/** TltContPersonalize command. */
typedef struct {
    cmpTltContPersonalizeCmd_t cmd;
} cmpCmdTltContPersonalize_t;

/** @} */ 

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_RSP Response
 * @{ */

typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    /* DataCont mapping info is in TCI cmpRspTltContPersonalizeTci */
    uint32_t soDataContLen;
    mcSoDataCont_t soDataCont;
} cmpTltContPersonalizeRspSdata_t;

typedef struct {
    cmpTltContPersonalizeRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContPersonalizeRsp_t;

/** TltContPersonalize response. */
typedef struct {
    cmpTltContPersonalizeRsp_t rsp;
} cmpRspTltContPersonalize_t;

/** @} */

/**
 * Map CMP TltContPersonalize message.
 */
typedef union {
    cmpCmdTltContPersonalize_t cmpCmdTltContPersonalize;
    cmpRspTltContPersonalize_t cmpRspTltContPersonalize;
} cmpMapTltContPersonalize_t;

/** @} */

/**
 * Map CMP Error message.
 */
typedef struct {
    cmpResponseHeaderMap_t rspHeader;
    cmpMac_t mac;
} cmpErrorRsp_t;

/** Cmp error response. */
typedef struct {
    cmpErrorRsp_t rsp;
} cmpMapError_t;

/**
 * Map CMP message.
 */
typedef union {
    // Compatible header with CMP message earlier than version 3.0
    cmpCommandHeader_t commandHeader;
    cmpResponseHeader_t responseHeader;

    // Compatible CMP message earlier than version 3.0
    cmpCmdGetVersion_t cmpCmdGetVersion;
    cmpRspGetVersion_t cmpRspGetVersion;
    cmpCmdGetSuid_t cmpCmdGetSuid;
    cmpRspGetSuid_t cmpRspGetSuid;
    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    cmpRspGenAuthToken_t cmpRspGenAuthToken;

    // CMP message headers
    cmpCommandHeaderMap_t cmdHeader;
    cmpResponseHeaderMap_t rspHeader;

    // CMP error response message
    cmpMapError_t cmpMapError;

    // CMP message for authentication
    cmpCmdBeginSocAuthentication_t cmpCmdBeginSocAuthentication;
    cmpRspBeginSocAuthentication_t cmpRspBeginSocAuthentication;
    cmpCmdBeginRootAuthentication_t cmpCmdBeginRootAuthentication;
    cmpRspBeginRootAuthentication_t cmpRspBeginRootAuthentication;
    cmpCmdBeginSpAuthentication_t cmpCmdBeginSpAuthentication;
    cmpRspBeginSpAuthentication_t cmpRspBeginSpAuthentication;
    cmpCmdAuthenticate_t cmpCmdAuthenticate;
    cmpRspAuthenticate_t cmpRspAuthenticate;
    cmpCmdAuthenticateTerminate_t cmpCmdAuthenticateTerminate;
    cmpRspAuthenticateTerminate_t cmpRspAuthenticateTerminate;

    // CMP message for SoC administration
    cmpCmdRootContRegisterActivate_t cmpCmdRootContRegisterActivate;
    cmpRspRootContRegisterActivate_t cmpRspRootContRegisterActivate;

    // CMP message for Root administration
    cmpCmdRootContUnregister_t cmpCmdRootContUnregister;
    cmpRspRootContUnregister_t cmpRspRootContUnregister;
    cmpCmdRootContLockByRoot_t cmpCmdRootContLockByRoot;
    cmpRspRootContLockByRoot_t cmpRspRootContLockByRoot;
    cmpCmdRootContUnlockByRoot_t cmpCmdRootContUnlockByRoot;
    cmpRspRootContUnlockByRoot_t cmpRspRootContUnlockByRoot;
    cmpCmdSpContRegisterActivate_t cmpCmdSpContRegisterActivate;
    cmpRspSpContRegisterActivate_t cmpRspSpContRegisterActivate;
    cmpCmdSpContUnregister_t cmpCmdSpContUnregister;
    cmpRspSpContUnregister_t cmpRspSpContUnregister;
    cmpCmdSpContRegister_t cmpCmdSpContRegister;
    cmpRspSpContRegister_t cmpRspSpContRegister;
    cmpCmdSpContLockByRoot_t cmpCmdSpContLockByRoot;
    cmpRspSpContLockByRoot_t cmpRspSpContLockByRoot;
    cmpCmdSpContUnlockByRoot_t cmpCmdSpContUnlockByRoot;
    cmpRspSpContUnlockByRoot_t cmpRspSpContUnlockByRoot;

    // CMP message for Sp administration
    cmpCmdSpContActivate_t cmpCmdSpContActivate;
    cmpRspSpContActivate_t cmpRspSpContActivate;
    cmpCmdSpContLockBySp_t cmpCmdSpContLockBySp;
    cmpRspSpContLockBySp_t cmpRspSpContLockBySp;
    cmpCmdSpContUnlockBySp_t cmpCmdSpContUnlockBySp;
    cmpRspSpContUnlockBySp_t cmpRspSpContUnlockBySp;
    cmpCmdTltContRegisterActivate_t cmpCmdTltContRegisterActivate;
    cmpRspTltContRegisterActivate_t cmpRspTltContRegisterActivate;
    cmpCmdTltContUnregister_t cmpCmdTltContUnregister;
    cmpRspTltContUnregister_t cmpRspTltContUnregister;
    cmpCmdTltContRegister_t cmpCmdTltContRegister;
    cmpRspTltContRegister_t cmpRspTltContRegister;
    cmpCmdTltContActivate_t cmpCmdTltContActivate;
    cmpRspTltContActivate_t cmpRspTltContActivate;
    cmpCmdTltContLockBySp_t cmpCmdTltContLockBySp;
    cmpRspTltContLockBySp_t cmpRspTltContLockBySp;
    cmpCmdTltContUnlockBySp_t cmpCmdTltContUnlockBySp;
    cmpRspTltContUnlockBySp_t cmpRspTltContUnlockBySp;
    cmpCmdTltContPersonalize_t cmpCmdTltContPersonalize;
    cmpRspTltContPersonalize_t cmpRspTltContPersonalize;
} cmpMap_t;

#endif // CMP_MAP_H_

/** @} */
