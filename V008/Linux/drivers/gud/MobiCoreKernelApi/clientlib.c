/**
 * MobiCore KernelApi module
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009-2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <net/net_namespace.h>
#include <linux/list.h>

#include "public/MobiCoreDriverApi.h"
#include "public/MobiCoreDriverCmd.h"
#include "device.h"
#include "session.h"

/* device list */
LIST_HEAD(devices);

/*----------------------------------------------------------------------------*/
static struct mcore_device_t *resolveDeviceId(
	uint32_t deviceId
) {
	struct mcore_device_t *tmp;
	struct list_head *pos;

	/* Get mcore_device_t for deviceId */
	list_for_each(pos, &devices) {
		tmp = list_entry(pos, struct mcore_device_t, list);
		if (tmp->deviceId == deviceId)
			return tmp;
	}
	return NULL;
}


/*----------------------------------------------------------------------------*/
static void addDevice(
	struct mcore_device_t *device
) {
	list_add_tail(&(device->list), &devices);
}


/*----------------------------------------------------------------------------*/
static bool removeDevice(
	uint32_t deviceId
) {
	struct mcore_device_t *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &devices) {
		tmp = list_entry(pos, struct mcore_device_t, list);
		if (tmp->deviceId == deviceId) {
			list_del(pos);
			mcore_device_cleanup(tmp);
			return true;
		}
	}
	return false;
}


/*----------------------------------------------------------------------------*/
enum mcResult_t mcOpenDevice(
	uint32_t deviceId
) {
	enum mcResult_t mcResult = MC_DRV_OK;
	struct connection_t *devCon = NULL;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {
		struct mcore_device_t *device = resolveDeviceId(deviceId);
		if (NULL != device) {
			MCDRV_DBG_ERROR("Device %d already opened", deviceId);
			mcResult = MC_DRV_ERR_INVALID_OPERATION;
			break;
		}

		/* Open new connection to device */
		devCon = connection_new();
		if (!connection_connect(devCon, MC_DAEMON_PID)) {
			MCDRV_DBG_ERROR(
				"Could not setup netlink connection to PID %u",
				MC_DAEMON_PID);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		/* Forward device open to the daemon and read result */
		struct mcDrvCmdOpenDevice_t mcDrvCmdOpenDevice = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
			/* .commandId = */ MC_DRV_CMD_OPEN_DEVICE
			},
		/* .payload = */ {
		/* .deviceId = */ deviceId
			}
		};

		int len = connection_writeData(
					devCon,
					&mcDrvCmdOpenDevice,
					sizeof(struct mcDrvCmdOpenDevice_t));
		if (len < 0) {
			MCDRV_DBG_ERROR("CMD_OPEN_DEVICE writeCmd failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		struct mcDrvResponseHeader_t  rspHeader;
		len = connection_readDataBlock(
					devCon,
					&rspHeader,
					sizeof(rspHeader));
		if (len != sizeof(rspHeader)) {
			MCDRV_DBG_ERROR("CMD_OPEN_DEVICE readRsp failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}
		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_OPEN_DEVICE failed, respId=%d",
							rspHeader.responseId);
			switch (rspHeader.responseId) {
			case MC_DRV_RSP_PAYLOAD_LENGTH_ERROR:
				mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
				break;
			case MC_DRV_INVALID_DEVICE_NAME:
				mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
				break;
			case MC_DRV_RSP_DEVICE_ALREADY_OPENED:
			default:
				mcResult = MC_DRV_ERR_INVALID_OPERATION;
				break;
			}
			break;
		}

		/* there is no payload to read */

		device = mcore_device_create(deviceId, devCon);
		if (!mcore_device_open(device, MC_DRV_MOD_DEVNODE_FULLPATH)) {
			mcore_device_cleanup(device);
			MCDRV_DBG_ERROR("could not open device file: %s",
				MC_DRV_MOD_DEVNODE_FULLPATH);
			mcResult = MC_DRV_ERR_INVALID_DEVICE_FILE;
			break;
		}

		addDevice(device);

	} while (false);

	if (mcResult != MC_DRV_OK)
		connection_cleanup(devCon);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcOpenDevice);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcCloseDevice(
	uint32_t deviceId
) {
	enum mcResult_t mcResult = MC_DRV_OK;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */
	do {
		struct mcore_device_t *device = resolveDeviceId(deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t *devCon = device->connection;

		/* Return if not all sessions have been closed */
		if (mcore_device_hasSessions(device)) {
			MCDRV_DBG_ERROR("cannot close with sessions pending");
			mcResult = MC_DRV_ERR_SESSION_PENDING;
			break;
		}

		struct mcDrvCmdCloseDevice_t mcDrvCmdCloseDevice = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_CLOSE_DEVICE
			}
		};
		int len = connection_writeData(
					devCon,
					&mcDrvCmdCloseDevice,
					sizeof(struct mcDrvCmdCloseDevice_t));
		/* ignore error, but log details */
		if (len < 0) {
			MCDRV_DBG_ERROR("CMD_CLOSE_DEVICE writeCmd failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
		}

		struct mcDrvResponseHeader_t  rspHeader;
		len = connection_readDataBlock(
					devCon,
					&rspHeader,
					sizeof(rspHeader));
		if (len != sizeof(rspHeader)) {
			MCDRV_DBG_ERROR("CMD_CLOSE_DEVICE readResp failed "
				" ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_CLOSE_DEVICE failed, respId=%d",
							rspHeader.responseId);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		removeDevice(deviceId);

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcCloseDevice);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcOpenSession(
	struct mcSessionHandle_t  *session,
	const struct mcUuid_t	 *uuid,
	uint8_t			*tci,
	uint32_t		   len
) {
	enum mcResult_t mcResult = MC_DRV_OK;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {
		if (NULL == session) {
			MCDRV_DBG_ERROR("Session is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == uuid) {
			MCDRV_DBG_ERROR("UUID is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == tci) {
			MCDRV_DBG_ERROR("TCI is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (len > MC_MAX_TCI_LEN) {
			MCDRV_DBG_ERROR("TCI length is longer than %d",
				MC_MAX_TCI_LEN);
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Get the device associated with the given session */
		struct mcore_device_t *device =
				resolveDeviceId(session->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t *devCon = device->connection;

		/* Get the physical address of the given TCI */
		struct wsm_t *pWsm =
			mcore_device_findContiguousWsm(device, tci);
		if (NULL == pWsm) {
			MCDRV_DBG_ERROR("Could not resolve TCI phy address ");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		if (pWsm->len < len) {
			MCDRV_DBG_ERROR("length is more than allocated TCI");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Prepare open session command */
		struct mcDrvCmdOpenSession_t cmdOpenSession = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_OPEN_SESSION
			},
			/* .payload = */ {
				/* .deviceId = */ session->deviceId,
				/* .uuid = */ *uuid,
				/* .tci = */ (uint32_t)pWsm->physAddr,
				/* .len = */ len
			}
		};

		/* Transmit command data */

		int len = connection_writeData(
						devCon,
						&cmdOpenSession,
						sizeof(cmdOpenSession));
		if (sizeof(cmdOpenSession) != len) {
			MCDRV_DBG_ERROR("CMD_OPEN_SESSION writeData failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		/* Read command response */

		/* read header first */
		struct mcDrvResponseHeader_t rspHeader;
		len = connection_readDataBlock(
					devCon,
					&rspHeader,
					sizeof(rspHeader));
		if (sizeof(rspHeader) != len) {
			MCDRV_DBG_ERROR("CMD_OPEN_SESSION readResp failed "
				" ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_OPEN_SESSION failed, respId=%d",
							rspHeader.responseId);
			switch (rspHeader.responseId) {
			case MC_DRV_RSP_TRUSTLET_NOT_FOUND:
				mcResult = MC_DRV_ERR_INVALID_DEVICE_FILE;
				break;
			case MC_DRV_RSP_PAYLOAD_LENGTH_ERROR:
			case MC_DRV_RSP_DEVICE_NOT_OPENED:
			case MC_DRV_RSP_FAILED:
			default:
				mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
				break;
			}
			break;
		}

		/* read payload */
		struct mcDrvRspOpenSessionPayload_t rspOpenSessionPayload;
		len = connection_readDataBlock(
					devCon,
					&rspOpenSessionPayload,
					sizeof(rspOpenSessionPayload));
		if (sizeof(rspOpenSessionPayload) != len) {
			MCDRV_DBG_ERROR("CMD_OPEN_SESSION readPayload failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		/* Register session with handle */
		session->sessionId = rspOpenSessionPayload.sessionId;

		/* Set up second channel for notifications */
		struct connection_t *sessionConnection = connection_new();
		/*TODO: no real need to connect here? */
		if (!connection_connect(sessionConnection, MC_DAEMON_PID)) {
			MCDRV_DBG_ERROR(
				"Could not setup netlink connection to PID %u",
				MC_DAEMON_PID);
			connection_cleanup(sessionConnection);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		/*TODO CONTINOUE HERE !!!! FIX RW RETURN HANDLING!!!! */

		/* Write command to use channel for notifications */
		struct mcDrvCmdNqConnect_t cmdNqConnect = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_NQ_CONNECT
			},
			/* .payload = */ {
				/* .deviceId =  */ session->deviceId,
				/* .sessionId = */ session->sessionId,
				/* .deviceSessionId = */
					rspOpenSessionPayload.deviceSessionId,
				/* .sessionMagic = */
					rspOpenSessionPayload.sessionMagic
			}
		};
		connection_writeData(sessionConnection,
			&cmdNqConnect,
			sizeof(cmdNqConnect));

		/* Read command response, header first */
		len = connection_readDataBlock(
					sessionConnection,
					&rspHeader,
					sizeof(rspHeader));
		if (sizeof(rspHeader) != len) {
			MCDRV_DBG_ERROR("CMD_NQ_CONNECT readRsp failed "
				"ret=%d", len);
			connection_cleanup(sessionConnection);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_NQ_CONNECT failed, respId=%d",
					rspHeader.responseId);
			connection_cleanup(sessionConnection);
			mcResult = MC_DRV_ERR_NQ_FAILED;
			break;
		}

		/* there is no payload. */

		/* Session established, new session object must be created */
		mcore_device_createNewSession(
			device,
			session->sessionId,
			sessionConnection);

	} while (false);

	/*pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcOpenSession);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcCloseSession(
	struct mcSessionHandle_t *session
) {
	enum mcResult_t mcResult = MC_DRV_OK;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {
		if (NULL == session) {
			MCDRV_DBG_ERROR("Session is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		struct mcore_device_t  *device =
					resolveDeviceId(session->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t  *devCon = device->connection;

		struct session_t  *nqSession =
		 mcore_device_resolveSessionId(device, session->sessionId);
		if (NULL == nqSession) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		/* Write close session command */
		struct mcDrvCmdCloseSession_t cmdCloseSession = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_CLOSE_SESSION
			},
			/* .payload = */ {
				/* .sessionId = */ session->sessionId,
			}
		};
		connection_writeData(
			devCon,
			&cmdCloseSession,
			sizeof(cmdCloseSession));

		/* Read command response */
		struct mcDrvResponseHeader_t rspHeader;
		int len = connection_readDataBlock(
						devCon,
						&rspHeader,
						sizeof(rspHeader));
		if (sizeof(rspHeader) != len) {
			MCDRV_DBG_ERROR("CMD_CLOSE_SESSION readRsp failed "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_CLOSE_SESSION failed, respId=%d",
							rspHeader.responseId);
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		mcore_device_removeSession(device, session->sessionId);
		mcResult = MC_DRV_OK;

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcCloseSession);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcNotify(
	struct mcSessionHandle_t   *session
) {
	enum mcResult_t mcResult = MC_DRV_OK;

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	do {
		if (NULL == session) {
			MCDRV_DBG_ERROR("Session is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		struct mcore_device_t *device =
					resolveDeviceId(session->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t *devCon = device->connection;

		struct session_t  *nqsession =
		 mcore_device_resolveSessionId(device, session->sessionId);
		if (NULL == nqsession) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		struct mcDrvCmdNotify_t cmdNotify = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_NOTIFY
			},
			/* .payload = */ {
				/* .sessionId = */ session->sessionId,
			}
		};

		connection_writeData(
			devCon,
			&cmdNotify,
			sizeof(cmdNotify));

		/* Daemon will not return a response */

	} while (false);

	return mcResult;
}
EXPORT_SYMBOL(mcNotify);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcWaitNotification(
	struct mcSessionHandle_t  *session,
	int32_t			timeout
) {
	enum mcResult_t mcResult = MC_DRV_OK;

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	do {
		if (NULL == session) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		struct mcore_device_t  *device =
					resolveDeviceId(session->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		struct session_t  *nqSession =
		 mcore_device_resolveSessionId(device, session->sessionId);
		if (NULL == nqSession) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		struct connection_t *nqconnection =
					nqSession->notificationConnection;
		uint32_t count = 0;

		/* Read notification queue till it's empty */
		for (;;) {
			struct notification_t notification;
			ssize_t numRead = connection_readData(
				nqconnection,
				&notification,
				sizeof(notification),
				timeout);
			/* Exit on timeout in first run. Later runs have
			 * timeout set to 0.
			 * -2 means, there is no more data. */
			if (0 == count && -2 == numRead) {
				MCDRV_DBG_ERROR("read timeout");
				mcResult = MC_DRV_ERR_TIMEOUT;
				break;
			}
			/* After first notification the queue will be
			 * drained, Thus we set no timeout for the
			 * following reads */
			timeout = 0;

			if (numRead != sizeof(struct notification_t)) {
				if (0 == count) {
					/* failure in first read, notify it */
					mcResult = MC_DRV_ERR_NOTIFICATION;
					MCDRV_DBG_ERROR(
					"read notification failed, "
					"%i bytes received", (int)numRead);
					break;
				} else {
					/* Read of the n-th notification
					   failed/timeout. We don't tell the
					   caller, as we got valid notifications
					   before. */
					mcResult = MC_DRV_OK;
					break;
				}
			}

			count++;
			MCDRV_DBG_VERBOSE("readNq count=%d, SessionID=%d, "
				"Payload=%d", count,
				notification.sessionId, notification.payload);

			if (0 != notification.payload) {
				/* Session end point died -> store exit code */
				session_setErrorInfo(nqSession,
					notification.payload);

				mcResult = MC_DRV_INFO_NOTIFICATION;
				break;
			}
		} /* for(;;) */

	} while (false);

	return mcResult;
}
EXPORT_SYMBOL(mcWaitNotification);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcMallocWsm(
	uint32_t	deviceId,
	uint32_t	align,
	uint32_t	len,
	uint8_t	 **wsm,
	uint32_t	wsmFlags
) {
	enum mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); */

	do {
		struct mcore_device_t *device = resolveDeviceId(deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		if (NULL == wsm) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		struct wsm_t *pWsm =
			mcore_device_allocateContiguousWsm(device, len);
		if (NULL == pWsm) {
			MCDRV_DBG_ERROR("Allocation of WSM failed");
			mcResult = MC_DRV_ERR_NO_FREE_MEMORY;
			break;
		}

		*wsm = (uint8_t *)pWsm->virtAddr;
		mcResult = MC_DRV_OK;

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcMallocWsm);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcFreeWsm(
	uint32_t	deviceId,
	uint8_t	 *wsm
) {
	enum mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
	struct mcore_device_t *device;

	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {

		/* Get the device associated wit the given session */
		device = resolveDeviceId(deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		/* find WSM object */
		struct wsm_t *pWsm =
			mcore_device_findContiguousWsm(device, wsm);
		if (NULL == pWsm) {
			MCDRV_DBG_ERROR("unknown address");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Free the given virtual address */
		if (!mcore_device_freeContiguousWsm(device, pWsm)) {
			MCDRV_DBG_ERROR("Free of virtual address failed");
			mcResult = MC_DRV_ERR_FREE_MEMORY_FAILED;
			break;
		}
		mcResult = MC_DRV_OK;

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcFreeWsm);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcMap(
	struct mcSessionHandle_t  *sessionHandle,
	void			   *buf,
	uint32_t		   bufLen,
	struct mcBulkMap_t		*mapInfo
) {
	enum mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {
		if (NULL == sessionHandle) {
			MCDRV_DBG_ERROR("sessionHandle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == mapInfo) {
			MCDRV_DBG_ERROR("mapInfo is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == buf) {
			MCDRV_DBG_ERROR("buf is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Determine device the session belongs to */
		struct mcore_device_t  *device = resolveDeviceId(
						sessionHandle->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t *devCon = device->connection;

		/* Get session */
		struct session_t  *session =
		mcore_device_resolveSessionId(device, sessionHandle->sessionId);
		if (NULL == session) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		/* Register mapped bulk buffer to Kernel Module and keep mapped
		   bulk buffer in mind */
		struct bulkBufferDescriptor_t *bulkBuf = session_addBulkBuf(
			session, buf, bufLen);
		if (NULL == bulkBuf) {
			MCDRV_DBG_ERROR("Error mapping bulk buffer");
			mcResult = MC_DRV_ERR_BULK_MAPPING;
			break;
		}

		/* Prepare map command */
		struct mcDrvCmdMapBulkMem_t mcDrvCmdMapBulkMem = {
			/* C++ does not support C99 designated initializers */
			/* .header = */ {
				/* .commandId = */ MC_DRV_CMD_MAP_BULK_BUF
			},
			/* .payload = */ {
				/* .sessionId = */ session->sessionId,
				/* .pAddrL2 = */
					(uint32_t)bulkBuf->physAddrWsmL2,
				/* .offsetPayload = */
					(uint32_t)(bulkBuf->virtAddr) & 0xFFF,
				/* .lenBulkMem = */ bulkBuf->len
			}
		};

		/* Transmit map command to MobiCore device */
		connection_writeData(
			devCon,
			&mcDrvCmdMapBulkMem,
			sizeof(mcDrvCmdMapBulkMem));

		/* Read command response */
		struct mcDrvResponseHeader_t rspHeader;
		int len = connection_readDataBlock(
						devCon,
						&rspHeader,
						sizeof(rspHeader));
		if (sizeof(rspHeader) != len) {
			MCDRV_DBG_ERROR("CMD_MAP_BULK_BUF readRsp failed, "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_MAP_BULK_BUF failed, respId=%d",
							rspHeader.responseId);
			/* REV We ignore Daemon Error code because client cannot
			   handle it anyhow. */
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;

			/* Unregister mapped bulk buffer from Kernel Module and
			   remove mapped bulk buffer from session maintenance */
			if (!session_removeBulkBuf(session, buf)) {
				/* Removing of bulk buffer not possible */
				MCDRV_DBG_ERROR("Unregistering of bulk memory"
					"from Kernel Module failed");
			}
			break;
		}

		struct mcDrvRspMapBulkMemPayload_t rspMapBulkMemPayload;
		connection_readDataBlock(
			devCon,
			&rspMapBulkMemPayload,
			sizeof(rspMapBulkMemPayload));

		/* Set mapping info for Trustlet */
		mapInfo->sVirtualAddr =
			(void *)(rspMapBulkMemPayload.secureVirtualAdr);
		mapInfo->sVirtualLen = bufLen;
		mcResult = MC_DRV_OK;

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcMap);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcUnmap(
	struct mcSessionHandle_t  *sessionHandle,
	void			   *buf,
	struct mcBulkMap_t		*mapInfo
) {
	enum mcResult_t mcResult = MC_DRV_ERR_UNKNOWN;
	/* static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	/* pthread_mutex_lock(&mutex); // Enter critical section */

	do {
		if (NULL == sessionHandle) {
			MCDRV_DBG_ERROR("sessionHandle is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == mapInfo) {
			MCDRV_DBG_ERROR("mapInfo is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}
		if (NULL == buf) {
			MCDRV_DBG_ERROR("buf is null");
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Determine device the session belongs to */
		struct mcore_device_t  *device =
			resolveDeviceId(sessionHandle->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}
		struct connection_t  *devCon = device->connection;

		/* Get session */
		struct session_t  *session =
		mcore_device_resolveSessionId(device, sessionHandle->sessionId);
		if (NULL == session) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		/* Prepare unmap command */
		struct mcDrvCmdUnmapBulkMem_t cmdUnmapBulkMem = {
				/* .header = */ {
					/* .commandId = */
						MC_DRV_CMD_UNMAP_BULK_BUF
				},
				/* .payload = */ {
					/* .sessionId = */ session->sessionId,
					/* .secureVirtualAdr = */
					(uint32_t)(mapInfo->sVirtualAddr),
					/* .lenBulkMem = mapInfo->sVirtualLen*/
				}
			};

		connection_writeData(
			devCon,
			&cmdUnmapBulkMem,
			sizeof(cmdUnmapBulkMem));

		/* Read command response */
		struct mcDrvResponseHeader_t rspHeader;
		int len = connection_readDataBlock(
						devCon,
						&rspHeader,
						sizeof(rspHeader));
		if (sizeof(rspHeader) != len) {
			MCDRV_DBG_ERROR("CMD_UNMAP_BULK_BUF readRsp failed, "
				"ret=%d", len);
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		if (MC_DRV_RSP_OK != rspHeader.responseId) {
			MCDRV_DBG_ERROR("CMD_UNMAP_BULK_BUF failed, respId=%d",
							rspHeader.responseId);
			/* REV We ignore Daemon Error code because client
			   cannot handle it anyhow. */
			mcResult = MC_DRV_ERR_DAEMON_UNREACHABLE;
			break;
		}

		struct mcDrvRspUnmapBulkMemPayload_t rspUnmapBulkMemPayload;
		connection_readDataBlock(
			devCon,
			&rspUnmapBulkMemPayload,
			sizeof(rspUnmapBulkMemPayload));

		/* REV axh: what about check the payload? */

		/* Unregister mapped bulk buffer from Kernel Module and
		 * remove mapped bulk buffer from session maintenance */
		if (!session_removeBulkBuf(session, buf)) {
			/* Removing of bulk buffer not possible */
			MCDRV_DBG_ERROR("Unregistering of bulk memory from "
							"Kernel Module failed");
			mcResult = MC_DRV_ERR_BULK_UNMAPPING;
			break;
		}

		mcResult = MC_DRV_OK;

	} while (false);

	/* pthread_mutex_unlock(&mutex); // Exit critical section */

	return mcResult;
}
EXPORT_SYMBOL(mcUnmap);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcGetSessionErrorCode(
	struct mcSessionHandle_t   *session,
	int32_t			 *lastErr
) {
	enum mcResult_t mcResult = MC_DRV_OK;

	MCDRV_DBG_VERBOSE("===%s()===", __func__);

	do {
		if (NULL == session || NULL == lastErr) {
			mcResult = MC_DRV_ERR_INVALID_PARAMETER;
			break;
		}

		/* Get device */
		struct mcore_device_t *device =
					resolveDeviceId(session->deviceId);
		if (NULL == device) {
			MCDRV_DBG_ERROR("Device not found");
			mcResult = MC_DRV_ERR_UNKNOWN_DEVICE;
			break;
		}

		/* Get session */
		struct session_t *nqsession =
		 mcore_device_resolveSessionId(device, session->sessionId);
		if (NULL == nqsession) {
			MCDRV_DBG_ERROR("Session not found");
			mcResult = MC_DRV_ERR_UNKNOWN_SESSION;
			break;
		}

		/* get session error code from session */
		*lastErr = session_getLastErr(nqsession);

	} while (false);

	return mcResult;
}
EXPORT_SYMBOL(mcGetSessionErrorCode);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcDriverCtrl(
	enum mcDriverCtrl_t  param,
	uint8_t		 *data,
	uint32_t		len
) {
	MCDRV_DBG_WARN("not implemented");
	return MC_DRV_ERR_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(mcDriverCtrl);

/*----------------------------------------------------------------------------*/
enum mcResult_t mcManage(
	uint32_t  deviceId,
	uint8_t   *data,
	uint32_t  len
) {
	MCDRV_DBG_WARN("not implemented");
	return MC_DRV_ERR_NOT_IMPLEMENTED;
}
EXPORT_SYMBOL(mcManage);

