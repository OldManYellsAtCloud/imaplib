#include "imapconnection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <loglib/loglib.h>
#include <poll.h>

#include <openssl/err.h>

inline int logSslError()
{
    int err;
    int count = 0;
    while ((err = ERR_get_error())) {
        if (err != 167772454)
            ++count;
        char *str = ERR_error_string(err, 0);
        if (!str)
            break;
        LOG_ERROR_F("Err {} - {}", err, str);
    }
    return count;
}

void ImapConnection::readMessagesThread(std::stop_token *st)
{
    std::string ret;
    std::string fullResponse;

    struct pollfd clientFd[1];
    clientFd[0].fd = sock;
    clientFd[0].events = POLLIN;

    while (!st->stop_requested()) {

        poll(clientFd, 1, 1000);
        // if there is nothing to read, unlock the mutex, so
        // maybe someone waiting to write can grab it
        if (!(clientFd[0].revents & POLLIN)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        fullResponse.clear();

        while (!(ret = readMessage()).empty()) {
            fullResponse.append(ret);
        }

        if (logSslError() > 0) {
            LOG_FATAL("SSL connection broke down!");
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

std::pair<std::string, std::string> ImapConnection::sendCommand(std::string cmd)
{
    std::string serialId = std::format("A{:0>5}", serial++);
    std::string cmdWithSerial = std::format("{} {}\r\n", serialId, cmd);
    std::pair<std::string, std::string> ret;
    ret.first = serialId;
    sendMessage(cmdWithSerial.c_str());
    ret.second = waitForResponse(serialId);

    return ret;
}

size_t ImapConnection::sendMessage(const char *buf)
{
    LOG_DEBUG_F("C: {}", buf);
    const std::lock_guard<std::mutex> lock(socket_mutex);
    int len = SSL_write(ssl, buf, strlen(buf));
    if (len <= 0) {
        logSslError();
    }
    return len;
}

std::string ImapConnection::readMessage()
{
    int recv_bytes;
    std::string ret;
    const std::lock_guard<std::mutex> lock(socket_mutex);

    while ((recv_bytes = SSL_read(ssl, buffer.data(), buffer.size())) > 0) {
        ret.append(buffer.data(), recv_bytes);
    }

    if (logSslError() > 0) {
        LOG_ERROR("SSL connection problem?");
        ok = false;
    }

    LOG_DEBUG_F("S: {}", ret);

    return ret;
}

std::string ImapConnection::waitForResponse(const std::string serial)
{
    std::string ret;
    std::string messageEndMarker = std::format("\r\n{} ", serial);
    while (ret.find(messageEndMarker) == std::string::npos)
        ret.append(readMessage());
    return ret;
}


ImapConnection::ImapConnection() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_ERROR_F("ImapConnection: Could not open socket: {}", strerror(errno));
        return;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    // make it timeout if there is nothing for 1s
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv));

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("142.251.18.108");
    sa.sin_port = htons(993);
    socklen_t socklen = sizeof(sa);

    if (connect(sock, (struct sockaddr *)&sa, socklen)) {
        LOG_ERROR_F("ImapConnection: Could not connect to server: {}", strerror(errno));
        ok = false;
        return;
    }

    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    SSL_CTX_set_options(ctx, SSL_OP_IGNORE_UNEXPECTED_EOF);

    ssl = SSL_new(ctx);

    if (!ssl){
        LOG_ERROR_F("ImapConnection: Could not create SSL object: {}", strerror(errno));
        logSslError();
        ok = false;
        return;
    }

    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) < 0){
        LOG_ERROR_F("ImapConnection: Could not create SSL connection: {}", strerror(errno));
        logSslError();
        ok = false;
        return;
    }

    readMessage();

    LOG_INFO("SSL Connection successful");
    ok = true;
}

ImapConnection::~ImapConnection()
{
    listenerThread.request_stop();
}

bool ImapConnection::isOk()
{
    return ok;
}
