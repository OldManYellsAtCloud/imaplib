#ifndef GMAILTAG_H
#define GMAILTAG_H

#include "imapconnection.h"
#include "imapcommand.h"
#include <map>

#define GMAIL_TAG_COMMAND "FETCH {} (X-GM-LABELS)"
#define GMAIL_TAG_UID_COMMAND "UID FETCH {} (X-GM-LABELS)"

ImapResponse gmailTagsRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
ImapResponse gmailTagsUidRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
std::string gmailTagsParse(const ImapResponse &response);

#endif // GMAILTAG_H
