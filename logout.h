#ifndef LOGOUT_H
#define LOGOUT_H

#include <string>
#include "imapcommand.h"

#include "imapconnection.h"

class Logout: public ImapCommand {
public:
    Logout();
    std::pair<std::string, std::string> rawPerform(ImapConnection* imap) ;
    bool perform(ImapConnection* imap);
};

#endif // LOGOUT_H
