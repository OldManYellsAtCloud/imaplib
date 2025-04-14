#ifndef IMAPCONNECTION_H
#define IMAPCONNECTION_H
#include <stddef.h>
#include <openssl/ssl.h>
#include <mutex>
#include <thread>

class ImapConnection
{
    bool ok;
    SSL *ssl;
    std::mutex socket_mutex;
    int sock;
    std::jthread listenerThread;
    std::stop_token stop;
    std::array<char, 10240> buffer;
    int serial = 1;
public:
    ImapConnection();
    ~ImapConnection();
    size_t sendMessage(const char *buf);
    std::string readMessage();
    std::string waitForResponse(const std::string serial);
    void readMessagesThread(std::stop_token *st);
    std::pair<std::string, std::string> sendCommand(std::string cmd);

    bool isOk();
};

#endif // IMAPCONNECTION_H
