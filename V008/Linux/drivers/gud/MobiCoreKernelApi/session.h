/** @addtogroup MCD_IMPL_LIB
 * @{
 * @file
 * <!-- Copyright Giesecke & Devrient GmbH 2009 - 2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SESSION_H_
#define SESSION_H_

#include "common.h"

#include <linux/list.h>
#include "connection.h"


struct bulkBufferDescriptor_t {
	void		*virtAddr; /**< The virtual address of the Bulk buffer*/
	uint32_t	len;	  /**< Length of the Bulk buffer*/
	uint32_t	handle;
	void		*physAddrWsmL2; /**< The physical address of the
				L2 table of the Bulk buffer*/
	struct list_head list; /**< The list param for using the kernel lists*/
};

struct bulkBufferDescriptor_t *bulkBufferDescriptor_create(
	void	*virtAddr,
	uint32_t  len,
	uint32_t  handle,
	void	*physAddrWsmL2
);

/** Session states.
 * At the moment not used !!.
 */
enum sessionState_t {
	SESSION_STATE_INITIAL,
	SESSION_STATE_OPEN,
	SESSION_STATE_TRUSTLET_DEAD
};

#define SESSION_ERR_NO	  0 /**< No session error */

/** Session information structure.
 * The information structure is used to hold the state of the session, which
 * will limit further actions for the session.
 * Also the last error code will be stored till it's read.
 */
struct sessionInformation_t {
	enum sessionState_t state;	   /**< Session state */
	int32_t		lastErr;	 /**< Last error of session */
};


struct session_t {
	struct mcInstance		*pInstance;
	/**< Descriptors of additional bulk buffer of a session */
	struct list_head	bulkBufferDescriptors;
	/**< Informations about session */
	struct sessionInformation_t	 sessionInfo;

	uint32_t		 sessionId;
	struct connection_t	 *notificationConnection;

	/**< The list param for using the kernel lists*/
	struct list_head list;
};

struct session_t *session_create(
	uint32_t	 sessionId,
	void		 *pInstance,
	struct connection_t *connection
);

void session_cleanup(
	struct session_t *session
);

/**
  * Add address information of additional bulk buffer memory to session and
  * register virtual memory in kernel module.
  *
  * @attention The virtual address can only be added one time. If the virtual
  * address already exist, NULL is returned.
  *
  * @param buf The virtual address of bulk buffer.
  * @param len Length of bulk buffer.
  *
  * @return On success the actual Bulk buffer descriptor with all address
  * information is retured, NULL if an error occurs.
  */
struct bulkBufferDescriptor_t *session_addBulkBuf(
	struct session_t *session,
	void	*buf,
	uint32_t  len
);

/**
  * Remove address information of additional bulk buffer memory from session and
  * unregister virtual memory in kernel module
  *
  * @param buf The virtual address of the bulk buffer.
  *
  * @return true on success.
  */
bool session_removeBulkBuf(
	struct session_t *session,
	void  *buf
);

/**
  * Set additional error information of the last error that occured.
  *
  * @param errorCode The actual error.
  */
void session_setErrorInfo(
	struct session_t *session,
	int32_t err
);

/**
  * Get additional error information of the last error that occured.
  *
  * @attention After request the information is set to SESSION_ERR_NO.
  *
  * @return Last stored error code or SESSION_ERR_NO.
  */
int32_t session_getLastErr(
	struct session_t *session
);

#endif /* SESSION_H_ */

/** @} */
