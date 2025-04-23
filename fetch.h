#ifndef FETCH_H
#define FETCH_H

#include <map>
#include "imapconnection.h"
#include "imapcommand.h"

#define FETCH_COMMAND "FETCH {} (BODY.PEEK[HEADER.FIELDS (DATE FROM TO SUBJECT CONTENT-TYPE)] BODY.PEEK[TEXT])"
#define FETCH_UID_COMMAND "UID FETCH {} (BODY.PEEK[HEADER.FIELDS (DATE FROM TO SUBJECT CONTENT-TYPE)] BODY.PEEK[TEXT])"

ImapResponse fetchMailRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
ImapResponse fetchMailUidRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
std::map<std::string, std::string> fetchMailParser(const ImapResponse &response);

#endif // FETCH_H
