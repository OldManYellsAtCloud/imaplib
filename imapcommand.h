#ifndef IMAPCOMMAND_H
#define IMAPCOMMAND_H

#include <string>
#include "imapconnection.h"

typedef std::pair<std::string, std::string> ImapResult;

class ImapCommand {
protected:
    std::string command;
public:
    ImapCommand();
    virtual ~ImapCommand() {};
    virtual size_t checkSuccess(ImapResult result);
    virtual ImapResult rawPerform(ImapConnection *imap) = 0;
    virtual void setProperty(const std::string& property, const std::string& value);
    virtual void setProperty(const int& property, const std::string& value);
};

#endif // IMAPCOMMAND_H
