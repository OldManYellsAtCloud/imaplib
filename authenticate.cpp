#include "authenticate.h"
#include "base64.h"
#include <loglib/loglib.h>

std::string authenticateParse(const ImapResponse &result)
{
    size_t successPosition = result.second.find(std::format("{} OK", result.first));

    return checkSuccess(result) ? "success" : "fail";
}

ImapResponse authenticateRaw(ImapConnection *imap, const std::map<std::string, std::string> &properties)
{
    if (!properties.contains("username") || !properties.contains("password"))
        LOG_ERROR("Authenticate - rawPerform: missing username of password!");

    std::string username = properties.at("username");
    std::string password = properties.at("password");

    // plain authentication requires the following format:
    // BASE64(<NUL>username<NUL>password)
    std::vector<uint8_t> rawCredentials;
    rawCredentials.push_back(0);
    rawCredentials.insert(rawCredentials.end(), username.begin(), username.end());
    rawCredentials.push_back(0);
    rawCredentials.insert(rawCredentials.end(), password.begin(), password.end());

    std::string base64Auth = base64::encode_base64(rawCredentials);
    std::string cmd = std::format("{} PLAIN {}", AUTHENTICATE_COMMAND, base64Auth);
    return imap->sendCommand(cmd);
}

