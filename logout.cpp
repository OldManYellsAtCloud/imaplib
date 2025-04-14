#include "logout.h"

Logout::Logout() : ImapCommand{}
{
    this->command = "LOGOUT";
}

ImapResult Logout::rawPerform(ImapConnection *imap)
{
    return imap->sendCommand(command);
}

bool Logout::perform(ImapConnection *imap)
{
    std::pair<std::string, std::string> res = rawPerform(imap);
    return res.second.find(std::format("{} OK", res.first)) != std::string::npos;
}


