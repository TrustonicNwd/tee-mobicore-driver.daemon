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

/* Common open/file device functions for Mobicore and GP */

#ifndef __MC_COMMON_H__
#define __MC_COMMON_H__

/** Open t-base device, includes version check
 *
 * @return 0 on success, -1 on error, with errno appropriately set
 */
int common_device_open(void);

/** Try to silently close t-base device (if it would remain open)
 *
 * As it is possible to call common_device_open() several times,
 * common_device_close() may actually do nothing else than decrement its usage
 * counter.
 *
 * This function tries to just decrement the usage counter.
 *
 * @return 0 if usage counter was more than 1, -1 otherwise
 */
int common_device_silentclose(void);

/** Close t-base device
 *
 * @return 0 on success, -1 on error, with errno appropriately set
 */
int common_device_close(void);

/** Get t-base device file descriptor
 *
 * @return fd if device is open, -1 otherwise
 */
int common_device_fd(void);

#endif /*  __MC_COMMON_H__ */
