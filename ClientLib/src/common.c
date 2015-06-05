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

#define _GNU_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#include "mcVersionHelper.h"
#include "mc_linux.h"

MC_CHECK_VERSION(MCDRVMODULEAPI, 2, 0);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int open_count;
static int fd = -1;

int common_device_open(void)
{
    /* Already open */
    pthread_mutex_lock(&mutex);
    if (open_count > 0) {
        goto done;
    }

    fd = open("/dev/" MC_USER_DEVNODE, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        goto error;
    }

    uint32_t version;
    int ret = ioctl(fd, MC_IO_DR_VERSION, &version);
    if (ret < 0) {
        goto error_file_open;
    }

    // Run-time check.
    char *errmsg;
    if (!checkVersionOkMCDRVMODULEAPI(version, &errmsg)) {
        errno = EHOSTDOWN;
        goto error_file_open;
    }

    /* All OK */
    goto done;

error_file_open:
    close(fd);
    fd = -1;
    goto error;
done:
    open_count++;
error:
    pthread_mutex_unlock(&mutex);
    return fd;
}

int common_device_silentclose(void)
{
    pthread_mutex_lock(&mutex);
    int ret = open_count > 1 ? 0 : -1;
    if (ret == 0) {
        open_count--;
    } else {
        errno = EPERM;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

int common_device_close(void)
{
    int ret = -1;

    pthread_mutex_lock(&mutex);
    /* Not open */
    if (open_count == 0) {
        errno = EBADF;
        goto error;
    }

    /* Last token */
    if (open_count == 1) {
        ret = close(fd);
        fd = -1;
    } else {
        ret = 0;
    }

    open_count--;
error:
    pthread_mutex_unlock(&mutex);
    return ret;
}

int common_device_fd(void)
{
    return fd;
}
