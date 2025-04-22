#ifndef EXAMINE_H
#define EXAMINE_H

#include "imapconnection.h"
#include "imap.h"

#define EXAMINE_COMMAND "EXAMINE"

ImapResponse examineRaw(ImapConnection *imap, const std::map<std::string, std::string> &params);
std::map<std::string, std::string> examineParse(const ImapResponse &result);


#endif // EXAMINE_H
