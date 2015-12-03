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

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <string.h>     // strncpy
#include <sys/mman.h>   // mmap, munmap
#include <sys/socket.h>
#include <sys/un.h>

#undef LOG_TAG
#define LOG_TAG "TeeProxyClient"
#include "log.h"
#include "proxy_common.h"
#include "proxy_client.h"

using namespace com::trustonic::tee_proxy;

class ClientConnection {
    std::string buffer_;
    std::mutex comm_mutex_;
    std::mutex comm_queue_mutex_;
    int socket_ = -1;
    bool is_open_ = false;
    bool is_broken_ = false;
    Session* session_ = nullptr;
public:
    ~ClientConnection() {
        if (is_open_) {
            close();
        }
        delete session_;
    }
    int open() {
        socket_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (socket_ < 0) {
            LOG_ERRNO("socket");
            return -1;
        }
        struct sockaddr_un sock_un;
        sock_un.sun_family = AF_UNIX;
        ::strncpy(sock_un.sun_path, SOCKET_PATH, sizeof(sock_un.sun_path) - 1);
        socklen_t len = static_cast<socklen_t>(strlen(sock_un.sun_path) +
                                               sizeof(sock_un.sun_family));
        sock_un.sun_path[0] = '\0';         // Abstract
        if (::connect(socket_, reinterpret_cast<struct sockaddr*>(&sock_un),
                      len) < 0) {
            LOG_ERRNO("connect");
            return -1;
        }
        is_open_ = true;
        LOG_D("socket %d connected", socket_);
        return 0;
    }
    int close() {
        ::close(socket_);
        is_open_ = false;
        LOG_D("socket %d session %x closed", socket_, session_ ? session_->id() : 0);
        return 0;
    }
    bool isBroken() const {
        return is_broken_;
    }
    void setSession(Session* session) {
        session_ = session;
    }
    Session* session() const {
        return session_;
    }
    int call(RpcMethod method, const ::google::protobuf::MessageLite& request,
             ::google::protobuf::MessageLite& response) {
        // Trick to ensure round-robin access to two callers
        comm_queue_mutex_.lock();
        std::lock_guard<std::mutex> comm_lock(comm_mutex_);
        comm_queue_mutex_.unlock();
        LOG_D("socket %d session %x call %s", socket_,
              session_ ? session_->id() : 0, methodToString(method));

        // Request
        {
            // Serialize request
            if (!request.SerializeToString(&buffer_)) {
                LOG_E("Failed to serialize");
                is_broken_ = true;
                return -1;
            }

            // Send request header
            RequestHeader header;
            ::memcpy(header.magic, PROTOCOL_MAGIC, sizeof(header.magic));
            header.version = PROTOCOL_VERSION;
            header.method = static_cast<uint16_t>(method);
            header.length = static_cast<uint32_t>(buffer_.length());
            if (send_all(socket_, "header", &header, sizeof(header))) {
                is_broken_ = true;
                return -1;
            }

            // Send request data
            LOG_D("send %u bytes of data", header.length);
            if (send_all(socket_, "data", &buffer_[0], header.length)) {
                is_broken_ = true;
                return -1;
            }
        }

        // Response
        {
            // Receive response header
            ResponseHeader header;
            if (recv_all(socket_, "header", &header, sizeof(header))) {
                is_broken_ = true;
                return -1;
            }

            // Check header
            if (::memcmp(header.magic, PROTOCOL_MAGIC, sizeof(header.magic))) {
                LOG_E("Wrong magic");
                is_broken_ = true;
                return -1;
            }
            if (header.version != PROTOCOL_VERSION) {
                LOG_E("Wrong version");
                is_broken_ = true;
                return -1;
            }
            if (header.proto_rc < 0) {
                errno = -header.proto_rc;
                LOG_E("Protocol error reported: %s",
                      strerror(-header.proto_rc));
                is_broken_ = true;
                return -1;
            }

            // Receive response data
            LOG_D("receive %d bytes of data", header.proto_rc);
            if (header.proto_rc != 0) {
                // Receive response data
                int length = header.proto_rc;
                buffer_.resize(length);
                if (recv_all(socket_, "data", &buffer_[0], length)) {
                    is_broken_ = true;
                    return -1;
                }

                // Parse response
                if (!response.ParseFromString(buffer_)) {
                    LOG_E("Failed to parse");
                    is_broken_ = true;
                    return -1;
                }
            }

            // Method errors do not require specific method action
            if (header.method_rc) {
                errno = header.method_rc;
                LOG_D("Error reported: %s", strerror(errno));
                return -1;
            }
        }
        LOG_D("socket %d session %x call done", socket_,
              session_ ? session_->id() : 0);
        return 0;
    }
};

class SessionConnectionsList {
    std::mutex mutex_;
    std::vector<ClientConnection*> connections_;
public:
    ClientConnection* find(uint32_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& connection: connections_) {
            if (connection->session()->id() == id) {
                return connection;
            }
        }
        return nullptr;
    }
    int deleteConnection(uint32_t id) {
        LOG_D("%s %p: %x", __FUNCTION__, this, id);
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find_if(connections_.begin(), connections_.end(),
        [this, id](ClientConnection* connection) {
            return connection->session()->id() == id;
        });
        if (it == connections_.end()) {
            // Not found
            return -1;
        }
        delete *it;
        connections_.erase(it);
        return 0;
    }
    void push_back(ClientConnection* connection) {
        LOG_D("%s %p: %x", __FUNCTION__, this, connection->session()->id());
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.push_back(connection);
    }
    bool empty() const {
        return connections_.empty();
    }
};

struct ProxyClient::Impl {
    bool is_open_;
    // Versions
    mc_version_info version_info;
    // Sessions
    SessionConnectionsList connections;
    Impl(): is_open_(false) {
        ::memset(&version_info, 0, sizeof(version_info));
    }
    int getVersion(ClientConnection& conn, struct mc_version_info& version_info);
};

ProxyClient::ProxyClient(): pimpl_(new Impl) {
}

ProxyClient::~ProxyClient() {
    delete pimpl_;
}

int ProxyClient::open() {
    ClientConnection conn;
    if (conn.open()) {
        return -1;
    }
    // Cache versions
    if (pimpl_->getVersion(conn, pimpl_->version_info)) {
        return -1;
    }
    conn.close();
    pimpl_->is_open_ = true;
    LOG_I("proxy client open");
    return 0;
}

int ProxyClient::close() {
    pimpl_->is_open_ = false;
    LOG_I("proxy client closed");
    return 0;
}

bool ProxyClient::isOpen() const {
    return pimpl_->is_open_;
}

int ProxyClient::hasOpenSessions() const {
    int ret = 0;
    if (!pimpl_->connections.empty()) {
        ret = -1;
        errno = ENOTEMPTY;
    }
    LOG_D("%s rc=%d", __FUNCTION__, ret);
    return ret;
}

int ProxyClient::openSession(struct mc_ioctl_open_session& session) {
    LOG_D("tci 0x%llx len %u", session.tci, session.tcilen);
    if ((session.tci && !session.tcilen) ||
            (!session.tci && session.tcilen)) {
        LOG_E("TCI and its length are inconsistent");
        return -1;
    }
    // So we return the correct error code
    if (session.tcilen > BUFFER_LENGTH_MAX) {
        errno = EINVAL;
        return -1;
    }
    // Open the session
    void* tci = reinterpret_cast<void*>(static_cast<uintptr_t>(session.tci));
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    OpenSessionRequest request;
    request.set_uuid(static_cast<void*>(&session.uuid), sizeof(session.uuid));
    request.set_is_gp_uuid(session.is_gp_uuid);
    if (tci) {
        request.set_tci(tci, session.tcilen);
    }
    request.set_login_type(static_cast<LoginType>(session.identity.login_type));
    request.set_login_data(static_cast<void*>(session.identity.login_data),
                           sizeof(session.identity.login_data));
    // Create connection
    std::unique_ptr<ClientConnection> conn(new ClientConnection);
    if (conn->open()) {
        LOG_E("Failed to open connection");
        return -1;
    }
    // Exchange
    OpenSessionResponse response;
    if (conn->call(OPEN_SESSION, request, response)) {
        // Error logged in call()
        return -1;
    }
    // Response
    if (!response.has_id()) {
        LOG_E("Required parameter missing");
        return -1;
    }
    // Success
    errno = saved_errno;
    session.sid = response.id();
    LOG_D("session %x open", session.sid);
    Session::Buffer* buffer;
    if (tci) {
        buffer = new Session::Buffer(tci, session.tcilen);
    } else {
        buffer = nullptr;
    }
    conn->setSession(new Session(session.sid, buffer));
    pimpl_->connections.push_back(conn.release());
    return 0;
}

int ProxyClient::openTrustlet(struct mc_ioctl_open_trustlet& trustlet) {
    LOG_D("tci 0x%llx len %u", trustlet.tci, trustlet.tcilen);
    if ((trustlet.tci && !trustlet.tcilen) ||
            (!trustlet.tci && trustlet.tcilen)) {
        LOG_E("TCI and its length are inconsistent");
        return -1;
    }
    // So we return the correct error code
    if (trustlet.tcilen > BUFFER_LENGTH_MAX) {
        errno = EINVAL;
        return -1;
    }
    // Open the trustlet
    void* app = reinterpret_cast<void*>(static_cast<uintptr_t>(trustlet.buffer));
    void* tci = reinterpret_cast<void*>(static_cast<uintptr_t>(trustlet.tci));
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    OpenTrustletRequest request;
    request.set_spid(trustlet.spid);
    request.set_trustapp(app, trustlet.tlen);
    if (tci) {
        request.set_tci(tci, trustlet.tcilen);
    }
    // Create connection
    std::unique_ptr<ClientConnection> conn(new ClientConnection);
    if (conn->open()) {
        LOG_E("Failed to open connection");
        return -1;
    }
    // Exchange
    OpenTrustletResponse response;
    if (conn->call(OPEN_TRUSTLET, request, response)) {
        // Error logged in call()
        return -1;
    }
    // Response
    if (!response.has_id()) {
        LOG_E("Required parameter missing");
        return -1;
    }
    // Success
    errno = saved_errno;
    trustlet.sid = response.id();
    LOG_D("session %x open", trustlet.sid);
    // Create session management object
    Session::Buffer* buffer;
    if (tci) {
        buffer = new Session::Buffer(tci, trustlet.tcilen);
    } else {
        buffer = nullptr;
    }
    conn->setSession(new Session(trustlet.sid, buffer));
    pimpl_->connections.push_back(conn.release());
    return 0;
}

int ProxyClient::closeSession(uint32_t session_id) {
    LOG_D("session %x close", session_id);
    // Find session connection
    auto connection = pimpl_->connections.find(session_id);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    CloseSessionRequest request;
    request.set_id(session_id);
    // Exchange
    CloseSessionResponse response;
    if (connection->call(CLOSE_SESSION, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(session_id);
        }
        return -1;
    }
    // No response
    // Success
    errno = saved_errno;
    if (pimpl_->connections.deleteConnection(session_id)) {
        errno = ENXIO;
        return -1;
    }
    LOG_D("session %x closed", session_id);
    return 0;
}

int ProxyClient::notify(uint32_t session_id) {
    LOG_D("session %x notify", session_id);
    // Find session connection
    auto connection = pimpl_->connections.find(session_id);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    NotifyRequest request;
    request.set_sid(session_id);
    auto session = connection->session();
    if (session->hasTci()) {
        request.set_tci(session->tci(), session->tciLen());
    }
    std::lock_guard<std::mutex> buffers_lock(session->buffersMutex());
    auto& buffers = session->buffers();
    for (auto& buf: buffers) {
        if (buf->info().flags & MC_IO_MAP_INPUT) {
            NotifyRequest_Buffers* buffer = request.add_buffers();
            buffer->set_sva(buf->info().sva);
            buffer->set_data(buf->address(), buf->info().len);
        }
    }
    // Exchange
    NotifyResponse response;
    if (connection->call(NOTIFY, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(session_id);
        }
        return -1;
    }
    // No response
    // Success
    errno = saved_errno;
    LOG_D("session %x notification sent", session_id);
    return 0;
}

int ProxyClient::waitNotification(const struct mc_ioctl_wait& wait) {
    LOG_D("session %x wait for notification", wait.sid);
    // Find session connection
    auto connection = pimpl_->connections.find(wait.sid);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    WaitNotificationRequest request;
    request.set_sid(wait.sid);
    // Timeout to server cannot exceed a few seconds, retry every 5s
    int32_t timeout_left = wait.timeout;
    WaitNotificationResponse response;
    bool failed = false;
    while (true) {
        if (wait.timeout < 0) {
            // Infinite
            request.set_timeout(timeout_max);
            request.set_partial(true);
        } else if (timeout_left > timeout_max) {
            // Big
            request.set_timeout(timeout_max);
            request.set_partial(true);
            timeout_left -= timeout_max;
        } else {
            // Small enough
            request.set_timeout(timeout_left);
            request.set_partial(false);
            timeout_left = 0;
        }
        LOG_D("timeout: asked=%d left=%d set=%d",
              wait.timeout, timeout_left, request.timeout());
        // Exchange
        if (!connection->call(WAIT, request, response)) {
            LOG_D("done");
            break;
        }
        // Real timeout or other error
        if ((errno != ETIME) || !request.partial()) {
            // Error logged in call()
            if (connection->isBroken()) {
                pimpl_->connections.deleteConnection(wait.sid);
                LOG_D("abort");
                return -1;
            }
            LOG_D("give up, but update buffers");
            failed = true;
            break;
        }
        LOG_D("retry");
    }
    // Response
    auto session = connection->session();
    if (response.has_tci() && session->updateTci(response.tci())) {
        LOG_E("Could not update TCI");
        return -1;
    }
    for (int i = 0; i < response.buffers_size(); i++) {
        const WaitNotificationResponse_Buffers& buffer = response.buffers(i);
        if (!buffer.has_sva() || !buffer.has_data()) {
            LOG_E("Required parameter missing");
            return -1;
        }
        std::lock_guard<std::mutex> buffers_lock(session->buffersMutex());
        auto buf = session->findBuffer(buffer.sva());
        if (!buf) {
            LOG_E("Buffer not found for SVA %jx", buffer.sva());
            return -1;
        }
        if (buffer.data().length() != buf->info().len) {
            LOG_E("Buffer sizes differ for SVA %jx: %zu != %u",
                  buffer.sva(), buffer.data().length(), buf->info().len);
            return -1;
        }
        ::memcpy(buf->address(), buffer.data().c_str(), buf->info().len);
    }
    if (failed) {
        return -1;
    }
    // Success
    errno = saved_errno;
    LOG_D("session %x notification received", wait.sid);
    return 0;
}

int ProxyClient::malloc(uint8_t** buffer, uint32_t length) {
    // Cannot share kernel buffers through the proxy
    if (!length) {
        errno = EINVAL;
        return -1;
    }
    *buffer = (uint8_t*)::mmap(0, length, PROT_READ | PROT_WRITE,
                               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (*buffer == MAP_FAILED) {
        errno = ENOMEM;
        return -1;
    }
    LOG_D("mmap'd buffer %p len %u", *buffer, length);
    return 0;
}

int ProxyClient::free(uint8_t* buffer, uint32_t length) {
    LOG_D("munmap'd buffer %p len %u", buffer, length);
    ::munmap(buffer, length);
    return 0;
}

int ProxyClient::map(struct mc_ioctl_map& map) {
    LOG_D("map buffer(s) to session %x", map.sid);
    // Find session connection
    auto connection = pimpl_->connections.find(map.sid);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    MapRequest request;
    request.set_sid(map.sid);
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (map.bufs[i].va) {
            if (map.bufs[i].len > BUFFER_LENGTH_MAX) {
                LOG_E("Incorrect length for buffer: %u", map.bufs[i].len);
                errno = -EINVAL;
                return -1;
            }
            auto buffer = request.add_buffers();
            buffer->set_len(map.bufs[i].len);
            buffer->set_flags(map.bufs[i].flags);
        }
    }
    // Exchange
    MapResponse response;
    if (connection->call(MAP, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(map.sid);
        }
        return -1;
    }
    // Response
    if (response.buffers_size() != request.buffers_size()) {
        LOG_E("Response buffers size (%d) does not match request's (%d)",
              response.buffers_size(), request.buffers_size());
        return -1;
    }
    int buffer_index = 0;
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (map.bufs[i].va) {
            const MapResponse_Buffers& buf = response.buffers(buffer_index++);
            if (!buf.has_sva()) {
                LOG_E("Required parameter missing");
                return -1;
            }
            map.bufs[i].sva = buf.sva();
        }
    }
    // Success
    errno = saved_errno;
    auto session = connection->session();
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (map.bufs[i].va) {
            session->addBuffer(map.bufs[i]);
        }
    }
    LOG_D("session %x buffer(s) mapped", map.sid);
    return 0;
}

int ProxyClient::unmap(const struct mc_ioctl_map& map) {
    LOG_D("unmap buffer(s) to session %x", map.sid);
    // Find session connection
    auto connection = pimpl_->connections.find(map.sid);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    UnmapRequest request;
    request.set_sid(map.sid);
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (map.bufs[i].va) {
            if (!map.bufs[i].sva) {
                errno = EINVAL;
                return -1;
            }
            UnmapRequest_Buffers* buffer = request.add_buffers();
            buffer->set_sva(map.bufs[i].sva);
        }
    }
    // Exchange
    UnmapResponse response;
    if (connection->call(UNMAP, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(map.sid);
        }
        return -1;
    }
    // No response
    // Success
    errno = saved_errno;
    auto session = connection->session();
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (map.bufs[i].va) {
            if (session->removeBuffer(map.bufs[i].sva)) {
                LOG_E("Unmapped buffer not found in session: %llu",
                      map.bufs[i].sva);
                return -1;
            }
        }
    }
    LOG_D("session %x buffer(s) unmapped", map.sid);
    return 0;
}

int ProxyClient::getError(struct mc_ioctl_geterr& err) {
    LOG_D("get session %x exit code", err.sid);
    // Find session connection
    auto connection = pimpl_->connections.find(err.sid);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    GetErrorRequest request;
    request.set_sid(err.sid);
    // Exchange
    GetErrorResponse response;
    if (connection->call(GET_ERROR, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(err.sid);
        }
        return -1;
    }
    // Response
    if (!response.has_exit_code()) {
        LOG_E("Required parameter missing");
        return -1;
    }
    // Success
    errno = saved_errno;
    err.value = response.exit_code();
    LOG_D("session %x exit code %d", err.sid, err.value);
    return 0;
}

int ProxyClient::Impl::getVersion(ClientConnection& conn,
                                  struct mc_version_info& version_info) {
    LOG_D("get version");
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    GetVersionRequest request;
    // No inputs to set
    // Exchange
    GetVersionResponse response;
    if (conn.call(GET_VERSION, request, response)) {
        // Error logged in call()
        return -1;
    }
    // Response
    if (!response.has_product_id() ||
            !response.has_mci() ||
            !response.has_so() ||
            !response.has_mclf() ||
            !response.has_container() ||
            !response.has_mc_config() ||
            !response.has_tl_api() ||
            !response.has_dr_api() ||
            !response.has_nwd()) {
        LOG_E("Required parameter missing");
        return -1;
    }
    // Success
    errno = saved_errno;
    ::strncpy(version_info.product_id, response.product_id().c_str(),
              MC_PRODUCT_ID_LEN);
    version_info.product_id[MC_PRODUCT_ID_LEN - 1] = '\0';
    version_info.version_mci = response.mci();
    version_info.version_so = response.so();
    version_info.version_mclf = response.mclf();
    version_info.version_container = response.container();
    version_info.version_mc_config = response.mc_config();
    version_info.version_tl_api = response.tl_api();
    version_info.version_dr_api = response.dr_api();
    version_info.version_nwd = response.nwd();
    return 0;
}

int ProxyClient::getVersion(struct mc_version_info& version_info) {
    if (!pimpl_->is_open_) {
        errno = EBADF;
        return -1;
    }
    version_info = pimpl_->version_info;
    return 0;
}

int ProxyClient::gpRequestCancellation(uint32_t session_id) {
    LOG_D("cancel GP operation on session %x", session_id);
    // Find session connection
    auto connection = pimpl_->connections.find(session_id);
    if (!connection) {
        errno = ENXIO;
        return -1;
    }
    int saved_errno = errno;
    errno = EHOSTUNREACH;
    // Request
    GpRequestCancellationRequest request;
    request.set_sid(session_id);
    // Exchange
    GpRequestCancellationResponse response;
    if (connection->call(GP_REQUESTCANCELLATION, request, response)) {
        // Error logged in call()
        if (connection->isBroken()) {
            pimpl_->connections.deleteConnection(session_id);
        }
        return -1;
    }
    // No response
    // Success
    errno = saved_errno;
    return 0;
}
