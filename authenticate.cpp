#include "authenticate.h"
#include "base64.h"
#include <loglib/loglib.h>

Authenticate::Authenticate() : ImapCommand{}
{
    command = "AUTHENTICATE";
}

void Authenticate::setProperty(const int &property, const std::string &value)
{
    switch (property) {
    case AUTH_PROPERTY::USERNAME:
        username = value;
        break;
    case AUTH_PROPERTY::PASSWORD:
        password = value;
        break;
    default:
        LOG_ERROR_F("Authenticate - setProperty: unknown property: {}", property);
    }
}

bool Authenticate::perform(ImapConnection *imap)
{
    std::pair<std::string, std::string> res = rawPerform(imap);
    return res.second.find(std::format("{} OK", res.first)) != std::string::npos;
}

std::pair<std::string, std::string> Authenticate::rawPerform(ImapConnection *imap)
{
    if (username.empty() || password.empty())
        LOG_ERROR("Authenticate - rawPerform: missing username of password!");

    // plain authentication requires the following format:
    // BASE64(<NUL>username<NUL>password)
    std::vector<uint8_t> rawCredentials;
    rawCredentials.push_back(0);
    rawCredentials.insert(rawCredentials.end(), username.begin(), username.end());
    rawCredentials.push_back(0);
    rawCredentials.insert(rawCredentials.end(), password.begin(), password.end());

    std::string base64Auth = base64::encode_base64(rawCredentials);
    std::string cmd = std::format("{} PLAIN {}", command, base64Auth);
    return imap->sendCommand(cmd);
}


