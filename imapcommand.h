#ifndef IMAPCOMMAND_H
#define IMAPCOMMAND_H

#include <string>

typedef std::pair<std::string, std::string> ImapResponse;
bool checkSuccess(const ImapResponse &result);

#endif // IMAPCOMMAND_H
