/** @addtogroup CMP
 * @{
 * @file
 * Interface to content management trustlet definitions.
 *
 * The CMTL (Content Management Trustlet) is responsible for implementing
 * CMP commands and generating approriate CMP responses.
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

#ifndef TL_CM_API_H_
#define TL_CM_API_H_

#include "TlCm/tlCmApiCommon.h"
#include "TlCm/3.0/cmp.h"
#include "TlCm/3.0/cmpMap.h"

/**
 * CMP Content Manager message data.
 */
typedef union {
    cmpCommandHeaderTci_t commandHeader;
    cmpResponseHeaderTci_t responseHeader;

    cmpCmdGetVersionTci_t cmpCmdGetVersionTci;
    cmpRspGetVersionTci_t cmpRspGetVersionTci;

    cmpCmdGetSuidTci_t cmpCmdGetSuidTci;
    cmpRspGetSuidTci_t cmpRspGetSuidTci;

    cmpCmdGenAuthTokenTci_t cmpCmdGenAuthTokenTci;
    cmpRspGenAuthTokenTci_t cmpRspGenAuthTokenTci;

    cmpCmdBeginSocAuthenticationTci_t cmpCmdBeginSocAuthenticationTci;
    cmpRspBeginSocAuthenticationTci_t cmpRspBeginSocAuthenticationTci;

    cmpCmdBeginRootAuthenticationTci_t cmpCmdBeginRootAuthenticationTci;
    cmpRspBeginRootAuthenticationTci_t cmpRspBeginRootAuthenticationTci;

    cmpCmdBeginSpAuthenticationTci_t cmpCmdBeginSpAuthenticationTci;
    cmpRspBeginSpAuthenticationTci_t cmpRspBeginSpAuthenticationTci;

    cmpCmdAuthenticateTci_t cmpCmdAuthenticateTci;
    cmpRspAuthenticateTci_t cmpRspAuthenticateTci;

    cmpCmdAuthenticateTerminateTci_t cmpCmdAuthenticateTerminateTci;
    cmpRspAuthenticateTerminateTci_t cmpRspAuthenticateTerminateTci;

    cmpCmdRootContRegisterActivateTci_t cmpCmdRootContRegisterActivateTci;
    cmpRspRootContRegisterActivateTci_t cmpRspRootContRegisterActivateTci;

    cmpCmdRootContUnregisterTci_t cmpCmdRootContUnregisterTci;
    cmpRspRootContUnregisterTci_t cmpRspRootContUnregisterTci;

    cmpCmdRootContLockByRootTci_t cmpCmdRootContLockByRootTci;
    cmpRspRootContLockByRootTci_t cmpRspRootContLockByRootTci;

    cmpCmdRootContUnlockByRootTci_t cmpCmdRootContUnlockByRootTci;
    cmpRspRootContUnlockByRootTci_t cmpRspRootContUnlockByRootTci;

    cmpCmdSpContRegisterActivateTci_t cmpCmdSpContRegisterActivateTci;
    cmpRspSpContRegisterActivateTci_t cmpRspSpContRegisterActivateTci;

    cmpCmdSpContUnregisterTci_t cmpCmdSpContUnregisterTci;
    cmpRspSpContUnregisterTci_t cmpRspSpContUnregisterTci;

    cmpCmdSpContRegisterTci_t cmpCmdSpContRegisterTci;
    cmpRspSpContRegisterTci_t cmpRspSpContRegisterTci;

    cmpCmdSpContLockByRootTci_t cmpCmdSpContLockByRootTci;
    cmpRspSpContLockByRootTci_t cmpRspSpContLockByRootTci;

    cmpCmdSpContUnlockByRootTci_t cmpCmdSpContUnlockByRootTci;
    cmpRspSpContUnlockByRootTci_t cmpRspSpContUnlockByRootTci;

    cmpCmdSpContActivateTci_t cmpCmdSpContActivateTci;
    cmpRspSpContActivateTci_t cmpRspSpContActivateTci;

    cmpCmdSpContLockBySpTci_t cmpCmdSpContLockBySpTci;
    cmpRspSpContLockBySpTci_t cmpRspSpContLockBySpTci;

    cmpCmdSpContUnlockBySpTci_t cmpCmdSpContUnlockBySpTci;
    cmpRspSpContUnlockBySpTci_t cmpRspSpContUnlockBySpTci;

    cmpCmdTltContRegisterActivateTci_t cmpCmdTltContRegisterActivateTci;
    cmpRspTltContRegisterActivateTci_t cmpRspTltContRegisterActivateTci;

    cmpCmdTltContUnregisterTci_t cmpCmdTltContUnregisterTci;
    cmpRspTltContUnregisterTci_t cmpRspTltContUnregisterTci;

    cmpCmdTltContRegisterTci_t cmpCmdTltContRegisterTci;
    cmpRspTltContRegisterTci_t cmpRspTltContRegisterTci;

    cmpCmdTltContActivateTci_t cmpCmdTltContActivateTci;
    cmpRspTltContActivateTci_t cmpRspTltContActivateTci;

    cmpCmdTltContLockBySpTci_t cmpCmdTltContLockBySpTci; 
    cmpRspTltContLockBySpTci_t cmpRspTltContLockBySpTci;

    cmpCmdTltContUnlockBySpTci_t cmpCmdTltContUnlockBySpTci;
    cmpRspTltContUnlockBySpTci_t cmpRspTltContUnlockBySpTci;

    cmpCmdTltContPersonalizeTci_t cmpCmdTltContPersonalizeTci;
    cmpRspTltContPersonalizeTci_t cmpRspTltContPersonalizeTci;
} cmpMessage_t;

/**
 * Overall CMP structure.
 */
typedef struct {
    /** CMP message. */
    cmpMessage_t msg;
} cmp_t;

#endif // TL_CM_API_H_

/** @} */
