#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <vector>
#include "imapconnection.h"
#include "imapcommand.h"


class Capability: public ImapCommand {
private:
    std::string cleanResponse(const std::string& response, size_t successStart);
public:
    Capability();
    ImapResult rawPerform(ImapConnection *imap) override;
    std::vector<std::string> perform(ImapConnection *imap);
};
#endif // CAPABILITY_H
