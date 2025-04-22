#ifndef SELECT_H
#define SELECT_H

#include <map>
#include "imapconnection.h"
#include "imapcommand.h"

#define SELECT_COMMAND "SELECT"

ImapResponse selectRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
std::string selectParse(const ImapResponse &response);

#endif // SELECT_H
