/*
Copyright  © Trustonic Limited 2013

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this 
     list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.

  3. Neither the name of the Trustonic Limited nor the names of its contributors 
     may be used to endorse or promote products derived from this software 
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <MobiCoreDriverApi.h>
#include <TlCm/3.0/tlCmApi.h>
#include <TlCm/3.0/cmpMap.h>
#include <TlCm/tlCmUuid.h>

#include "logging.h"

#define LOG_TAG "RootPA-C"


/*
    All the functions called from outside of this file are intruduced in the public header files of MobiCore
*/


// forward declarations and global variables  =======================================================

void handleMessage();

uint32_t tciLength_=0;
uint8_t* storedWsm_=NULL;
uint8_t* mappedMemory_=NULL;
uint32_t mappedMemoryLen_=0;
mcSessionHandle_t sessionHandle_;
bool notified_=false;
bool sessionOpen_=false;
bool deviceOpen_=false;

/*
These variables are used for ensuring that the session handling and authentication works
*/

bool socAuthenticationPending_=false;
bool rootAuthenticationPending_=false;
bool spAuthenticationPending_=false;

bool socAuthenticated_=false;
bool rootAuthenticated_=false;
bool spAuthenticated_=false;


// stubbed methods, mobicore driver  ================================================================

__MC_CLIENT_LIB_API mcResult_t mcMallocWsm(
    uint32_t  deviceId,
    uint32_t  align,
    uint32_t  len,
    uint8_t   **wsm,
    uint32_t  wsmFlags
)
{
    if(deviceId!=MC_DEVICE_ID_DEFAULT)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    if(align!=0 || 0==len || wsmFlags!=0 || len > MC_MAX_TCI_LEN ) // implement align and wsmFlags only if needed
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    *wsm=malloc(len);
    
    if (NULL==*wsm)
    {
        return MC_DRV_ERR_NO_FREE_MEMORY;
    }

    tciLength_=len;
    storedWsm_=*wsm;

    return MC_DRV_OK;    
}


/** Open a new connection to a MobiCore device.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_INVALID_DEVICE_FILE if kernel module under /dev/mobicore cannot be opened
 */
__MC_CLIENT_LIB_API mcResult_t mcOpenDevice(
    uint32_t deviceId
)
{

    if(deviceId!=MC_DEVICE_ID_DEFAULT)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }
    else if(true==deviceOpen_)
    {
        return MC_DRV_ERR_INVALID_OPERATION;
    }

    deviceOpen_=true;
    return MC_DRV_OK;
}


/**
*/
__MC_CLIENT_LIB_API mcResult_t mcFreeWsm(
    uint32_t  deviceId,
    uint8_t   *wsm
)
{
    if(deviceId!=MC_DEVICE_ID_DEFAULT)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    if(wsm != storedWsm_)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    free(wsm);    
    storedWsm_=NULL;
    return MC_DRV_OK;    

}

/** Close the connection to a MobiCore device.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 */
__MC_CLIENT_LIB_API mcResult_t mcCloseDevice(
    uint32_t deviceId
)
{
    if(deviceId!=MC_DEVICE_ID_DEFAULT)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }
    else if (true==sessionOpen_)
    {
        return MC_DRV_ERR_SESSION_PENDING;
    }

    deviceOpen_=false;
    sessionOpen_=false;

    socAuthenticationPending_=false;
    rootAuthenticationPending_=false;
    spAuthenticationPending_=false;
    socAuthenticated_=false;
    rootAuthenticated_=false;
    spAuthenticated_=false;

    return MC_DRV_OK;

}


/** Open a new session to a Trustlet. The trustlet with the given UUID has to be available in the flash filesystem.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon socket occur.
 */
__MC_CLIENT_LIB_API mcResult_t mcOpenSession(
    mcSessionHandle_t  *session,
    const mcUuid_t     *uuid,
    uint8_t            *tci,
    uint32_t           tciLen
)
{
    mcUuid_t cmUuid=TL_CM_UUID;
    if( NULL == session || tciLen!=tciLength_ || 0 == tciLen ||  tciLen > MC_MAX_TCI_LEN ||
        tci != storedWsm_ || NULL == tci || NULL == uuid || 
        memcmp((void*)&cmUuid, (void*) uuid, sizeof(mcUuid_t))!=0 )
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    if(session->deviceId!=MC_DEVICE_ID_DEFAULT || false == deviceOpen_ )
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }
    
    session->sessionId=rand();
    sessionHandle_.sessionId=session->sessionId;
    sessionHandle_.deviceId=session->deviceId;
    sessionOpen_=true;
    socAuthenticationPending_=false;
    rootAuthenticationPending_=false;
    spAuthenticationPending_=false;
    socAuthenticated_=false;
    rootAuthenticated_=false;
    spAuthenticated_=false;

    return MC_DRV_OK;
}


/** Close a Trustlet session.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_INVALID_DEVICE_FILE when daemon cannot open trustlet file.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcCloseSession(
    mcSessionHandle_t *session
)
{
    if(NULL == session)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }
    
    if (session->sessionId != sessionHandle_.sessionId || false == sessionOpen_)
    {
        return MC_DRV_ERR_UNKNOWN_SESSION;
    }
    
    if (session->deviceId != sessionHandle_.deviceId || false == deviceOpen_)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    session->sessionId=0;
    sessionHandle_.sessionId=session->sessionId;
    sessionHandle_.deviceId=0;
    sessionOpen_=false;

    socAuthenticationPending_=false;
    rootAuthenticationPending_=false;
    spAuthenticationPending_=false;
    socAuthenticated_=false;
    rootAuthenticated_=false;
    spAuthenticated_=false;

    return MC_DRV_OK;
}


/** Notify a session.
 */
__MC_CLIENT_LIB_API mcResult_t mcNotify(
    mcSessionHandle_t *session
)
{
    notified_=false;
    if(NULL == session || false==sessionOpen_)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    if (session->sessionId != sessionHandle_.sessionId)
    {
        return MC_DRV_ERR_UNKNOWN_SESSION;
    }
    
    if (session->deviceId != sessionHandle_.deviceId)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    handleMessage();

    notified_=true;
    return MC_DRV_OK;
}


/** Wait for a notification.
 * @return MC_DRV_INFO_NOTIFICATION if a problem with the session was encountered. Get more details with mcGetSessionErrorCode().
 */
__MC_CLIENT_LIB_API mcResult_t mcWaitNotification(
    mcSessionHandle_t  *session,
    int32_t            timeout
)
{

    if(NULL == session || false==sessionOpen_)
    {
        notified_=false;
        return MC_DRV_ERR_INVALID_PARAMETER;
    }
    
    if (session->sessionId != sessionHandle_.sessionId)
    {
        notified_=false;
        return MC_DRV_ERR_UNKNOWN_SESSION;
    }
    
    if (session->deviceId != sessionHandle_.deviceId)
    {
        notified_=false;
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    if(notified_)
    {
        notified_=false;
        return MC_DRV_OK;
    }

    if(timeout != MC_DRV_ERR_TIMEOUT)
    {
        notified_=false;
        return MC_DRV_ERR_TIMEOUT;
    }

    return MC_DRV_ERR_NOTIFICATION;
}


__MC_CLIENT_LIB_API mcResult_t mcMap(
    mcSessionHandle_t  *session,
    void               *buf,
    uint32_t           len,
    mcBulkMap_t        *mapInfo
)
{
    if(NULL == session || false==sessionOpen_)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }
    
    if (session->sessionId != sessionHandle_.sessionId)
    {
        return MC_DRV_ERR_UNKNOWN_SESSION;
    }
    
    if (session->deviceId != sessionHandle_.deviceId)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }

    // it is assumed that RootPA calls this only once for one buffer, 
    // if that changes, this needs to be also changed
    if (mappedMemory_!=NULL || mappedMemoryLen_!=0)
    {
        return MC_DRV_ERR_BULK_MAPPING;
    }

    mapInfo->sVirtualAddr=buf;
    mapInfo->sVirtualLen=len;

    mappedMemory_=buf;
    mappedMemoryLen_=len;

    return MC_DRV_OK;
}


__MC_CLIENT_LIB_API mcResult_t mcUnmap(
    mcSessionHandle_t  *session,
    void               *buf,
    mcBulkMap_t        *mapInfo
)
{
    if(NULL == session || false==sessionOpen_)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }
    
    if (session->sessionId != sessionHandle_.sessionId)
    {
        return MC_DRV_ERR_UNKNOWN_SESSION;
    }
    
    if (session->deviceId != sessionHandle_.deviceId)
    {
        return MC_DRV_ERR_UNKNOWN_DEVICE;
    }
    
    if (buf != mappedMemory_)
    {
        return MC_DRV_ERR_BULK_MAPPING;
    }

    if (mapInfo->sVirtualAddr != mappedMemory_ || mapInfo->sVirtualLen != mappedMemoryLen_)
    {
        return MC_DRV_ERR_BULK_MAPPING;
    }

  
    mappedMemory_=NULL;
    mappedMemoryLen_=0;
    
    return MC_DRV_OK;
}


// test data used by registry and cmp commands  =======================================================================
// {1,1,1,1} == 16843009
// {2,2,2,2} == 33686018
// {3,3,3,3} == 50529027

#define ROOTID 7
#define SPID 8
#define SPID_UNKNOWN 12
#define SPID_UNKNOWN_TLT_CONT_INDICES 16
static const mcUuid_t TLTUUID={{3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6}};
static const mcSoAuthTokenCont_t AUTHTOKENCONT={{16843009,16843009,(mcSoContext_t)16843009,(mcSoLifeTime_t)16843009,{16843009,
                                      {{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}}},33686018,
                                      50529027},{(contType_t)50529027,50529027,{(mcContainerState_t)50529027},{50529027,
                                      {{4,4,4,4,4,4,4,4,4,4,4,4}}},
                                      {{{0x08080808,0x08080808,0x08080808,0x08080808,0x08080808,       //  mcSymmetricKey (8 ints)
                                      0x09090909,0x09090909,0x09090909}}}},{9,9,9,9,9,9,9,9,
                                      0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA, 
                                      0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB}};


static const mcSoRootCont_t ROOTCONT={{16843009,16843009,(mcSoContext_t)16843009,(mcSoLifeTime_t)16843009,{16843009,
                                      {{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}}},33686018,
                                      50529027},{(contType_t)50529027,50529027,{(mcContainerState_t)50529027},{50529027,
                                      {{4,4,4,4,4,4,4,4,4,4,4,4}}},0x04040404,{0x04040404,                 // 1 child
                                      0x05050505,0x05050505,0x05050505,0x05050505,0x05050505,            // 5 children
                                      0x06060606,0x06060606,0x06060606,0x06060606,0x06060606,            // 5 childrenAUT
                                      0x07070707,0x07070707,0x07070707,0x07070707,0x07070707},          // 5 children
                                      {{{0x08080808,0x08080808,0x08080808,0x08080808,0x08080808,       //  mcSymmetricKey (8 ints)
                                      0x09090909,0x09090909,0x09090909}}}},{9,9,9,9,9,9,9,9,
                                      0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA, // 200
                                      0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB}};


static const mcSoSpCont_t SPCONT={{16843009,16843009,(mcSoContext_t)16843009,(mcSoLifeTime_t)16843009,{16843009,                    // [header               
                                  {{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}}},33686018,                     // header                
                                  50529027},{(contType_t)50529027,50529027,{(mcContainerState_t)4},50529027,                  // header] mcSpCont // state == 4 (SP_LOCKED)
                                 {{{3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6}},{{3,3,3,3,                        // children 16/child 1
                                  4,4,4,4,5,5,5,5,6,6,6,6}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 2
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 3
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                        // 5
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,                        // 6
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 7
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 8
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                  // 10 childred so far
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,                  // 11
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 12 
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 13
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                  // 15 
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}},{{{0x10101010,      // mcSymmetricKey (8 ints)
                                  0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,                                              // 340
                                  0x12121212,0x12121212}}}},{0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,                // 360
                                  0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13, // 380
                                  0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14}                     // 396
                                  };


static const mcSoSpCont_t SPCONT_UNKNOWN_TLT_CONT_INDICES={{16843009,16843009,(mcSoContext_t)16843009,(mcSoLifeTime_t)16843009,{16843009,                    // [header               
                                  {{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}}},33686018,                     // header                
                                  50529027},{(contType_t)50529027,50529027,{(mcContainerState_t)4},50529027,                  // header] mcSpCont // state == 4 (SP_LOCKED)
                                 {{{2,2,2,2,4,4,4,4,5,5,5,5,6,6,6,6}},{{2,2,2,2,                        // children 16/child 1
                                  4,4,4,4,5,5,5,5,6,6,6,6}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 2
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 3
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                        // 5
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,                        // 6
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                         // 7
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 8
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                  // 10 childred so far
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,                  // 11
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 12 
                                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,                   // 13
                                  0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},                  // 15 
                                  {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}},{{{0x10101010,      // mcSymmetricKey (8 ints)
                                  0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,                                              // 340
                                  0x12121212,0x12121212}}}},{0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,                // 360
                                  0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13, // 380
                                  0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14}                     // 396
                                  };
                                  
                                  
static const mcSoTltCont_2_0_t TLTCONT={{16843009,16843009,(mcSoContext_t)16843009,(mcSoLifeTime_t)16843009,{16843009,                   // [header               
                                  {{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}}},33686018,                        // header                
                                  50529027},{{(contType_t)0x03030303,0x03030303,{(mcContainerState_t)4},0x03030303,             // header]60  state=
                                  {{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}},{{{0x04040404,                      // [uuid][mcSymmetric ...
                                  0x05050505,0x05050505,0x05050505,0x05050505,0x05050505,
                                  0x06060606,0x06060606}}}}},{6,6,6,6,6,6,6,6,6,6,6,6,                    // 120
                                  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  
                                  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}};                                   // 156
                                  
static const mcSoDataCont_t DATACONT={{0x11111111,0x11111111,(mcSoContext_t)0x11111111,(mcSoLifeTime_t)0x11111111,{0x11111111,
                                      {{0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21}}},0x21212121,
                                      0x31313131},{(contType_t)0x31313131,0x31313131,{{0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
                                      0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41}},{0x41414141},{{0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                      0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,
                                      0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,
                                      0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71}}}};
                                      

static const cmpMac_t MAC={{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}};

static const int CHANGING_AUTH_TOKEN_CONTAINER = 0xFE;
    
static const suidData_t ANOTHER_SUID = {{
    0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0x01, 0x01, 0x01, 0x01
}};

static const suidData_t OFFSET_OUT_SUID={{
    0x04, 0x05, 0x06, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0x01, 0x01, 0x01, 0x01 }};

static const suidData_t OFFSET_PLUS_LEN_OUT_SUID={{
    0x04, 0x05, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0x01, 0x01, 0x01, 0x01 }};

static const suidData_t RESPONSE_ID_WRONG_SUID={{
    0x04, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0x01, 0x01, 0x01, 0x01 }};
   
static const suidData_t SOFTWARE_SUID = {{
    0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
}};


bool rootWritten_;
cmpCommandId_t lastCommand_;
uint32_t storedSpid_;
mcUuid_t storedTltUuid_;

/*
These variables are used for ensuring that all registry calls are made
*/
bool callRequiresRootWrite_=false;
bool callRequiresSpWrite_=false;
bool callRequiresTltWrite_=false;
bool callRequiresAuthTokenWrite_=false;
bool callRequiresDataWrite_=false;
bool callRequiresRootCleanup_=false;
bool callRequiresAuthTokenDelete_=false;
bool callRequiresSpCleanup_=false;
bool callRequiresTltCleanup_=false;

// stubbed methods, registry  =======================================================================

/*
NOTE that this works in co-operation with handleBeginRootAuthentication and handleBeginSpAuthentication, 
it writes values later used by those
*/

uint32_t smaller(uint32_t first, uint32_t second)
{
    return (first>second?second:first);
}

mcResult_t mcRegistryReadRoot(mcSoRootCont_t* container, uint32_t* size)
{
    LOGI("mcStub:mcRegistryReadRoot");
    if(NULL==container)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    *size=sizeof(ROOTCONT);
    memcpy(container, &ROOTCONT, *size);
    return MC_DRV_OK;
}

mcResult_t mcRegistryStoreRoot(const mcSoRootCont_t *so, uint32_t  size)
{
    LOGI("mcStub:mcRegistryStoreRoot");
    callRequiresRootWrite_=false;
    rootWritten_=true;
    if(NULL==so)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }

    if(memcmp(so,&ROOTCONT, smaller(size, sizeof(ROOTCONT)))==0)
    {
        return MC_DRV_OK;
    }
    
    LOGE("FAILURE: mcStub:mcRegistryStoreRoot not correctly formatted root cont 0x%x ... 0x%d", ((uint8_t*)so)[0],((uint8_t*)so)[200]);
    return MC_DRV_ERR_INVALID_PARAMETER;
}

mcResult_t mcRegistryCleanupRoot(void)
{
    LOGI("mcStub:mcRegistryCleanupRoot");
    callRequiresRootCleanup_=false;
    return MC_DRV_OK;
}

mcResult_t mcRegistryReadAuthToken(mcSoAuthTokenCont_t *so, uint32_t* size)
{
    LOGI("mcStub:mcRegistryReadAuthToken");
    if(NULL==so)
    {
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    *size=sizeof(AUTHTOKENCONT);
    memcpy(so, &AUTHTOKENCONT, *size);
    return MC_DRV_OK;
}

mcResult_t mcRegistryStoreAuthToken(const mcSoAuthTokenCont_t *so, uint32_t size)
{
    LOGI("mcStub:mcRegistryStoreAuthToken");
    callRequiresAuthTokenWrite_=false;

    if(NULL==so)
    {
        LOGE("FAILURE: mcStub:mcRegistryStoreAuthToken no authToken");
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }

    if(memcmp(so,&AUTHTOKENCONT, smaller(size, sizeof(AUTHTOKENCONT)))==0)
    {
        return MC_DRV_OK;
    }   

    if(so->soHeader.type != CHANGING_AUTH_TOKEN_CONTAINER) // checking special case for error case testing, that is not failure
    {
        LOGE("FAILURE: ");
    }
    LOGE("mcStub:mcRegistryStoreAuthToken invalid authToken (size %d)", size);    
    return MC_DRV_ERR_INVALID_PARAMETER;
}

mcResult_t mcRegistryDeleteAuthToken(void)
{
    LOGI("mcStub:mcRegistryDeleteAuthToken");
    callRequiresAuthTokenDelete_=false;
    if(rootWritten_ && MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE==lastCommand_)
    {
        rootWritten_=false;
        return MC_DRV_OK;
    }
    else
    {
        rootWritten_=false;
        return MC_DRV_ERR_UNKNOWN;
    }
}

/*
NOTE that this works in co-operation with handleBeginSpAuthentication, 
it writes values later used by that
*/
mcResult_t mcRegistryReadSp(mcSpid_t spid, mcSoSpCont_t* container, uint32_t* size)
{
    LOGI("mcStub:mcRegistryReadSp");

    if(spid != SPID && spid != SPID_UNKNOWN_TLT_CONT_INDICES )
    {
        LOGE("mcStub:mcRegistryReadSp invalid parameter spid==%d expecting %d",spid,SPID); // in some tests this is expected
        return MC_DRV_ERR_INVALID_DEVICE_FILE;
    }

    if(NULL==container)
    {
        LOGE("FAILURE: mcStub:mcRegistryReadSp invalid parameter container==%ld", (long int) container);
        return MC_DRV_ERR_INVALID_PARAMETER;
    }
    
    *size=sizeof(SPCONT);
    if(SPID_UNKNOWN_TLT_CONT_INDICES==spid)
    {
        memcpy(container, &SPCONT_UNKNOWN_TLT_CONT_INDICES, *size);     
    }
    else
    {
        memcpy(container, &SPCONT, *size); 
    }
    return MC_DRV_OK;
}

mcResult_t mcRegistryStoreSp(mcSpid_t spid, const mcSoSpCont_t *so, uint32_t size)
{
    LOGI("mcStub:mcRegistryStoreSp");
    callRequiresSpWrite_=false;

    if(NULL==so || storedSpid_ != spid || spid != SPID)
    {
        LOGE("FAILURE: mcStub:mcRegistryStoreSp invalid parameter %ld %d %d", (long int) so, spid, storedSpid_);
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }

    if(memcmp(so,&SPCONT, smaller(size, sizeof(SPCONT)))==0)
    {
        return MC_DRV_OK;
    }

    storedSpid_=0;
    LOGE("FAILURE: mcStub:mcRegistryStoreSp not correctly formatted sp cont %d ... %d", ((uint8_t*)so)[0],((uint8_t*)so)[200]);
    return MC_DRV_ERR_INVALID_PARAMETER;
}

mcResult_t mcRegistryCleanupSp(mcSpid_t spid)
{
    LOGI("mcStub:mcRegistryCleanupSp");
    callRequiresSpCleanup_=false;
    if(spid != SPID)
    {
        LOGE("FAILURE: mcStub:mcRegistryCleanupSp invalid parameter %d",spid);
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    return MC_DRV_OK;
}

mcResult_t mcRegistryReadTrustletCon(const mcUuid_t* uuid, mcSpid_t spid, mcSoTltCont_t* so, uint32_t* size)
{
    LOGI("mcStub:mcRegistryReadTrustletCon");

    if(NULL==so || NULL==uuid )
    {
        LOGE("FAILURE: mcStub:mcRegistryReadTrustletCon invalid parameter %ld %ld", (long int) so, (long int)uuid);
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    
    if(memcmp(&TLTUUID, uuid, sizeof(mcUuid_t))!=0)
    {
        LOGE("mcStub:mcRegistryReadSp invalid parameter uuid[0]==%d",(uint32_t)(uuid->value[0]));
        return MC_DRV_ERR_INVALID_DEVICE_FILE;
    }
    
    *size=sizeof(TLTCONT);
    memcpy(so, &TLTCONT, *size); 
    return MC_DRV_OK;
}

mcResult_t mcRegistryStoreTrustletCon(const mcUuid_t *uuid, mcSpid_t spid, const mcSoTltCont_t *so, uint32_t size)
{
    callRequiresTltWrite_=false;
    LOGI("mcStub:mcRegistryStoreTrustletCon");

    if(NULL==so || NULL==uuid || memcmp(&storedTltUuid_, uuid, sizeof(mcUuid_t))!=0 || memcmp(&TLTUUID, uuid, sizeof(mcUuid_t))!=0)
    {
        LOGE("FAILURE: mcStub:mcRegistryStoreTrustletCon invalid parameter %ld %ld %d", (long int) so, ((uuid==NULL)?(long int)uuid:(uint32_t)(uuid->value[0])), storedTltUuid_.value[0]);
        memset(&storedTltUuid_,0,sizeof(mcUuid_t));
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    memset(&storedTltUuid_,0,sizeof(mcUuid_t));
    if(memcmp(so, &TLTCONT, smaller(size,sizeof(TLTCONT)))==0)
    {
        return MC_DRV_OK;
    }

    LOGE("FAILURE: mcStub:mcRegistryStoreTrustletCon not correctly formatted tlt cont %d ... %d", ((uint8_t*)so)[0],((uint8_t*)so)[155]);
    return MC_DRV_ERR_INVALID_PARAMETER;
}

mcResult_t mcRegistryCleanupTrustlet(const mcUuid_t* uuid, mcSpid_t spid)
{
    LOGI("mcStub:mcRegistryCleanupTrustlet");
    callRequiresTltCleanup_=false;

    if(NULL==uuid || memcmp(&storedTltUuid_, uuid, sizeof(mcUuid_t))!=0 || memcmp(&TLTUUID, uuid, sizeof(mcUuid_t))!=0 )
    {
        LOGE("FAILURE: mcStub:mcRegistryCleanupTrustlet invalid parameter %ld %ld %d", (long int) uuid, ((uuid==NULL)?(long int)uuid:(uuid->value[0])), storedTltUuid_.value[0]);
        memset(&storedTltUuid_,0,sizeof(mcUuid_t));
        return MC_DRV_ERR_INVALID_PARAMETER;    
    }
    memset(&storedTltUuid_,0,sizeof(mcUuid_t));
    return MC_DRV_OK;
}


// internal functions for cmp message handling/checking ================================================================

// TODO-WHEN-AVAILABLE use CMP version from CMP header files

#define NEW_CMP_VERSION 0x00030000

/**
sets the needed bytes to different from 0 in case there are problems. This way it is possible to return immediately
after calling this.
*/
uint8_t* getElement(int elementNbr, uint8_t* wsmP, uint32_t* elementLength)
{
    LOGI(">>mcStub: getElement");

    if(0==elementNbr) return NULL;
    elementNbr--;
    cmpMapOffsetInfo_t* elementP=(&((cmpCommandHeaderTci_t*)wsmP)->cmpCmdMapOffsetInfo);
    elementP+=elementNbr;

    *elementLength=elementP->len;

    if(0==*elementLength)
    {
        LOGI("mcStub: getElement element length == 0");
        return NULL;
    }        
    
    if(elementP->offset + *elementLength > mappedMemoryLen_ )
    {
        LOGE("mcStub: getElement %d + %d > %d ",elementP->offset,  *elementLength, mappedMemoryLen_ );
        ((cmpResponseHeaderTci_t*)wsmP)->len=elementP->offset + elementP->len;
        *elementLength=0;
        return NULL;
    }
    LOGI("<<mcStub: getElement %d, %d \n",  elementP->offset, *elementLength);
    return (mappedMemory_+elementP->offset);
}

/**
sets the needed bytes to different from 0 in case there are problems. This way it is possible to return immediately
after calling this.
*/
uint8_t* setElement(uint32_t elementNbr, uint8_t* wsmP, uint32_t elementLength)
{
    LOGI(">>mcStub: setElement %d, %d/%d \n",  elementNbr, elementLength, mappedMemoryLen_);
    if(0==elementNbr) return NULL;
    elementNbr--;
    
    cmpMapOffsetInfo_t* elementP=(cmpMapOffsetInfo_t*)(wsmP+sizeof(cmpResponseHeaderTci_t));
    elementP+=elementNbr;
    
    elementP->len=elementLength;

    if(elementNbr>0)
    {
        cmpMapOffsetInfo_t* prevElementP=elementP-1;
        elementP->offset=(prevElementP->offset)+(prevElementP->len);
    }

    if( elementP->offset + elementP->len > mappedMemoryLen_ )
    {
        LOGE("mcStub: setElement %d + %d > %d ",elementP->offset, elementP->len, mappedMemoryLen_ );

        ((cmpResponseHeaderTci_t*)wsmP)->len=elementP->offset + elementP->len;
        return NULL;
    }

    ((cmpResponseHeaderTci_t*)wsmP)->len=0;

    LOGI("<<mcStub: setElement %d, %d \n",  elementP->offset, elementP->len);    
    return (mappedMemory_+elementP->offset);
}

cmpMapOffsetInfo_t* accessElement(uint32_t elementNbr, uint8_t* wsmP)
{
    if(0==elementNbr) return NULL;
    elementNbr--;    
    cmpMapOffsetInfo_t* elementP=(cmpMapOffsetInfo_t*)(wsmP+sizeof(cmpResponseHeaderTci_t));
    elementP+=elementNbr;
    return elementP;
}

uint32_t getOffset(uint32_t elementNbr, uint8_t* wsmP)
{
    return accessElement(elementNbr, wsmP)->offset;
}

uint32_t getLen(uint32_t elementNbr, uint8_t* wsmP)
{
    return accessElement(elementNbr, wsmP)->len;
}


void modifyOffset(uint32_t elementNbr, uint8_t* wsmP, uint32_t offset, uint32_t len)
{
    LOGI("mcStub: modifyOffset %d %d %d", elementNbr, offset, len);    
    accessElement(elementNbr, wsmP)->offset=offset;
    accessElement(elementNbr, wsmP)->len=len;    
}


void handleGetSuid(uint8_t* cmpP, uint8_t* cmpRspP)
{
    memcpy(((cmpRspGetSuid_t*)(cmpRspP))->suid.suidData.data, &SOFTWARE_SUID, sizeof(SOFTWARE_SUID));
    ((cmpRspGetSuid_t*)(cmpRspP))->suid.sipId=0;
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

void handleGetVersion(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(cmpRspP!=NULL)    
    {
        ((cmpRspGetVersion_t*)(cmpRspP))->tag=CMP_VERSION_TAG2;
        strcpy(((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.productId, "xxxxx");       
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionMci=1;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionSo=2;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionMclf=3;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionContainer=4;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionMcConfig=5;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionTlApi=6;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionDrApi=7;
        ((cmpRspGetVersion_t*)(cmpRspP))->data.versionData2.versionInfo.versionCmp=8;

        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
    }
} 

void handleGenerateAuthToken(uint8_t* cmpP, uint8_t* cmpRspP)
{
    suidData_t cmdSuid;
    memcpy(&cmdSuid,&((cmpCmdGenAuthToken_t*)cmpP)->cmd.sdata.suid.suidData,sizeof(suidData_t));
    memcpy(&((cmpRspGenAuthToken_t*)(cmpRspP))->soAuthCont, &AUTHTOKENCONT ,sizeof(AUTHTOKENCONT));
    uint8_t* containerTargetP=NULL;
    containerTargetP=setElement(2, storedWsm_, sizeof(mcSoAuthTokenCont_t));        
    memcpy(containerTargetP, &AUTHTOKENCONT ,sizeof(AUTHTOKENCONT));

    /**
    the following lines are for testing various error cases with RootPAClient
    */    
    if(memcmp(&ANOTHER_SUID,&cmdSuid,sizeof(suidData_t))==0)
    {
        ((mcSoAuthTokenCont_t*)containerTargetP)->soHeader.type=CHANGING_AUTH_TOKEN_CONTAINER;
    }
    else if(memcmp(&OFFSET_OUT_SUID,&cmdSuid,sizeof(suidData_t))==0)
    {
        // doing this with the container
        uint32_t offset=getOffset(2,storedWsm_);
        uint32_t len=getLen(2,storedWsm_);
        offset=mappedMemoryLen_+3;
        modifyOffset(2, storedWsm_, offset, len);
    }
    else if(memcmp(&OFFSET_PLUS_LEN_OUT_SUID,&cmdSuid,sizeof(suidData_t))==0)
    {
        // doing this with the actual message        
        uint32_t offset=getOffset(1,storedWsm_);
        uint32_t len=getLen(1,storedWsm_);
        len=mappedMemoryLen_+3;
        modifyOffset(1, storedWsm_, offset, len);
    }
    else if(memcmp(&RESPONSE_ID_WRONG_SUID,&cmdSuid,sizeof(suidData_t))==0)
    {
        *((uint32_t*)(cmpRspP))=RSP_ID(MC_CMP_CMD_BEGIN_SP_AUTHENTICATION); // could be anything but GEN_AUTH_TOKEN, we are using real value
    }    
    
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

void handleAuthenticate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(socAuthenticationPending_)
    {
        socAuthenticationPending_=false;
        socAuthenticated_=true;
    }
    else if (rootAuthenticationPending_)
    {
        rootAuthenticationPending_=false;
        rootAuthenticated_=true;
    }
    else if(spAuthenticationPending_)
    {
        spAuthenticationPending_=false;
        spAuthenticated_=true;
    }

    memcpy(&((cmpRspAuthenticate_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdAuthenticate_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

/*
    NOTE that this works in co-operation with mcRegistryStoreRoot, that checks that the written root container is ROOCONT
*/
void handleRootContLockByRoot(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresRootWrite_=true;

    memcpy(&((cmpRspRootContLockByRoot_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdRootContLockByRoot_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspRootContLockByRoot_t*)(cmpRspP))->rsp.sdata.soRootCont, &ROOTCONT , sizeof(ROOTCONT));    

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(ROOTCONT));
    memcpy(containerTargetP, &ROOTCONT, sizeof(ROOTCONT));
}

void handleRootContRegisterActivate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!socAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresRootWrite_=true;
    callRequiresAuthTokenDelete_=true;    

    memcpy(&((cmpRspRootContRegisterActivate_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdRootContRegisterActivate_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));   
    memcpy(&((cmpRspRootContRegisterActivate_t*)(cmpRspP))->rsp.sdata.soRootCont, &ROOTCONT, sizeof(ROOTCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(ROOTCONT));
    memcpy(containerTargetP, &ROOTCONT, sizeof(ROOTCONT));
}

void handleRootContUnlockByRoot(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresRootWrite_=true;

    memcpy(&((cmpRspRootContUnlockByRoot_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdRootContUnlockByRoot_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));
    
    memcpy(&((cmpRspRootContUnlockByRoot_t*)(cmpRspP))->rsp.sdata.soRootCont, &ROOTCONT, sizeof(ROOTCONT));
    
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(ROOTCONT));
    memcpy(containerTargetP, &ROOTCONT, sizeof(ROOTCONT));           
}

void handleRootContUnregister(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresRootCleanup_=true;

    memcpy(((cmpRspRootContUnregister_t*)(cmpRspP))->rsp.sdata.ed.edata.suid.suidData.data, 
           ((cmpCmdRootContUnregister_t*)(cmpP))->cmd.sdata.ed.edata.suid.suidData.data, sizeof(SOFTWARE_SUID));
   
    memcpy(&((cmpRspRootContUnregister_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdRootContUnregister_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

}

void handleSpContActivate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresSpWrite_=true;

    storedSpid_=((cmpCmdSpContActivate_t*)(cmpP))->cmd.sdata.spid;

    memcpy(&((cmpRspSpContActivate_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdSpContActivate_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));
    memcpy(&((cmpRspSpContActivate_t*)(cmpRspP))->rsp.sdata.soSpCont, &SPCONT, sizeof(SPCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
    
    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, &SPCONT, sizeof(SPCONT));    
}

void handleSpContLockByRoot(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{

    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresSpWrite_=true;

    storedSpid_=((cmpCmdSpContLockByRoot_t*)(cmpP))->cmd.sdata.spid;
    
    memcpy(&((cmpRspSpContLockByRoot_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdSpContLockByRoot_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));
    memcpy(&((cmpRspSpContLockByRoot_t*)(cmpRspP))->rsp.sdata.soSpCont, cntP, sizeof(SPCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, cntP, sizeof(SPCONT));
}

void handleSpContLockBySp(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresSpWrite_=true;

    storedSpid_=((cmpCmdSpContLockBySp_t*)(cmpP))->cmd.sdata.spid;
    
    memcpy(&((cmpRspSpContLockBySp_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdSpContLockBySp_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));
    memcpy(&((cmpRspSpContLockBySp_t*)(cmpRspP))->rsp.sdata.soSpCont, &SPCONT, sizeof(SPCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, &SPCONT, sizeof(SPCONT));    
}

void handleSpContRegister(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresSpWrite_=true;
    callRequiresRootWrite_=true;

    storedSpid_=((cmpCmdSpContRegister_t*)(cmpP))->cmd.sdata.spid;
    
    memcpy(&((cmpRspSpContRegister_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdSpContRegister_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspSpContRegister_t*)(cmpRspP))->rsp.sdata.soSpCont, &SPCONT, sizeof(SPCONT));
    memcpy(&((cmpRspSpContRegister_t*)(cmpRspP))->rsp.sdata.soRootCont, &ROOTCONT, sizeof(ROOTCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(ROOTCONT));
    memcpy(containerTargetP, &ROOTCONT, sizeof(ROOTCONT));

    containerTargetP=setElement(3, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, &SPCONT, sizeof(SPCONT));
}

void handleSpContRegisterActivate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    handleSpContRegister(cmpP, cmpRspP); // relying the message structure to be the same
}

void handleSpContUnlockByRoot(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    handleSpContLockByRoot( cmpP, cntP, cmpRspP); // relying on the message format staying the same
}

void handleSpContUnlockBySp(uint8_t* cmpP, uint8_t* cmpRspP)
{
    handleSpContLockBySp(cmpP, cmpRspP); // relying on the message format staying the same
}

void handleSpContUnregister(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresRootWrite_=true;
    callRequiresSpCleanup_=true;
        
    memcpy(&((cmpRspSpContUnregister_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdSpContUnregister_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));
    
    memcpy(&((cmpRspSpContUnregister_t*)(cmpRspP))->rsp.sdata.soRootCont, &ROOTCONT, sizeof(ROOTCONT));
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(ROOTCONT));
    memcpy(containerTargetP, &ROOTCONT, sizeof(ROOTCONT));
}


void handleTltContActivate(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresTltWrite_=true;

    storedSpid_=((cmpCmdTltContActivate_t*)(cmpP))->cmd.sdata.spid;

    memcpy(&storedTltUuid_,
           &((cmpCmdTltContActivate_t*)(cmpP))->cmd.sdata.uuid,sizeof(mcUuid_t));

    memcpy(&((cmpRspTltContActivate_t*)(cmpRspP))->rsp.mac,
           &((cmpCmdTltContActivate_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspTltContActivate_t*)(cmpRspP))->rsp.sdata.soTltCont, cntP, sizeof(TLTCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(TLTCONT));
    memcpy(containerTargetP, cntP, sizeof(TLTCONT));
}            

void handleTltContLockBySp(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresTltWrite_=true;

    storedSpid_=((cmpCmdTltContLockBySp_t*)(cmpP))->cmd.sdata.spid;
    memcpy(&storedTltUuid_,&((cmpCmdTltContLockBySp_t*)(cmpP))->cmd.sdata.uuid,sizeof(mcUuid_t));

    memcpy(&((cmpRspTltContLockBySp_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdTltContLockBySp_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspTltContLockBySp_t*)(cmpRspP))->rsp.sdata.soTltCont, cntP, sizeof(TLTCONT));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(TLTCONT));
    memcpy(containerTargetP, cntP, sizeof(TLTCONT));
}


void handleTltContPersonalize(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

/* checking incoming parameters */
    if(memcmp(&TLTCONT, (mcSoTltCont_t*)cntP , sizeof(TLTCONT)))
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_INCORRECT_PARAMETERS;
        LOGI("mcStub:handleTltContPersonalize not correctly formatted tlt cont");
        return;
    }
// comparing just the first 140 bytes since we do not use the whole buffer here.

    if(memcmp(&DATACONT, &((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.sdata.ed.edata.dataCont , 140)!=0)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_INCORRECT_PARAMETERS;
        LOGI("mcStub:handleTltContPersonalize not correctly formatted data cont");
        return;
    }


    uint32_t len=((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.sdata.edLen;
    storedSpid_=((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.sdata.spid;
    memcpy(&storedTltUuid_,&((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.sdata.uuid,sizeof(mcUuid_t));

    memcpy(&((cmpRspTltContPersonalize_t*)(cmpRspP))->rsp.mac,
           &((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspTltContPersonalize_t*)(cmpRspP))->rsp.sdata.soDataCont, 
           &((cmpCmdTltContPersonalize_t*)(cmpP))->cmd.sdata.ed.edata.dataCont, len);

    ((cmpRspTltContPersonalize_t*)(cmpRspP))->rsp.sdata.soDataContLen=len;
   
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

}

void handleTltContRegister(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresTltWrite_=true;
    callRequiresSpWrite_=true;

    storedSpid_=((cmpCmdTltContRegister_t*)(cmpP))->cmd.sdata.spid;
    memcpy(&storedTltUuid_,&((cmpCmdTltContRegister_t*)(cmpP))->cmd.sdata.uuid,sizeof(mcUuid_t));

    memcpy(&((cmpRspTltContRegister_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdTltContRegister_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspTltContRegister_t*)(cmpRspP))->rsp.sdata.soSpCont, &SPCONT, sizeof(SPCONT));    
    memcpy(&((cmpRspTltContRegister_t*)(cmpRspP))->rsp.sdata.soTltCont, &TLTCONT, sizeof(TLTCONT));
    
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)

    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, &SPCONT, sizeof(SPCONT));

    containerTargetP=setElement(3, storedWsm_, sizeof(TLTCONT));
    memcpy(containerTargetP, &TLTCONT, sizeof(TLTCONT));
}

void handleTltContRegisterActivate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    handleTltContRegister(cmpP, cmpRspP); // relying on the message structure being the same
}            

void handleTltContUnlockBySp(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    handleTltContLockBySp(cmpP, cntP, cmpRspP); // relying on the message structure being the same
}

void handleTltContUnregister(uint8_t* cmpP, uint8_t* cmpRspP)
{
    if(!spAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }

    callRequiresTltCleanup_=true;
    callRequiresSpWrite_=true;

    storedSpid_=((cmpCmdTltContUnregister_t*)(cmpP))->cmd.sdata.spid;
    memcpy(&storedTltUuid_,&((cmpCmdTltContUnregister_t*)(cmpP))->cmd.sdata.uuid,sizeof(mcUuid_t));

    memcpy(&((cmpRspTltContUnregister_t*)(cmpRspP))->rsp.mac, 
           &((cmpCmdTltContUnregister_t*)(cmpP))->cmd.mac, sizeof(cmpMac_t));

    memcpy(&((cmpRspTltContUnregister_t*)(cmpRspP))->rsp.sdata.soSpCont, &SPCONT, sizeof(SPCONT));    


    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)


    uint8_t* containerTargetP=setElement(2, storedWsm_, sizeof(SPCONT));
    memcpy(containerTargetP, &SPCONT, sizeof(SPCONT));

}

void handleAuthenticateTerminate(uint8_t* cmpP, uint8_t* cmpRspP)
{
    
    if(!spAuthenticated_ && !socAuthenticated_ && !rootAuthenticated_)
    {
        ((cmpResponseHeader_t*)(cmpRspP))->returnCode=RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED;
        return;
    }
    
    socAuthenticated_=false;
    rootAuthenticated_=false;
    spAuthenticated_=false;

    memcpy(&((cmpRspAuthenticateTerminate_t*)(cmpRspP))->rsp.mac, &MAC, sizeof(cmpMac_t));
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

/*
    NOTE that this works in co-operation with mcRegistryReadRoot, it copies the values written by that
    NOTE that both command and response use the same buffer, the areas copied here from command to response must not overlap
*/
void handleBeginRootAuthentication(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    rootAuthenticationPending_=true;

    memcpy(((cmpRspBeginRootAuthentication_t*)(cmpRspP))->rsp.sdata.suid.suidData.data, &SOFTWARE_SUID, sizeof(SOFTWARE_SUID));

    ((cmpRspBeginRootAuthentication_t*)(cmpRspP))->rsp.sdata.suid.sipId=0;
    
    memcpy(&((cmpRspBeginRootAuthentication_t*)(cmpRspP))->rsp.sdata.rnd1,
           &((mcSoRootCont_t*)(cntP))->cont ,sizeof(cmpRnd8_t));

    memcpy(&((cmpRspBeginRootAuthentication_t*)(cmpRspP))->rsp.mac,
           &((mcSoRootCont_t*)(cntP))->hashAndPad,sizeof(cmpMac_t));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

/*
    NOTE that both command and response use the same buffer, the areas copied here from command to response must not overlap
*/
void handleBeginSocAuthentication(uint8_t* cmpP, uint8_t* cmpRspP)
{
    socAuthenticationPending_=true;
    
    memcpy(((cmpRspBeginSocAuthentication_t*)(cmpRspP))->rsp.sdata.suid.suidData.data, &SOFTWARE_SUID, sizeof(SOFTWARE_SUID));
    ((cmpRspBeginSocAuthentication_t*)(cmpRspP))->rsp.sdata.suid.sipId=0;
    memset(&((cmpRspBeginSocAuthentication_t*)(cmpRspP))->rsp.sdata.rnd1,0xF0,sizeof(cmpRnd8_t));
    memcpy(&((cmpRspBeginSocAuthentication_t*)(cmpRspP))->rsp.mac, &MAC, sizeof(cmpMac_t));
    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

/*
    NOTE that this works in co-operation with mcRegistryReadRoot and mcRegistryReadSp, it copies the values written by those
    NOTE that both command and response use the same buffer, the areas copied here from command to response must not overlap
*/
void handleBeginSpAuthentication(uint8_t* cmpP, uint8_t* cntP, uint8_t* cmpRspP)
{
    spAuthenticationPending_=true;

    memcpy(((cmpRspBeginSpAuthentication_t*)(cmpRspP))->rsp.sdata.suid.suidData.data, &SOFTWARE_SUID, sizeof(SOFTWARE_SUID));
    ((cmpRspBeginSpAuthentication_t*)(cmpRspP))->rsp.sdata.suid.sipId=0;
    
    ((cmpRspBeginSpAuthentication_t*)(cmpRspP))->rsp.sdata.spid=((cmpCmdBeginSpAuthentication_t*)(cmpP))->cmd.spid;
    
    memcpy(&((cmpRspBeginSpAuthentication_t*)(cmpRspP))->rsp.sdata.rnd1,
           &((mcSoSpCont_t*)(cntP))->cont ,sizeof(cmpRnd8_t));
    memcpy(&((cmpRspBeginSpAuthentication_t*)(cmpRspP))->rsp.mac,
           &((mcSoSpCont_t*)(cntP))->hashAndPad,sizeof(cmpMac_t));

    ((cmpResponseHeader_t*)(cmpRspP))->returnCode=SUCCESSFUL; // (tlCmError.h)
}

uint32_t mcStub_getResponseSize(uint32_t commandId)
{
    switch(commandId)
    {
        case MC_CMP_CMD_GET_SUID:
            return sizeof(cmpRspGetSuid_t);
            break;
        case MC_CMP_CMD_GET_VERSION:
            return sizeof(cmpRspGetVersion_t);
            break;
        case MC_CMP_CMD_GENERATE_AUTH_TOKEN:
            return sizeof(cmpRspGenAuthToken_t);
            break;
        case MC_CMP_CMD_AUTHENTICATE:
            return sizeof(cmpRspAuthenticate_t);
            break;
        case MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT:
            return sizeof(cmpRspRootContLockByRoot_t);
            break;
        case MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE:
            return sizeof(cmpRspRootContRegisterActivate_t);
            break;
        case MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT :
            return sizeof(cmpRspRootContUnlockByRoot_t);
            break;
        case MC_CMP_CMD_ROOT_CONT_UNREGISTER:
            return sizeof(cmpRspRootContUnregister_t);
            break;
        case MC_CMP_CMD_SP_CONT_ACTIVATE:
            return sizeof(cmpRspSpContActivate_t);
            break;
        case MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT:
            return sizeof(cmpRspSpContLockByRoot_t);
            break;
        case MC_CMP_CMD_SP_CONT_LOCK_BY_SP:
            return sizeof(cmpRspSpContLockBySp_t);
            break;
        case MC_CMP_CMD_SP_CONT_REGISTER:
            return sizeof(cmpRspSpContRegister_t);
            break;
        case MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE:
            return sizeof(cmpRspSpContRegisterActivate_t);
            break;
        case MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT:
            return sizeof(cmpRspSpContUnlockByRoot_t);
            break;
        case MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP:
            return sizeof(cmpRspSpContLockBySp_t);
            break;
        case MC_CMP_CMD_SP_CONT_UNREGISTER:
            return sizeof(cmpRspSpContUnregister_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_ACTIVATE):
            return sizeof(cmpRspTltContActivate_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_LOCK_BY_SP):
            return sizeof(cmpRspTltContLockBySp_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_PERSONALIZE):
            return sizeof(cmpRspTltContPersonalize_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_REGISTER):
            return sizeof(cmpRspTltContRegister_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE):
            return sizeof(cmpRspTltContRegisterActivate_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP):
            return sizeof(cmpRspTltContUnlockBySp_t);
            break;
        case(MC_CMP_CMD_TLT_CONT_UNREGISTER):
            return sizeof(cmpRspTltContUnregister_t);
            break;  
        case(MC_CMP_CMD_AUTHENTICATE_TERMINATE):
            return sizeof(cmpRspAuthenticateTerminate_t);
            break;
        case(MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION):
            return sizeof(cmpRspBeginRootAuthentication_t);
            break;
        case(MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION):
            return sizeof(cmpRspBeginSocAuthentication_t);
            break;
        case(MC_CMP_CMD_BEGIN_SP_AUTHENTICATION):
            return sizeof(cmpRspBeginSpAuthentication_t);
            break;
        default:
            return 0;    
    }
}

void handleMessage()
{
    /*
    This is to check that previous command really called the registory service it was supposed to. 
    It is recognized that this will not work with the last test case executed, but it can be handled 
    with the order of the test cases (last one should not require registry).
    */
    if(callRequiresRootWrite_ || callRequiresSpWrite_ || callRequiresTltWrite_ || 
       callRequiresAuthTokenWrite_ || callRequiresDataWrite_ || callRequiresRootCleanup_ ||
       callRequiresAuthTokenDelete_)
    {
        LOGI("mcStub: FAILURE, previous command %d did not call registry: \n  \
RootWrite_ %d SpWrite_ %d TltWrite_ %d AuthTokenWrite_ %d DataWrite_ %d \
RootCleanup_ %d AuthTokenDelete_ %d SpCleanup_ %d TltCleanup_ %d", 
                                                  lastCommand_,
                                                  callRequiresRootWrite_, 
                                                  callRequiresSpWrite_, 
                                                  callRequiresTltWrite_, 
                                                  callRequiresAuthTokenWrite_, 
                                                  callRequiresDataWrite_, 
                                                  callRequiresRootCleanup_,
                                                  callRequiresAuthTokenDelete_,
                                                  callRequiresSpCleanup_,
                                                  callRequiresTltCleanup_); 
        callRequiresRootWrite_=false;
        callRequiresSpWrite_=false;
        callRequiresTltWrite_=false;
        callRequiresAuthTokenWrite_=false;
        callRequiresDataWrite_=false;
        callRequiresRootCleanup_=false;
        callRequiresAuthTokenDelete_=false;        
        callRequiresSpCleanup_=false;
        callRequiresTltCleanup_=false;
    }


    cmpCommandHeaderTci_t* commandInfoP = (cmpCommandHeaderTci_t*) storedWsm_;
    
    cmpVersionId_t version=commandInfoP->version;
    cmpCommandId_t commandId=commandInfoP->commandId;
    if(NEW_CMP_VERSION!=version)
    {
        LOGE("mcStub: FAILURE not valid version 0x%x %d", version, commandId);
    }
    LOGI("mcStub: received wsmP_ ver=0x%x msgId=0x%x (%d) mappedP=%ld mappedSize=%d elementOff=%d elementSize=%d", commandInfoP->version, 
                                                                                                                  commandInfoP->commandId, 
                                                                                                                  commandInfoP->commandId, 
                                                                                                                  (long int) commandInfoP->mapInfo.addr, 
                                                                                                                  commandInfoP->mapInfo.len, 
                                                                                                                  commandInfoP->cmpCmdMapOffsetInfo.offset, 
                                                                                                                  commandInfoP->cmpCmdMapOffsetInfo.len);
    if(mappedMemory_!=NULL)
    {
        commandId=*((int*)mappedMemory_);
        if((void*)mappedMemory_!=commandInfoP->mapInfo.addr)
        {
            LOGI("mcStub: FAILURE, mappedMemory_ %ld different from the one received %ld\n", (long int) mappedMemory_, (long int) commandInfoP->mapInfo.addr);
            return; 
        }
    }
    else
    {
        LOGI("mcStub: FAILURE, no mappedMemory_ when using new CMP protocol\n");
        return; 
    }

    lastCommand_=commandId;
        
    uint32_t cmpCommandLength=0;

    uint32_t cnt1Length=0;
    uint32_t cnt2Length=0;

    uint8_t* origCmpP=getElement(1, storedWsm_, &cmpCommandLength);
    uint8_t* origCnt1P=getElement(2, storedWsm_, &cnt1Length);
    uint8_t* origCnt2P=getElement(3, storedWsm_, &cnt2Length);

    if(NULL==origCmpP || 0 == cmpCommandLength)
    {
        LOGE("FAILURE: mcStub:handleMessage getting element failed %ld", (long int) origCmpP);
        return;
    }    
    uint8_t* cmpP=malloc(cmpCommandLength+300);  // TODO-Tero: this +300 is here to get rid of Valgrind varnings. It seems that (at least) one of the messages   
    memset(cmpP, 0, cmpCommandLength+300);       // that has size of 116 when received from RootPA/Client is understood bigger in mcStub and it causes problems with Valgrind
    memcpy(cmpP, origCmpP, cmpCommandLength);    // very likely it is handleAuthenticate
    
    uint8_t* cnt1P=NULL;
    uint8_t* cnt2P=NULL;

    if(origCnt1P)
    {
        cnt1P=malloc(cnt1Length);
        memcpy(cnt1P, origCnt1P, cnt1Length);
    }

    if(origCnt2P)
    {
        cnt2P=malloc(cnt2Length);
        memcpy(cnt2P, origCnt2P, cnt2Length);
    }

    memset(storedWsm_, 0, sizeof(cmp_t));    
    uint8_t* cmpRspP=setElement(1, storedWsm_, mcStub_getResponseSize(commandId));
    memset(cmpRspP, 0, mcStub_getResponseSize(commandId));
    cmpResponseHeaderTci_t* responseInfoP = (cmpResponseHeaderTci_t*) storedWsm_;    

    *((uint32_t*)(mappedMemory_))=RSP_ID(commandId);
    
    if(sessionOpen_==false)
    {
        LOGI("mcStub.handleMessage: FAILURE, session not open");
        ((cmpResponseHeader_t*)(storedWsm_))->responseId=RSP_ID(commandId);
        ((cmpResponseHeader_t*)(storedWsm_))->returnCode=MC_DRV_ERR_UNKNOWN_SESSION;
    }
    else switch(commandId)
    {
        case MC_CMP_CMD_GET_SUID:
            handleGetSuid(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_GET_VERSION:
            handleGetVersion(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_GENERATE_AUTH_TOKEN:
            handleGenerateAuthToken(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_AUTHENTICATE:
            handleAuthenticate(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT:
            handleRootContLockByRoot(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE:
            handleRootContRegisterActivate(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT :
            handleRootContUnlockByRoot(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_ROOT_CONT_UNREGISTER:
            handleRootContUnregister(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_ACTIVATE:
            handleSpContActivate(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT:
            handleSpContLockByRoot(cmpP, cnt1P, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_LOCK_BY_SP:
            handleSpContLockBySp(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_REGISTER:
            handleSpContRegister(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE:
            handleSpContRegisterActivate(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT:
            handleSpContUnlockByRoot(cmpP, cnt1P, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP:
            handleSpContUnlockBySp(cmpP, cmpRspP);
            break;
        case MC_CMP_CMD_SP_CONT_UNREGISTER:
            handleSpContUnregister(cmpP, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_ACTIVATE):
            handleTltContActivate(cmpP, cnt1P, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_LOCK_BY_SP):
            handleTltContLockBySp(cmpP, cnt1P, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_PERSONALIZE):
            handleTltContPersonalize(cmpP, cnt1P, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_REGISTER):
            handleTltContRegister(cmpP, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE):
            handleTltContRegisterActivate(cmpP, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP):
            handleTltContUnlockBySp(cmpP, cnt1P, cmpRspP);
            break;
        case(MC_CMP_CMD_TLT_CONT_UNREGISTER):
            handleTltContUnregister(cmpP, cmpRspP);
            break;  
        case(MC_CMP_CMD_AUTHENTICATE_TERMINATE):
            handleAuthenticateTerminate(cmpP, cmpRspP);
            break;
        case(MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION):
            handleBeginRootAuthentication(cmpP, cnt1P, cmpRspP);
            break;
        case(MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION):
            handleBeginSocAuthentication(cmpP, cmpRspP);
            break;
        case(MC_CMP_CMD_BEGIN_SP_AUTHENTICATION):
            handleBeginSpAuthentication(cmpP, cnt2P, cmpRspP); // the test only uses SP container
            break;
        default:
            ((cmpResponseHeader_t*)(storedWsm_))->returnCode=RET_ERR_EXT_UNKNOWN_COMMAND;
            LOGI("mcStub: Unknown command %d", commandId);
    
    }
    free(cmpP);
    free(cnt1P);
    free(cnt2P);

    responseInfoP->version=NEW_CMP_VERSION;
    responseInfoP->responseId=*((uint32_t*)(mappedMemory_));
    responseInfoP->len=0;
    LOGI("mcStub: returning wsmP_ ver=0x%x rspId=0x%x neededBytes=%d elementoff=%d, elementSize=%d container1off=%d, container1Size=%d", 
                                                        *((int*)storedWsm_), 
                                                        *((int*)(storedWsm_+4)), 
                                                        *((int*)(storedWsm_+8)), 
                                                        *((int*)(storedWsm_+12)), 
                                                        *((int*)(storedWsm_+16)), 
                                                        *((int*)(storedWsm_+20)), 
                                                        *((int*)(storedWsm_+24)));

    return;
}

