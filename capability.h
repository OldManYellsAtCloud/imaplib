#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <vector>
#include "imapconnection.h"
#include "imapcommand.h"

#define CAPABILITY_COMMAND "CAPABILITY"

std::string cleanResponse(const std::string& response, size_t successStart);

ImapResponse capabilityRaw(ImapConnection *imap);
std::vector<std::string> capabilityParse(const ImapResponse &result);

#endif // CAPABILITY_H
