#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H
#include <string>
#include "imapcommand.h"

enum AUTH_PROPERTY {
    USERNAME = 0,
    PASSWORD = 1
};

class Authenticate: public ImapCommand {
private:
    std::string username;
    std::string password;
public:
    Authenticate();
    ImapResult rawPerform(ImapConnection *imap) override;
    bool perform(ImapConnection *imap);
    void setProperty(const int &property, const std::string &value) override;
};

#endif // AUTHENTICATE_H
