#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H
#include <map>
#include <string>
#include "imapcommand.h"
#include "imapconnection.h"

#define AUTHENTICATE_COMMAND "AUTHENTICATE"

ImapResponse authenticateRaw(ImapConnection *imap, const std::map<std::string, std::string> &properties);
std::string authenticateParse(const ImapResponse &result);

#endif // AUTHENTICATE_H
