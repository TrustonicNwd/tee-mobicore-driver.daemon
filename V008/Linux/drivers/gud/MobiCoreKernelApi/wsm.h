/** @addtogroup MCD_MCDIMPL_DAEMON_SRV
 * @{
 * @file
 *
 * World shared memory definitions.
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009 - 2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef WSM_H_
#define WSM_H_

#include "common.h"
#include <linux/list.h>

struct wsm_t {
	void *virtAddr;
	uint32_t len;
	uint32_t handle;
	void *physAddr;
	struct list_head list;
};

struct wsm_t *wsm_create(
	void	*virtAddr,
	uint32_t  len,
	uint32_t  handle,
	void	*physAddr /*= NULL this may be unknown, so is can be omitted.*/
);
#endif /* WSM_H_ */

/** @} */
