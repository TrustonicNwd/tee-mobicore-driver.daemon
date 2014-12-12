/*
 * Copyright (c) 2013-2014 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "tlcCmApi.h"
#include "TlCm/tlCmUuid.h"

#include "log.h"

#include <assert.h>
#include <stdlib.h>

static mcSessionHandle_t m_sessionHandle = { 0, 0 };

mcResult_t cmOpen(cmp_t **cmp)
{
    const mcUuid_t UUID = TL_CM_UUID;
    mcResult_t result;

    if (NULL == cmp) {
        LOG_E("Parameter cmp is NULL");
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    result = mcOpenDevice(MC_DEVICE_ID_DEFAULT);
    if (MC_DRV_OK != result) {
        LOG_E("Error opening device: %d", result);
        return result;
    }
    result = mcMallocWsm(MC_DEVICE_ID_DEFAULT, 0, sizeof(cmp_t), (uint8_t **)cmp, 0);
    if (MC_DRV_OK != result) {
        LOG_E("Allocation of CMP WSM failed: %d", result);
        return result;
    }

    result = mcOpenSession(&m_sessionHandle, &UUID, (uint8_t *) * cmp, (uint32_t) sizeof(cmp_t));
    if (MC_DRV_OK != result) {
        LOG_E("Open session failed: %d", result);
    }

    return result;
}

mcResult_t cmClose(void)
{
    mcResult_t result;

    if (0 == m_sessionHandle.sessionId) {
        LOG_E("No session to close!");
    }
    result = mcCloseSession(&m_sessionHandle);
    if (MC_DRV_OK != result) {
        LOG_E("Closing session failed: %d", result);
    }
    result = mcCloseDevice(MC_DEVICE_ID_DEFAULT);
    if (MC_DRV_OK != result) {
        LOG_E("Closing MobiCore device failed: %d", result);
    }

    return result;
}

mcResult_t cmManage(void)
{
    mcResult_t result;

    if (0 == m_sessionHandle.sessionId) {
        LOG_E("No session.");
        return MC_DRV_ERR_INIT;
    }

    // Send CMP message to content management trustlet.
    result = mcNotify(&m_sessionHandle);

    if (MC_DRV_OK != result) {
        LOG_E("Notify failed: %d", result);
        return result;
    }

    // Wait for trustlet response.
    result = mcWaitNotification(&m_sessionHandle, MC_INFINITE_TIMEOUT);

    if (MC_DRV_OK != result) {
        LOG_E("Wait for response notification failed: %d", result);
        return result;
    }

    return MC_DRV_OK;
}

/** @} */
