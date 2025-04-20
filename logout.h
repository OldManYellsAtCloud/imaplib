#ifndef LOGOUT_H
#define LOGOUT_H

#include <string>
#include "imapcommand.h"

#include "imapconnection.h"

#define LOGOUT_COMMAND "LOGOUT"

ImapResponse logoutRaw(ImapConnection* imap) ;
std::string logoutParse(ImapResponse response);

#endif // LOGOUT_H
