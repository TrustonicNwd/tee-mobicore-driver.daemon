/** @addtogroup MCD_MCDIMPL_DAEMON
 * @{
 * @file
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009 - 2012 -->
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *	products derived from this software without specific prior
 *	written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MCDAEMON_H_
#define MCDAEMON_H_




#include "mcUuid.h"

enum mcDrvCmd_t {
	MC_DRV_CMD_PING			= 0,
	MC_DRV_CMD_GET_INFO		= 1,
	MC_DRV_CMD_OPEN_DEVICE	 = 2,
	MC_DRV_CMD_CLOSE_DEVICE	= 3,
	MC_DRV_CMD_NQ_CONNECT	  = 4,
	MC_DRV_CMD_OPEN_SESSION	= 5,
	MC_DRV_CMD_CLOSE_SESSION   = 6,
	MC_DRV_CMD_NOTIFY		  = 7,
	MC_DRV_CMD_MAP_BULK_BUF	= 8,
	MC_DRV_CMD_UNMAP_BULK_BUF  = 9
};


enum mcDrvRsp_t {
	MC_DRV_RSP_OK					 = 0,
	MC_DRV_RSP_FAILED				 = 1,
	MC_DRV_RSP_DEVICE_NOT_OPENED	  = 2,
	MC_DRV_RSP_DEVICE_ALREADY_OPENED  = 3,
	MC_DRV_RSP_COMMAND_NOT_ALLOWED	= 4,
	MC_DRV_INVALID_DEVICE_NAME		= 5,
	MC_DRV_RSP_MAP_BULK_ERRO		  = 6,
	MC_DRV_RSP_TRUSTLET_NOT_FOUND	 = 7,
	MC_DRV_RSP_PAYLOAD_LENGTH_ERROR	  = 8,
};


struct mcDrvCommandHeader_t {
	uint32_t  commandId;
};

struct mcDrvResponseHeader_t {
	uint32_t  responseId;
};

#define MC_DEVICE_ID_DEFAULT	0 /**< The default device ID */


/*****************************************************************************/
struct mcDrvCmdOpenDevicePayload_t {
	uint32_t  deviceId;
};

struct mcDrvCmdOpenDevice_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdOpenDevicePayload_t payload;
};


struct mcDrvRspOpenDevicePayload_t {
	/* empty */
};

struct mcDrvRspOpenDevice_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspOpenDevicePayload_t payload;
};


/*****************************************************************************/
struct mcDrvCmdCloseDevice_t {
	struct mcDrvCommandHeader_t header;
	/* no payload here because close has none.
	   If we use an empty struct, C++ will count it as 4 bytes.
	   This will write too much into the socket at write(cmd,sizeof(cmd)) */
};


struct mcDrvRspCloseDevicePayload_t {
	/* empty */
};

struct mcDrvRspCloseDevice_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspCloseDevicePayload_t payload;
};


/*****************************************************************************/
struct mcDrvCmdOpenSessionPayload_t {
	uint32_t deviceId;
	struct mcUuid_t uuid;
	uint32_t tci;
	uint32_t len;
};

struct mcDrvCmdOpenSession_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdOpenSessionPayload_t payload;
};


struct mcDrvRspOpenSessionPayload_t {
	uint32_t deviceId;
	uint32_t sessionId;
	uint32_t deviceSessionId;
	uint32_t mcResult;
	uint32_t sessionMagic;
};

struct mcDrvRspOpenSession_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspOpenSessionPayload_t  payload;
};


/*****************************************************************************/
struct mcDrvCmdCloseSessionPayload_t {
	uint32_t  sessionId;
};

struct mcDrvCmdCloseSession_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdCloseSessionPayload_t payload;
};


struct mcDrvRspCloseSessionPayload_t {
	/* empty */
};

struct mcDrvRspCloseSession_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspCloseSessionPayload_t payload;
};


/*****************************************************************************/
struct mcDrvCmdNotifyPayload_t {
	uint32_t sessionId;
};

struct mcDrvCmdNotify_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdNotifyPayload_t payload;
};


struct mcDrvRspNotifyPayload_t {
	/* empty */
};

struct mcDrvRspNotify_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspNotifyPayload_t  payload;
};


/*****************************************************************************/
struct mcDrvCmdMapBulkMemPayload_t {
	uint32_t sessionId;
	uint32_t pAddrL2;
	uint32_t offsetPayload;
	uint32_t lenBulkMem;
};

struct mcDrvCmdMapBulkMem_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdMapBulkMemPayload_t  payload;
};


struct mcDrvRspMapBulkMemPayload_t {
	uint32_t sessionId;
	uint32_t secureVirtualAdr;
	uint32_t mcResult;
};

struct mcDrvRspMapBulkMem_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspMapBulkMemPayload_t  payload;
};


/*****************************************************************************/
struct mcDrvCmdUnmapBulkMemPayload_t {
	uint32_t sessionId;
	uint32_t secureVirtualAdr;
	uint32_t lenBulkMem;
};

struct mcDrvCmdUnmapBulkMem_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdUnmapBulkMemPayload_t payload;
};


struct mcDrvRspUnmapBulkMemPayload_t {
	uint32_t responseId;
	uint32_t sessionId;
	uint32_t mcResult;
};

struct mcDrvRspUnmapBulkMem_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspUnmapBulkMemPayload_t payload;
};


/*****************************************************************************/
struct mcDrvCmdNqConnectPayload_t {
	uint32_t deviceId;
	uint32_t sessionId;
	uint32_t deviceSessionId;
	uint32_t sessionMagic; /* Random data */
};

struct mcDrvCmdNqConnect_t {
	struct mcDrvCommandHeader_t header;
	struct mcDrvCmdNqConnectPayload_t  payload;
};


struct mcDrvRspNqConnectPayload_t {
	/* empty; */
};

struct mcDrvRspNqConnect_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspNqConnectPayload_t payload;
};


/*****************************************************************************/
union mcDrvCommand_t {
	struct mcDrvCommandHeader_t	header;
	struct mcDrvCmdOpenDevice_t	mcDrvCmdOpenDevice;
	struct mcDrvCmdCloseDevice_t   mcDrvCmdCloseDevice;
	struct mcDrvCmdOpenSession_t   mcDrvCmdOpenSession;
	struct mcDrvCmdCloseSession_t  mcDrvCmdCloseSession;
	struct mcDrvCmdNqConnect_t	 mcDrvCmdNqConnect;
	struct mcDrvCmdNotify_t		mcDrvCmdNotify;
	struct mcDrvCmdMapBulkMem_t	mcDrvCmdMapBulkMem;
	struct mcDrvCmdUnmapBulkMem_t  mcDrvCmdUnmapBulkMem;
};

union mcDrvResponse_t {
	struct mcDrvResponseHeader_t header;
	struct mcDrvRspOpenDevice_t	mcDrvRspOpenDevice;
	struct mcDrvRspCloseDevice_t   mcDrvRspCloseDevice;
	struct mcDrvRspOpenSession_t   mcDrvRspOpenSession;
	struct mcDrvRspCloseSession_t  mcDrvRspCloseSession;
	struct mcDrvRspNqConnect_t	 mcDrvRspNqConnect;
	struct mcDrvRspNotify_t		mcDrvRspNotify;
	struct mcDrvRspMapBulkMem_t	mcDrvRspMapBulkMem;
	struct mcDrvRspUnmapBulkMem_t  mcDrvRspUnmapBulkMem;
};

#endif /* MCDAEMON_H_ */

/** @} */
