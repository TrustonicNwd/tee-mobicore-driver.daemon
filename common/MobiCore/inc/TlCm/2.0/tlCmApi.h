/** @addtogroup CMP
 * @{
 * @file
 * Interface to content management trustlet definitions.
 *
 * The CMTL (Content Management Trustlet) is responsible for implementing
 * CMP 2.0 commands and generating approriate CMP 2.0 responses.
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
#include "TlCm/2.0/cmp.h"

/**
 * CMP Content Manager message data.
 */
typedef union {
    cmpCommandHeader_t commandHeader;
    cmpResponseHeader_t responseHeader;

    cmpCmdGetVersion_t cmpCmdGetVersion;
    cmpRspGetVersion_t cmpRspGetVersion;

    cmpCmdBeginSocAuthentication_t cmpCmdBeginSocAuthentication;
    cmpRspBeginSocAuthentication_t cmpRspBeginSocAuthentication;

    cmpCmdBeginRootAuthentication_t cmpCmdBeginRootAuthentication;
    cmpRspBeginRootAuthentication_t cmpRspBeginRootAuthentication;

    cmpCmdBeginSpAuthentication_t cmpCmdBeginSpAuthentication;
    cmpRspBeginSpAuthentication_t cmpRspBeginSpAuthentication;

    cmpCmdAuthenticate_t cmpCmdAuthenticate;
    cmpRspAuthenticate_t cmpRspAuthenticate;

    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    cmpRspGenAuthToken_t cmpRspGenAuthToken;

    cmpCmdRootContRegisterActivate_t cmpCmdRootContRegisterActivate;
    cmpRspRootContRegisterActivate_t cmpRspRootContRegisterActivate;

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

    cmpCmdSpContLockByRoot_t cmpCmdSpContLockByRoot;
    cmpRspSpContLockByRoot_t cmpRspSpContLockByRoot;

    cmpCmdSpContUnlockByRoot_t cmpCmdSpContUnlockByRoot;
    cmpRspSpContUnlockByRoot_t cmpRspSpContUnlockByRoot;

    cmpCmdSpContLockBySp_t cmpCmdSpContLockBySp;
    cmpRspSpContLockBySp_t cmpRspSpContLockBySp;

    cmpCmdSpContUnlockBySp_t cmpCmdSpContUnlockBySp;
    cmpRspSpContUnlockBySp_t cmpRspSpContUnlockBySp;

    cmpCmdTltContRegister_t cmpCmdTltContRegister;
    cmpRspTltContRegister_t cmpRspTltContRegister;

    cmpCmdTltContActivate_t cmpCmdTltContActivate;
    cmpRspTltContActivate_t cmpRspTltContActivate;

    cmpCmdTltContRegisterActivate_t cmpCmdTltContRegisterActivate;
    cmpRspTltContRegisterActivate_t cmpRspTltContRegisterActivate;

    cmpCmdTltContLockBySp_t cmpCmdTltContLockBySp;
    cmpRspTltContLockBySp_t cmpRspTltContLockBySp;

    cmpCmdTltContUnlockBySp_t cmpCmdTltContUnlockBySp;
    cmpRspTltContUnlockBySp_t cmpRspTltContUnlockBySp;

    cmpCmdTltContUnregister_t cmpCmdTltContUnregister;
    cmpRspTltContUnregister_t cmpRspTltContUnregister;

    cmpCmdGetSuid_t cmpCmdGetSuid;
    cmpRspGetSuid_t cmpRspGetSuid;

    cmpCmdAuthenticateTerminate_t cmpCmdAuthenticateTerminate;
    cmpRspAuthenticateTerminate_t cmpRspAuthenticateTerminate;

    cmpCmdTltContPersonalize_t cmpCmdTltContPersonalize;
    cmpRspTltContPersonalize_t cmpRspTltContPersonalize;

    cmpCmdSpContRegister_t cmpCmdSpContRegister;
    cmpRspSpContRegister_t cmpRspSpContRegister;

    cmpCmdSpContActivate_t cmpCmdSpContActivate;
    cmpRspSpContActivate_t cmpRspSpContActivate;
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
