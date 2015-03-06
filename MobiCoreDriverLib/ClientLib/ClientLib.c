/*
 * Copyright (c) 2013-2015 TRUSTONIC LIMITED
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

#include <stdint.h>

#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#include "mc_linux.h"
#endif

#include "mcVersionHelper.h"
#include "log.h"
#include "public/MobiCoreDriverApi.h"
#include "buildTag.h"

#ifndef WIN32

static const __attribute__((used)) char* buildtag = MOBICORE_COMPONENT_BUILD_TAG;

MC_CHECK_VERSION(MCDRVMODULEAPI, 2, 0);
//------------------------------------------------------------------------------

/*
 * On mcCloseDevice(), all mappings need to be removed, which means we need to
 * keep them somewhere.  We keep them in an array, resized when needed.
 * Both map and length must be kept in user-space, as after a fork another
 * mapping with same address may have appeared from the other process(es).
 */
struct maplen {
    uint8_t* wsm;
    uint32_t len;
};
static struct maplen* wsms;
static size_t wsms_capacity, wsms_length;
static pthread_mutex_t wsms_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline int wsms_add(uint8_t* wsm, uint32_t len)
{
    int ret = 0;
    pthread_mutex_lock(&wsms_mutex);
    if (wsms_length == wsms_capacity) {
        size_t new_wsms_capacity = wsms_capacity;
        /* Need some room */
        if (new_wsms_capacity == 0) {
            new_wsms_capacity = 16;
        } else {
            new_wsms_capacity *= 2;
        }
        struct maplen* new_wsms = realloc(wsms, new_wsms_capacity * sizeof(*wsms));
        if (!new_wsms) {
            LOG_ERRNO("resize WSMs array");
            ret = -1;
        } else {
            wsms = new_wsms;
            wsms_capacity = new_wsms_capacity;
        }
    }
    if (ret == 0) {
        wsms[wsms_length].wsm = wsm;
        wsms[wsms_length].len = len;
        wsms_length++;
    }
    pthread_mutex_unlock(&wsms_mutex);
    return ret;
}

static inline int wsms_remove(uint8_t* wsm)
{
    int ret = 0;
    pthread_mutex_lock(&wsms_mutex);
    /* Find position of WSM */
    size_t i;
    for (i = 0; i < wsms_length; i++) {
        if (wsms[i].wsm == wsm) {
            break;
        }
    }
    if (i == wsms_length) {
        LOG_E("WSM %p not found in array", wsm);
        ret = -1;
    } else {
        /* Replace WSM with last so the first free element remains wsms[wsms_length] */
        wsms[i] = wsms[--wsms_length];
    }
    pthread_mutex_unlock(&wsms_mutex);
    return ret;
}

static inline uint32_t wsms_len(uint8_t* wsm)
{
    uint32_t len = 0;
    pthread_mutex_lock(&wsms_mutex);
    for (size_t i = 0; i < wsms_length; i++) {
        if (wsms[i].wsm == wsm) {
            len = wsms[i].len;
            break;
        }
    }
    pthread_mutex_unlock(&wsms_mutex);
    return len;
}

static inline struct maplen wsms_removeLast(void)
{
    struct maplen wsm;
    pthread_mutex_lock(&wsms_mutex);
    if (wsms_length > 0) {
        wsm = wsms[--wsms_length];
    } else {
        wsm.wsm = NULL;
        // To make Coverity happy
        wsm.len = 0;
    }
    pthread_mutex_unlock(&wsms_mutex);
    return wsm;
}

/* Static data */

/* Our unique file descriptor */
static int fdClient = -1;
/* Mutex to match openCount with open/close state of device */
static pthread_mutex_t dev_mutex = PTHREAD_MUTEX_INITIALIZER;
/* Number of times user called mcOpenDevice() */
static uint32_t openCount = 0;

/* Only 1 device is supported */
static inline bool isValidDevice(uint32_t deviceId)
{
	return (MC_DEVICE_ID_DEFAULT == deviceId);
}

/* Convert driver errors into tbase set */
static mcResult_t convert_syscall_error(int32_t ret)
{
        switch (ret) {
        case 0:
        	return MC_DRV_OK;
        case -1:
                switch (errno) {
                case ENOMSG:
                	return MC_DRV_NO_NOTIFICATION;
                case EBADMSG:
                	return MC_DRV_ERR_NOTIFICATION;
                case ENOSYS:
                	return MC_DRV_ERR_NOT_IMPLEMENTED;
                case ENOSPC:
                	return MC_DRV_ERR_OUT_OF_RESOURCES;
                case EHOSTDOWN:
                	return MC_DRV_ERR_INIT;
                case ENODEV:
                	return MC_DRV_ERR_UNKNOWN_DEVICE;
                case ENXIO:
                	return MC_DRV_ERR_UNKNOWN_SESSION;
                case EPERM:
                	return MC_DRV_ERR_INVALID_OPERATION;
                case EBADE:
                	return MC_DRV_ERR_INVALID_RESPONSE;
                case ETIME:
                	return MC_DRV_ERR_TIMEOUT;
                case ENOMEM:
                        return MC_DRV_ERR_NO_FREE_MEMORY;
                case EUCLEAN:
                	return MC_DRV_ERR_FREE_MEMORY_FAILED;
                case ENOTEMPTY:
                	return MC_DRV_ERR_SESSION_PENDING;
                case EHOSTUNREACH:
                	return MC_DRV_ERR_DAEMON_UNREACHABLE;
                case ENOENT:
                	return MC_DRV_ERR_INVALID_DEVICE_FILE;
                case EINVAL:
                	return MC_DRV_ERR_INVALID_PARAMETER;
                case EPROTO:
                	return MC_DRV_ERR_KERNEL_MODULE;
                case EADDRINUSE:
                	return MC_DRV_ERR_BULK_MAPPING;
                case EADDRNOTAVAIL:
                	return MC_DRV_ERR_BULK_UNMAPPING;
                case ECOMM:
                	return MC_DRV_INFO_NOTIFICATION;
                case EUNATCH:
                	return MC_DRV_ERR_NQ_FAILED;
                case EBADF:
                	return MC_DRV_ERR_DAEMON_DEVICE_NOT_OPEN;
                case EINTR:
                	return MC_DRV_ERR_INTERRUPTED_BY_SIGNAL;
                default:
                        LOG_ERRNO("syscall");
                        return MC_DRV_ERR_UNKNOWN;
                }
        default:
        	LOG_E("Unknown code %d", ret);
                return MC_DRV_ERR_UNKNOWN;
        }
}

/* Check the version of the driver */
static bool checkVersion(int fh)
{
    uint32_t version;

    int ret = ioctl(fh, MC_IO_DR_VERSION, &version);
    if (ret) {
        convert_syscall_error(ret);
        return false;
    }

    // Run-time check.
    char *errmsg;
    if (!checkVersionOkMCDRVMODULEAPI(version, &errmsg)) {
        LOG_E("%s", errmsg);
        return false;
    }

    return true;
}


#endif /* WIN32 */

//------------------------------------------------------------------------------
//		CLIENT API
//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcOpenDevice(
    uint32_t deviceId
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32

    //__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", MOBICORE_COMPONENT_BUILD_TAG);

    pthread_mutex_lock(&dev_mutex);
    LOG_I("===%s(%i)===", __FUNCTION__, deviceId);

    do {
        // Check parameters
	if (!isValidDevice(deviceId)) {
		mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
		break;
	}

        /* Get a temporary file descriptor */
        if (!openCount) {
            int h = open("/dev/" MC_USER_DEVNODE, O_RDWR | O_CLOEXEC);
            if(h == -1) {
                mcResult = convert_syscall_error(h);
                LOG_E("failed, err %u", mcResult);
                break;
            }

            /* Validate fd only if driver version is ok */
            if(checkVersion(h)) {
                fdClient = h;
            } else {
                close(h);
                mcResult = MC_DRV_ERR_INIT;
                break;
            }
        }

        openCount++;
        mcResult = MC_DRV_OK;
    } while (false);

    pthread_mutex_unlock(&dev_mutex);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcCloseDevice(
    uint32_t deviceId
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    int ret;

    pthread_mutex_lock(&dev_mutex);
    LOG_I("===%s(%i)===", __FUNCTION__, deviceId);

    do {
        // Check parameters
	if (!isValidDevice(deviceId)) {
		mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
		break;
	}

        // Check if not used by another thread
        if (openCount) {
            openCount--;
        }
        if (openCount) {
            mcResult = MC_DRV_OK;
            break;
        }

        // Check that all sessions are closed and freeze client
        ret = ioctl(fdClient, MC_IO_FREEZE);
        mcResult = convert_syscall_error(ret);
        if (MC_DRV_OK != mcResult) {
            break;
        }

        // Free all remaining WSMs
        while (true) {
            struct maplen wsm = wsms_removeLast();
            if (wsm.wsm == NULL) {
                break;
            }
            mcFreeWsm(deviceId, wsm.wsm);
        }

        // Close the device
        close(fdClient);
    } while (false);

    pthread_mutex_unlock(&dev_mutex);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcOpenSession(
    mcSessionHandle_t  *session,
    const mcUuid_t     *uuid,
    uint8_t            *tci,
    uint32_t           len
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_open_sess sess;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		if (!uuid) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		// Call ioctl
		sess.sid = 0;
		memcpy( (void *)&sess.uuid, uuid, sizeof(sess.uuid) );
		sess.tci = (uintptr_t)tci;
		sess.tcilen = len;
		sess.is_gp_uuid = false;
		ret = ioctl(fdClient, MC_IO_OPEN_SESSION, &sess);
		mcResult = convert_syscall_error(ret);
		if (MC_DRV_OK != mcResult) {
			break;
		}

		// Fill in return parameters
		session->sessionId = sess.sid;
    } while (false);

    /* Legacy error matching */
    if (MC_DRV_OK != mcResult) {
	    if (MC_DRV_ERR_UNKNOWN == mcResult)
		    mcResult = MC_DRV_ERR_INVALID_PARAMETER;

	    LOG_E("return 0x%x", mcResult);
    }

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcOpenGPTA(
    mcSessionHandle_t  *session,
    const mcUuid_t     *uuid,
    uint8_t            *tci,
    uint32_t           len
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_open_sess sess;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
                if (!uuid) {
                        LOG_E("uuid pointer is null");
                        mcResult = MC_DRV_ERR_INVALID_PARAMETER;
                        break;
                }

		// Call ioctl
		sess.sid = 0;
                memcpy( (void *)&sess.uuid, uuid, sizeof(sess.uuid) );
		sess.tci = (uintptr_t)tci;
		sess.tcilen = len;
		sess.is_gp_uuid = true;
		ret = ioctl(fdClient, MC_IO_OPEN_SESSION, &sess);
		mcResult = convert_syscall_error(ret);
		if (MC_DRV_OK != mcResult) {
			break;
		}

		// Fill in return parameters
		session->sessionId = sess.sid;
    } while (false);

    /* Legacy error matching */
    if (MC_DRV_OK != mcResult) {
	    if (MC_DRV_ERR_UNKNOWN == mcResult)
		    mcResult = MC_DRV_ERR_INVALID_PARAMETER;

	    LOG_E("return 0x%x", mcResult);
    }

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcOpenTrustlet(
    mcSessionHandle_t  *session,
    mcSpid_t           spid,
    uint8_t            *trustlet,
    uint32_t           tLen,
    uint8_t            *tci,
    uint32_t           tciLen
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_open_trustlet taDesc;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
                // Check parameters
                if (!session) {
                	LOG_E("Session handle is null");
                        mcResult = MC_DRV_ERR_INVALID_PARAMETER;
                        break;
                }
        	if (!isValidDevice(session->deviceId)) {
        		mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
        		break;
        	}

                // Call ioctl
                taDesc.sid = 0;
                taDesc.spid = spid;
                taDesc.buffer = (uintptr_t)trustlet;
                taDesc.tlen = tLen;
                taDesc.tci = (uintptr_t)tci;
                taDesc.tcilen = tciLen;
                ret = ioctl(fdClient, MC_IO_OPEN_TRUSTLET, &taDesc);
                mcResult = convert_syscall_error(ret);

                // Fill in return parameters
                if (MC_DRV_OK == mcResult) {
                        session->sessionId = taDesc.sid;
                }

    } while (false);

    /* Legacy error matching */
    if (MC_DRV_OK != mcResult) {
	    if (MC_DRV_ERR_UNKNOWN == mcResult)
		    mcResult = MC_DRV_ERR_INVALID_PARAMETER;

	    LOG_E("return 0x%x", mcResult);
    }

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcCloseSession(
    mcSessionHandle_t *session
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		// Call ioctl
		ret = ioctl(fdClient, MC_IO_CLOSE_SESSION, session->sessionId);
		mcResult = convert_syscall_error(ret);

    } while (false);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcNotify(
    mcSessionHandle_t   *session
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		// Call ioctl
		ret = ioctl(fdClient, MC_IO_NOTIFY, session->sessionId);
		mcResult = convert_syscall_error(ret);

    } while (false);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcWaitNotification(
    mcSessionHandle_t  *session,
    int32_t            timeout
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_wait wait;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		// Call ioctl
		wait.sid = session->sessionId;
		wait.timeout = timeout;
		do {
			ret = ioctl(fdClient, MC_IO_WAIT, &wait);
			mcResult = convert_syscall_error(ret);
		} while (MC_INFINITE_TIMEOUT == timeout &&
			 MC_DRV_ERR_INTERRUPTED_BY_SIGNAL == mcResult);

    } while (false);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcMallocWsm(
    uint32_t    deviceId,
    uint32_t    align,
    uint32_t    len,
    uint8_t     **wsm,
    uint32_t    wsmFlags
)
{
    (void) align;
    (void) wsmFlags;
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    LOG_I("===%s(len=%i)===", __FUNCTION__, len);

    do {
	// Check parameters
	if (!isValidDevice(deviceId)) {
		mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
		break;
	}
        if (!wsm) {
            LOG_E("Wsm pointer is null");
            mcResult = MC_DRV_ERR_INVALID_PARAMETER;
            break;
        }

	// Alloc and map kernel buffer into user space
	*wsm = (uint8_t *)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED,
	    		       fdClient, 0);
        if (MAP_FAILED == *wsm) {
            mcResult = convert_syscall_error(-1);
            break;
        }
        wsms_add(*wsm, len);
        mcResult = MC_DRV_OK;

    } while (false);

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
// Note: mcFreeWsm() only succeeds if wsm is as returned by mcMallocWsm().
__MC_CLIENT_LIB_API mcResult_t mcFreeWsm(
    uint32_t    deviceId,
    uint8_t     *wsm
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32

    LOG_I("===%s(%p)===", __FUNCTION__, wsm);

    do {
            // Check parameters
            if (!isValidDevice(deviceId)) {
                mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
                break;
            }

            // munmap() takes no FD parameter, but product spec requires client
            // to be open. So we must check ourselves.
            if (!openCount) {
                    mcResult = MC_DRV_ERR_DAEMON_DEVICE_NOT_OPEN;
                    break;
            }

            // Get mapping length
            uint32_t len = wsms_len(wsm);
            if (len == 0) {
                    LOG_E("wsm %p length not available", wsm);
                    mcResult = MC_DRV_ERR_INVALID_PARAMETER;
            } else {
                    // Unmap and free kernel buffer
                    int ret = munmap(wsm, len);
                    mcResult = convert_syscall_error(ret);
                    if (ret == 0) {
                        wsms_remove(wsm);
                    }
            }
    } while (false);

#endif /* WIN32 */
    return mcResult;
}

//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcMap(
    mcSessionHandle_t  *session,
    void               *buf,
    uint32_t           bufLen,
    mcBulkMap_t        *mapInfo
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_map map;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
 		if (!mapInfo) {
			LOG_E("mapInfo pointer is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		// Call ioctl
 		map.sid = session->sessionId;
 		map.buf = (uintptr_t)buf;
 		map.len = bufLen;
		ret = ioctl(fdClient, MC_IO_MAP, &map);
		mcResult = convert_syscall_error(ret);
		if (MC_DRV_OK != mcResult) {
			break;
		}

		// Fill in return parameters
#if ( __WORDSIZE != 64 )
		mapInfo->sVirtualAddr = (void *)map.sva;
#else
		mapInfo->sVirtualAddr = map.sva;
#endif
		mapInfo->sVirtualLen = map.slen;

    } while (false);

    /* Legacy error matching */
    if (MC_DRV_OK != mcResult) {
	    if (MC_DRV_ERR_UNKNOWN == mcResult)
		    mcResult = MC_DRV_ERR_BULK_MAPPING;

	    LOG_E("return 0x%x", mcResult);
    }

#endif /* WIN32 */
    return mcResult;
}

//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcUnmap(
    mcSessionHandle_t  *session,
    void               *buf,
    mcBulkMap_t        *mapInfo
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_map map;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
 		if (!mapInfo) {
			LOG_E("mapInfo pointer is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		// Call ioctl
 		map.sid = session->sessionId;
 		map.buf = (uintptr_t)buf;
		map.sva = (uint32_t)mapInfo->sVirtualAddr;
		map.slen = mapInfo->sVirtualLen;
		ret = ioctl(fdClient, MC_IO_UNMAP, &map);
		mcResult = convert_syscall_error(ret);

    } while (false);

    if (MC_DRV_OK != mcResult) {
	    if (MC_DRV_ERR_UNKNOWN == mcResult)
		    mcResult = MC_DRV_ERR_BULK_UNMAPPING;

	    LOG_E("return 0x%x", mcResult);
    }

#endif /* WIN32 */
    return mcResult;
}


//------------------------------------------------------------------------------
__MC_CLIENT_LIB_API mcResult_t mcGetSessionErrorCode(
    mcSessionHandle_t   *session,
    int32_t             *lastErr
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    struct mc_ioctl_geterr err;
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!session) {
			LOG_E("Session handle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (!isValidDevice(session->deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
 		if (!lastErr) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		// Call ioctl
 		err.sid = session->sessionId;
		ret = ioctl(fdClient, MC_IO_ERR, &err);
		mcResult = convert_syscall_error(ret);
		if (MC_DRV_OK != mcResult) {
			break;
		}

		// Fill in return parameters
		*lastErr = err.value;

    } while (false);

#endif /* WIN32 */
	return mcResult;
}

//------------------------------------------------------------------------------
/* CPI todo: user api only ? */
__MC_CLIENT_LIB_API mcResult_t mcGetMobiCoreVersion(
    uint32_t  deviceId,
    mcVersionInfo_t *versionInfo
)
{
    mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
#ifndef WIN32
    int ret;

    LOG_I("===%s()===", __FUNCTION__);

    do {
		// Check parameters
		if (!isValidDevice(deviceId)) {
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		if (!versionInfo) {
			LOG_E("versionInfo pointer is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		ret = ioctl(fdClient, MC_IO_VERSION, versionInfo);
		mcResult = convert_syscall_error(ret);

    } while (false);

#endif /* WIN32 */
    return mcResult;
}



