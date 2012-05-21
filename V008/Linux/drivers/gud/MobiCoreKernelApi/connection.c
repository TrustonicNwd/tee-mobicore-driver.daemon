/** @addtogroup MCD_MCDIMPL_DAEMON_SRV
 * @{
 * @file
 *
 * Connection data.
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009 - 2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/semaphore.h>
#include <linux/time.h>
#include <net/sock.h>
#include <net/net_namespace.h>

#include "connection.h"
#include "common.h"

/* Define the initial state of the Data Available Semaphore */
#define SEM_NO_DATA_AVAILABLE 0

/*----------------------------------------------------------------------------*/
struct connection_t *connection_new(
	void
) {
	struct connection_t *conn = kzalloc(sizeof(struct connection_t),
					GFP_KERNEL);
	conn->sequenceMagic = mcapi_unique_id();
	mutex_init(&conn->dataLock);
	/* No data available */
	sema_init(&conn->dataAvailableSem, SEM_NO_DATA_AVAILABLE);

	mcapi_insert_connection(conn);
	return conn;
}

/*----------------------------------------------------------------------------*/
struct connection_t *connection_create(
	int	 socketDescriptor,
	pid_t   dest
) {
	struct connection_t *conn = connection_new();

	conn->peerPid = dest;
	return conn;
}


/*----------------------------------------------------------------------------*/
void connection_cleanup(
	struct connection_t *conn
) {
	if (!conn)
		return;

	kfree_skb(conn->skb);

	mcapi_remove_connection(conn->sequenceMagic);
	kfree(conn);
}


/*----------------------------------------------------------------------------*/
bool connection_connect(
	struct connection_t *conn,
	pid_t		dest
) {
	/* Nothing to connect */
	conn->peerPid = dest;
	return true;
}

/*----------------------------------------------------------------------------*/
size_t connection_readDataMsg(
	struct connection_t *conn,
	void *buffer,
	uint32_t len
) {
	size_t ret = -1;
	MCDRV_DBG_VERBOSE("reading connection data %u, connection data left %u",
			len, conn->dataLen);
	/* trying to read more than the left data */
	if (len > conn->dataLen) {
		ret = conn->dataLen;
		memcpy(buffer, conn->dataStart, conn->dataLen);
		conn->dataLen = 0;
	} else {
		ret = len;
		memcpy(buffer, conn->dataStart, len);
		conn->dataLen -= len;
		conn->dataStart += len;
	}

	if (conn->dataLen == 0)	{
		conn->dataStart = NULL;
		kfree_skb(conn->skb);
		conn->skb = NULL;
	}
	MCDRV_DBG_VERBOSE("read %u",  ret);
	return ret;
}

/*----------------------------------------------------------------------------*/
size_t connection_readDataBlock(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	 len
) {
	return connection_readData(conn, buffer, len, -1);
}


/*----------------------------------------------------------------------------*/
size_t connection_readData(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	 len,
	int32_t	  timeout
) {
	size_t ret = 0;

	MCDRV_ASSERT(NULL != buffer);
	MCDRV_ASSERT(NULL != conn->socketDescriptor);

	MCDRV_DBG_VERBOSE("read data len = %u for PID = %u",
						len, conn->sequenceMagic);
	do {
		/* Wait until data is available or timeout
		   msecs_to_jiffies(-1) -> wait forever for the sem */
		if (down_timeout(&(conn->dataAvailableSem),
				  msecs_to_jiffies(timeout))) {
			MCDRV_DBG_VERBOSE("Timeout reading the data sem");
			ret = -2;
			break;
		}

		if (mutex_lock_interruptible(&(conn->dataLock))) {
			MCDRV_DBG_ERROR("interrupted reading the data sem");
			ret = -1;
			break;
		}
		/* Have data, use it */
		if (conn->dataLen > 0)
			ret = connection_readDataMsg(conn, buffer, len);

			mutex_unlock(&(conn->dataLock));

		/* There is still some data left */
		if (conn->dataLen > 0)
			up(&conn->dataAvailableSem);
	} while (0);

	return ret;
}

/*----------------------------------------------------------------------------*/
size_t connection_writeData(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	 len
) {
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh;
	int ret = 0;

	MCDRV_DBG_VERBOSE("buffer length %u from pid %u\n",
		  len,  conn->sequenceMagic);
	do {
		skb = nlmsg_new(NLMSG_SPACE(len), GFP_KERNEL);
		if (!skb) {
			ret = -1;
			break;
		}

		nlh = nlmsg_put(skb, 0, conn->sequenceMagic, 2,
					  NLMSG_LENGTH(len), NLM_F_REQUEST);
		if (!nlh) {
			ret = -1;
			break;
		}
		memcpy(NLMSG_DATA(nlh), buffer, len);

		netlink_unicast(conn->socketDescriptor, skb,
						conn->peerPid, MSG_DONTWAIT);
		ret = len;
	} while (0);

	if (!ret && skb != NULL)
		kfree_skb(skb);

	return ret;
}

int connection_process(
	struct connection_t *conn,
	struct sk_buff *skb
)
{
	int ret = 0;
	do {
		if (mutex_lock_interruptible(&(conn->dataLock))) {
			MCDRV_DBG_ERROR("Interrupted getting data semaphore!");
			ret = -1;
			break;
		}

		kfree_skb(conn->skb);

		/* Get a reference to the incomming skb */
		conn->skb = skb_get(skb);
		if (conn->skb) {
			conn->dataMsg = nlmsg_hdr(conn->skb);
			conn->dataLen = NLMSG_PAYLOAD(conn->dataMsg, 0);
			conn->dataStart = NLMSG_DATA(conn->dataMsg);
			up(&(conn->dataAvailableSem));
		}
		mutex_unlock(&(conn->dataLock));
		ret = 0;
	} while (0);
	return ret;
}
/** @} */
