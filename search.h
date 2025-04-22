#ifndef SEARCH_H
#define SEARCH_H

#include "imapcommand.h"
#include "imapconnection.h"
#include <vector>
#include <map>

#define SEARCH_COMMAND "SEARCH"
#define UID_SEARCH_COMMAND "UID SEARCH"

ImapResponse searchRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
ImapResponse uidSearchRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
std::vector<std::string> searchParse(const ImapResponse &result);

#endif // SEARCH_H
