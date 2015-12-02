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

/**
 * Filesystem v2 delegate.
 *
 * Handles incoming storage requests from TA through STH
 */

#include <unistd.h>
#include <string>
#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <memory>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/stat.h>

#undef LOG_TAG
#define LOG_TAG "TeeFilesystem"
#include <log.h>
#include "tee_client_api.h"     /* TEEC_Result */
#include "MobiCoreDriverApi.h"  /* MC session */
#include "sth2ProxyApi.h"
#include "sfs_error.h"
#include "FSD2.h"

#define MAX_SECTOR_SIZE                 4096
#define SECTOR_NUM                      8
#define SFS_L2_CACHE_SLOT_SPACE         12  // Hard coded size, cf. sfs_internal.h
#define DEFAULT_WORKSPACE_SIZE          (SECTOR_NUM * (MAX_SECTOR_SIZE + SFS_L2_CACHE_SLOT_SPACE))

extern const std::string& getTbStoragePath();

/*----------------------------------------------------------------------------
 * Utilities functions
 *----------------------------------------------------------------------------*/
static TEEC_Result errno2serror() {
    switch (errno) {
        case EINVAL:
            return S_ERROR_BAD_PARAMETERS;
        case EMFILE:
            return S_ERROR_NO_MORE_HANDLES;
        case ENOENT:
            return S_ERROR_ITEM_NOT_FOUND;
        case EEXIST:
            return S_ERROR_ITEM_EXISTS;
        case ENOSPC:
            return S_ERROR_STORAGE_NO_SPACE;
        case ENOMEM:
            return S_ERROR_OUT_OF_MEMORY;
        case EBADF:
        case EACCES:
        default:
            return S_ERROR_STORAGE_UNREACHABLE;
    }
}

class Partition {
    std::string name_;
    FILE* fd_;
    off_t size_;
public:
    Partition(std::string name): name_(name), fd_(NULL), size_(0) {}
    const char* name() const {
        return name_.c_str();
    }
    off_t size() {
        if (size_ == 0) {
            struct stat st;
            if (::stat(name(), &st)) {
                return -1;
            }
            size_ = st.st_size;
        }
        return size_;
    }
    TEEC_Result create() {
        LOG_I("%s: Create storage file \"%s\"", __func__, name());
        fd_ = ::fopen(name(), "w+b");
        if (!fd_) {
            LOG_E("%s: %s creating storage file \"%s\"", __func__,
                  strerror(errno), name());
            return errno2serror();
        }
        return S_SUCCESS;
    }
    TEEC_Result destroy() {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        /* Try to erase the file */
        if (::unlink(name())) {
            /* File in use or OS didn't allow the operation */
            return errno2serror();
        }

        return S_SUCCESS;
    }
    TEEC_Result open() {
        /* Open the file */
        LOG_I("%s: Open storage file \"%s\"", __func__, name());
        fd_ = ::fopen(name(), "r+b");
        if (!fd_) {
            LOG_E("%s: %s opening storage file \"%s\"", __func__,
                  strerror(errno), name());
            return errno2serror();
        }
        LOG_I("%s: storage file \"%s\" successfully open (size: %ld KB / %ld B))",
              __func__, name(), size() / 1024, size());
        return S_SUCCESS;
    }
    TEEC_Result close() {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        ::fclose(fd_);
        fd_ = NULL;

        return S_SUCCESS;
    }
    TEEC_Result read(uint8_t* buf, uint32_t length, uint32_t offset) {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        if (::fseek(fd_, offset, SEEK_SET)) {
            LOG_E("%s: fseek error: %s", __func__, strerror(errno));
            return errno2serror();
        }

        if (::fread(buf, length, 1, fd_) != 1) {
            if (feof(fd_)) {
                LOG_E("%s: fread error: End-Of-File detected", __func__);
                return S_ERROR_ITEM_NOT_FOUND;
            }
            LOG_E("%s: fread error: %s", __func__, strerror(errno));
            return errno2serror();
        }

        return S_SUCCESS;
    }
    TEEC_Result write(const uint8_t* buf, uint32_t length, uint32_t offset) {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        if (::fseek(fd_, offset, SEEK_SET)) {
            LOG_E("%s: fseek error: %s", __func__, strerror(errno));
            return errno2serror();
        }

        if (::fwrite(buf, length, 1, fd_) != 1) {
            LOG_E("%s: fwrite error: %s", __func__, strerror(errno));
            return errno2serror();
        }
        return S_SUCCESS;
    }
    TEEC_Result sync() {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        /*
         * First make sure that the data in the stdio buffers is flushed to the
         * file descriptor
         */
        if (::fflush(fd_)) {
            return errno2serror();
        }

        /* Then synchronize the file descriptor with the file-system */
        if (::fdatasync(fileno(fd_))) {
            return errno2serror();
        }

        return S_SUCCESS;
    }
    TEEC_Result resize(off_t new_size) {
        if (!fd_) {
            /* The partition is not open */
            return S_ERROR_BAD_STATE;
        }

        if (new_size == size()) {
            return S_SUCCESS;
        }

        if (new_size > size()) {
            /*
             * Enlarge the partition file. Make sure we actually write some
             * non-zero data into the new sectors. Otherwise, some file-system
             * might not really reserve the storage space but use a sparse
             * representation. In this case, a subsequent write instruction
             * could fail due to out-of-space, which we want to avoid.
             */
            if (::fseek(fd_, 0, SEEK_END)) {
                LOG_E("%s: fseek error: %s", __func__, strerror(errno));
                return errno2serror();
            }

            off_t count = new_size - size();
            while (count) {
                if (::fputc(0xA5, fd_) != 0xA5) {
                    LOG_E("%s: fputc error: %s", __func__, strerror(errno));
                    return errno2serror();
                }
                count--;
            }
        } else {
            /* Truncate the partition file */
            if (::ftruncate(fileno(fd_), new_size)) {
                return errno2serror();
            }
        }
        // Update size
        size_ = new_size;
        return S_SUCCESS;
    }
};

struct FileSystem::Impl {
    pthread_t                   thread;

    /*
     * Communication buffer, includes the workspace
     */
    struct {
        STH2_delegation_exchange_buffer_t exchange_buffer;
        uint8_t workspace[DEFAULT_WORKSPACE_SIZE];
    } dci;


    /*
     * Provided by the SWd
     */
    uint32_t sector_size;

    /*
     * The 16 possible partitions
     */
    Partition* partitions[16];

    Impl() {
        sector_size = 0;
        for (int i = 0; i < 16; i++) {
            partitions[i] = NULL;
        }
    }
    void run();
    int executeCommand();
    const char* getCommandtypeString(uint32_t nInstructionID);
};

FileSystem::FileSystem(): pimpl_(new Impl) {}

FileSystem::~FileSystem() {
    delete pimpl_;
}

void* FileSystem::run(void* arg) {
    FileSystem::Impl* pimpl = static_cast<FileSystem::Impl*>(arg);
    pimpl->run();
    return NULL;
}

int FileSystem::open() {
    const std::string storage_dir_name = getTbStoragePath();

    // Create Tbase storage directory if necessary, parent is assumed to exist
    if (::mkdir(storage_dir_name.c_str(), 0700) && (errno != EEXIST)) {
        LOG_ERRNO("creating storage folder");
        return -1;
    }

    // Create partitions with default names
    for (int i = 0; i < 16; i++) {
        char file_name[16];
        snprintf(file_name, sizeof(file_name), "/Store_%1X.tf", i);
        pimpl_->partitions[i] = new Partition(storage_dir_name + file_name);
    }

    /* Prepare DCI message buffer */
    ::memset(&pimpl_->dci, 0, sizeof(pimpl_->dci));
    pimpl_->dci.exchange_buffer.nWorkspaceLength = DEFAULT_WORKSPACE_SIZE;

    // Create listening thread
    int ret = pthread_create(&pimpl_->thread, NULL, run, pimpl_);
    if (ret) {
        LOG_E("pthread_create failed with error code %d", ret);
        return -1;
    }

    pthread_setname_np(pimpl_->thread, "McDaemon.FileSystem");
    return 0;
}

int FileSystem::close() {
    // Stop listening thread
    pthread_kill(pimpl_->thread, SIGUSR1);
    pthread_join(pimpl_->thread, NULL);

    /* Clear DCI message buffer */
    ::memset(&pimpl_->dci, 0, sizeof(pimpl_->dci));
    return 0;
}

/*
 * main
 */
void FileSystem::Impl::run() {
    // Only accept USR1, to give up
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK, &sigmask, (sigset_t*)0);

    mcResult_t mc_ret = mcOpenDevice(MC_DEVICE_ID_DEFAULT);
    if (MC_DRV_OK != mc_ret) {
        LOG_E("%s: mcOpenDevice returned: 0x%08X\n", __func__, mc_ret);
        return;
    }

    const mcUuid_t uuid = SERVICE_DELEGATION_UUID;
    mcSessionHandle_t session_handle;
    session_handle.deviceId = MC_DEVICE_ID_DEFAULT;
    mc_ret = mcOpenSession(&session_handle, &uuid, reinterpret_cast<uint8_t*>(&dci),
                           sizeof(dci));
    if (MC_DRV_OK != mc_ret) {
        LOG_E("%s: mcOpenSession returned: 0x%08X\n", __func__, mc_ret);
        mcCloseDevice(MC_DEVICE_ID_DEFAULT);
        return;
    }

    LOG_I("%s: Start listening for request from STH2", __func__);
    while (true) {
        /* Wait for notification from SWd */
        LOG_D("%s: Waiting for notification\n", __func__);
        mc_ret = mcWaitNotification(&session_handle, MC_INFINITE_TIMEOUT_INTERRUPTIBLE);
        if (mc_ret != MC_DRV_OK) {
            LOG_E("%s: mcWaitNotification failed, error=0x%08X\n", __func__, mc_ret);
            break;
        }

        /* Read sector size */
        if (sector_size == 0) {
            sector_size = dci.exchange_buffer.nSectorSize;
            LOG_D("%s: Sector Size: %d bytes", __func__, sector_size);

            /* Check sector size */
            if (!(sector_size == 512 || sector_size == 1024 ||
                    sector_size == 2048 || sector_size == 4096)) {
                LOG_E("%s: Incorrect sector size: terminating...", __func__);
                break;
            }
        }

        LOG_D("%s: Received Command from STH2\n", __func__);
        if (executeCommand()) {
            break;
        }

        /* Set "listening" flag before notifying SPT2 */
        dci.exchange_buffer.nDaemonState = STH2_DAEMON_LISTENING;
        mc_ret = mcNotify(&session_handle);
        if (mc_ret != MC_DRV_OK) {
            LOG_E("%s: mcNotify() returned: 0x%08X\n", __func__, mc_ret);
            break;
        }
    }

    mc_ret = mcCloseSession(&session_handle);
    if (MC_DRV_OK != mc_ret) {
        LOG_E("%s: mcCloseSession returned: 0x%08X\n", __func__, mc_ret);
    }

    mc_ret = mcCloseDevice(MC_DEVICE_ID_DEFAULT);
    if (MC_DRV_OK != mc_ret) {
        LOG_E("%s: mcCloseDevice returned: 0x%08X\n", __func__, mc_ret);
    }

    LOG_W("%s: Exiting Filesystem thread", __func__);
}

/*----------------------------------------------------------------------------
 * Instructions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Command dispatcher function
 *----------------------------------------------------------------------------*/
/* Debug function to show the command name */
const char* FileSystem::Impl::getCommandtypeString(uint32_t nInstructionID) {
    switch (nInstructionID) {
        case DELEGATION_INSTRUCTION_PARTITION_CREATE:
            return "PARTITION_CREATE";
        case DELEGATION_INSTRUCTION_PARTITION_OPEN:
            return "PARTITION_OPEN";
        case DELEGATION_INSTRUCTION_PARTITION_READ:
            return "PARTITION_READ";
        case DELEGATION_INSTRUCTION_PARTITION_WRITE:
            return "PARTITION_WRITE";
        case DELEGATION_INSTRUCTION_PARTITION_SET_SIZE:
            return "PARTITION_SET_SIZE";
        case DELEGATION_INSTRUCTION_PARTITION_SYNC:
            return "PARTITION_SYNC";
        case DELEGATION_INSTRUCTION_PARTITION_CLOSE:
            return "PARTITION_CLOSE";
        case DELEGATION_INSTRUCTION_PARTITION_DESTROY:
            return "PARTITION_DESTROY";
        case DELEGATION_INSTRUCTION_SHUTDOWN:
            return "SHUTDOWN";
        case DELEGATION_INSTRUCTION_NOTIFY:
            return "NOTIFY";
        default:
            return "UNKNOWN";
    }
}

int FileSystem::Impl::executeCommand() {
    TEEC_Result    nError;
    uint32_t       nInstructionsIndex;
    uint32_t       nInstructionsBufferSize =
        dci.exchange_buffer.nInstructionsBufferSize;


    LOG_D("%s: nInstructionsBufferSize=%d", __func__, nInstructionsBufferSize);

    /* Reset the operation results */
    nError = TEEC_SUCCESS;
    dci.exchange_buffer.sAdministrativeData.nSyncExecuted = 0;
    ::memset(dci.exchange_buffer.sAdministrativeData.nPartitionErrorStates, 0,
             sizeof(dci.exchange_buffer.sAdministrativeData.nPartitionErrorStates));
    ::memset(dci.exchange_buffer.sAdministrativeData.nPartitionOpenSizes, 0,
             sizeof(dci.exchange_buffer.sAdministrativeData.nPartitionOpenSizes));

    /* Execute the instructions */
    nInstructionsIndex = 0;
    while (true) {
        DELEGATION_INSTRUCTION* pInstruction;
        uint32_t nInstructionID;
        pInstruction = (DELEGATION_INSTRUCTION*)(
                           &dci.exchange_buffer.sInstructions[nInstructionsIndex/4]);
        if (nInstructionsIndex + 4 > nInstructionsBufferSize) {
            LOG_D("%s: Instruction buffer end, size = %i", __func__,
                  nInstructionsBufferSize);
            return 0;
        }

        nInstructionID = pInstruction->sGeneric.nInstructionID;
        nInstructionsIndex += 4;

        LOG_D("%s: nInstructionID=0x%02X [%s], nInstructionsIndex=%d", __func__,
              nInstructionID, getCommandtypeString(nInstructionID), nInstructionsIndex);

        if ((nInstructionID & 0x0F) == 0) {
            /* Partition-independent instruction */
            switch (nInstructionID) {
                case DELEGATION_INSTRUCTION_SHUTDOWN: {
                    return 1;
                }
                case DELEGATION_INSTRUCTION_NOTIFY: {
                    /* Parse the instruction parameters */
                    wchar_t  pMessage[100];
                    uint32_t nMessageType;
                    uint32_t nMessageSize;
                    ::memset(pMessage, 0, 100*sizeof(wchar_t));

                    if (nInstructionsIndex + 8 > nInstructionsBufferSize) {
                        return 0;
                    }

                    nMessageType = pInstruction->sNotify.nMessageType;
                    nMessageSize = pInstruction->sNotify.nMessageSize;
                    nInstructionsIndex += 8;
                    if (nMessageSize > (99)*sizeof(wchar_t)) {
                        /* How to handle the error correctly in this case ? */
                        return 0;
                    }

                    if (nInstructionsIndex + nMessageSize > nInstructionsBufferSize) {
                        return 0;
                    }

                    ::memcpy(pMessage, &pInstruction->sNotify.nMessage[0], nMessageSize);
                    nInstructionsIndex += nMessageSize;
                    /* Align the pInstructionsIndex on 4 bytes */
                    nInstructionsIndex = (nInstructionsIndex + 3)&~3;
                    switch (nMessageType) {
                        case DELEGATION_NOTIFY_TYPE_ERROR:
                            LOG_E("%s: %ls", __func__, pMessage);
                            break;
                        case DELEGATION_NOTIFY_TYPE_WARNING:
                            LOG_W("%s: %ls", __func__, pMessage);
                            break;
                        case DELEGATION_NOTIFY_TYPE_DEBUG:
                            LOG_D("%s: DEBUG: %ls", __func__, pMessage);
                            break;
                        default:
                            LOG_D("%s: %ls", __func__, pMessage);
                    }
                    break;
                }
                default: {
                    LOG_E("%s: Unknown instruction identifier: %02X", __func__, nInstructionID);
                    nError = S_ERROR_BAD_PARAMETERS;
                    break;
                }
            }
        } else {
            /* Partition-specific instruction */
            uint32_t nPartitionID = (nInstructionID & 0xF0) >> 4;
            Partition* partition = partitions[nPartitionID];
            if (dci.exchange_buffer.sAdministrativeData.nPartitionErrorStates[nPartitionID]
                    == S_SUCCESS) {
                /* Execute the instruction only if there is currently no error on the partition */
                switch (nInstructionID & 0x0F) {
                    case DELEGATION_INSTRUCTION_PARTITION_CREATE: {
                        nError = partition->create();
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nError);
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_OPEN: {
                        nError = partition->open();
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d pSize=%ld err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, partition->size() / sector_size,
                              nError);
                        if (nError == S_SUCCESS) {
                            dci.exchange_buffer.sAdministrativeData.nPartitionOpenSizes[nPartitionID] =
                                static_cast<unsigned int>(partition->size() / sector_size);
                        }
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_READ: {
                        /* Parse parameters */
                        uint32_t nSectorID;
                        uint32_t nWorkspaceOffset;
                        if (nInstructionsIndex + 8 > nInstructionsBufferSize) {
                            return 0;
                        }

                        nSectorID = pInstruction->sReadWrite.nSectorID;
                        nWorkspaceOffset = pInstruction->sReadWrite.nWorkspaceOffset;
                        nInstructionsIndex += 8;
                        LOG_D("%s: >Partition %1X: read sector 0x%08X into workspace at offset 0x%08X",
                              __func__, nPartitionID, nSectorID, nWorkspaceOffset);
                        nError = partition->read(dci.exchange_buffer.sWorkspace + nWorkspaceOffset,
                                                 sector_size, nSectorID * sector_size);
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d sid=%d woff=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nSectorID, nWorkspaceOffset, nError);
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_WRITE: {
                        /* Parse parameters */
                        uint32_t nSectorID;
                        uint32_t nWorkspaceOffset;
                        if (nInstructionsIndex + 8 > nInstructionsBufferSize) {
                            return 0;
                        }

                        nSectorID = pInstruction->sReadWrite.nSectorID;
                        nWorkspaceOffset = pInstruction->sReadWrite.nWorkspaceOffset;
                        nInstructionsIndex += 8;
                        LOG_D("%s: >Partition %1X: write sector 0x%X from workspace at offset 0x%X",
                              __func__, nPartitionID, nSectorID, nWorkspaceOffset);
                        nError = partition->write(dci.exchange_buffer.sWorkspace + nWorkspaceOffset,
                                                  sector_size, nSectorID * sector_size);
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d sid=%d woff=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nSectorID, nWorkspaceOffset, nError);
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_SYNC: {
                        nError = partition->sync();
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nError);
                        if (nError == S_SUCCESS) {
                            dci.exchange_buffer.sAdministrativeData.nSyncExecuted++;
                        }
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_SET_SIZE: {
                        // nNewSize is a number of sectors
                        uint32_t nNewSize;
                        if (nInstructionsIndex + 4 > nInstructionsBufferSize) {
                            return 0;
                        }

                        nNewSize = pInstruction->sSetSize.nNewSize;
                        nInstructionsIndex += 4;
                        nError = partition->resize(nNewSize * sector_size);
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d nNewSize=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nNewSize, nError);
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_CLOSE: {
                        nError = partition->close();
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nError);
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_DESTROY: {
                        nError = partition->destroy();
                        LOG_D("%s: INSTRUCTION: ID=0x%x pid=%d err=0x%08X", __func__,
                              (nInstructionID & 0x0F), nPartitionID, nError);
                        break;
                    }
                    default: {
                        LOG_E("%s: Unknown instruction identifier: %02X", __func__, nInstructionID);
                        nError = S_ERROR_BAD_PARAMETERS;
                        break;
                    }
                }
                dci.exchange_buffer.sAdministrativeData.nPartitionErrorStates[nPartitionID] =
                    nError;
            } else {
                /* Skip the instruction if there is currently error on the partition */
                switch (nInstructionID & 0x0F) {
                    case DELEGATION_INSTRUCTION_PARTITION_CREATE:
                    case DELEGATION_INSTRUCTION_PARTITION_OPEN:
                    case DELEGATION_INSTRUCTION_PARTITION_SYNC:
                    case DELEGATION_INSTRUCTION_PARTITION_CLOSE:
                    case DELEGATION_INSTRUCTION_PARTITION_DESTROY:
                        break;
                    case DELEGATION_INSTRUCTION_PARTITION_READ:
                    case DELEGATION_INSTRUCTION_PARTITION_WRITE: {
                        if (nInstructionsIndex + 8 > nInstructionsBufferSize) {
                            return 0;
                        }
                        nInstructionsIndex += 8;
                        break;
                    }
                    case DELEGATION_INSTRUCTION_PARTITION_SET_SIZE: {
                        if (nInstructionsIndex + 4 > nInstructionsBufferSize) {
                            return 0;
                        }
                        nInstructionsIndex += 4;
                        break;
                    }
                    default: {
                        LOG_E("%s: Unknown instruction identifier: %02X", __func__, nInstructionID);
                        /* OMS: update partition error with BAD PARAM ? */
                        break;
                    }
                }
            }
        }
    }
    return 0;
}
