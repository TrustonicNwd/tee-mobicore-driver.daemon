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
#ifndef TRUSTZONEDEVICE_H_
#define TRUSTZONEDEVICE_H_


#include <stdint.h>

#include "McTypes.h"

#include "CSemaphore.h"
#include "CMcKMod.h"
#include "CWsm.h"

#include "ExcDevice.h"
#include "MobiCoreDevice.h"


#define SCHEDULING_FREQ     5   /**< N-SIQ every n-th time */

class TrustZoneDevice : public MobiCoreDevice
{

protected:
    bool         schedulerEnabled; /**< NQ IRQ Scheduler enabling */
    CSemaphore   schedSync; /**< Semaphore to synchronize S-SIQs with scheduler thread */
    CMcKMod_ptr  pMcKMod; /**< kernel module */
    CWsm_ptr     pWsmMcp; /**< WSM use for MCP */
    CWsm_ptr     mobicoreInDDR;  /**< WSM used for Mobicore binary */

    /** Access functions to the MC Linux kernel module
     */
    bool yield(void);

    bool nsiq(void);

    bool waitSsiq(void);

public:

    TrustZoneDevice(void);

    virtual ~TrustZoneDevice(void);

//  static MobiCoreDevice* getDeviceInstance(
//      void
//  );
    /** Set up MCI and wait till MC is initialized
     *
     * @param devFile the device node to speak to.
     * @param loadMobiCore
     * @param mobicoreImage
     * @param enableScheduler
     *
     * @return true if mobicore is initialized
     * @trows ExcDevice
     */
    bool initDevice(
        const char  *devFile,
        bool        enableScheduler
    );

    void initDeviceStep2(void);

    void notify(uint32_t sessionId);

    void dumpMobicoreStatus(void);

    uint32_t getMobicoreStatus(void);

    bool checkMciVersion(void);

    /** Memory allocation functions */
    bool getMciInstance(uint32_t len, CWsm_ptr *mci, bool *reused);

    //bool freeWsm(CWsm_ptr pWsm);

    CWsm_ptr registerWsmL2(addr_t buffer, uint32_t len, uint32_t pid);

    bool unregisterWsmL2(CWsm_ptr pWsm);

    bool lockWsmL2(uint32_t handle);

    bool unlockWsmL2(uint32_t handle);

    addr_t findWsmL2(uint32_t handle, int fd);

    bool findContiguousWsm(uint32_t handle, int fd, addr_t *phys, uint32_t *len);

    /**
     * Cleanup all orphaned bulk buffers.
     */
    bool cleanupWsmL2(void);

    /**
     * Allocates persistent WSM memory for TL (won't be fried when TLC exits).
     */
    CWsm_ptr allocateContiguousPersistentWsm(uint32_t len);

    bool setupLog(void);

    bool schedulerAvailable(void);

    void schedule(void);

    void handleIrq(void);
};

#endif /* TRUSTZONEDEVICE_H_ */

/** @} */
