#include "select.h"
#include <loglib/loglib.h>
#include <format>

ImapResponse selectRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("folderName")){
        LOG_ERROR_F("selectRaw - missing folderName argument");
        return ImapResponse{};
    }

    const std::string folder = params.at("folderName");
    const std::string cmd = std::format("{} \"{}\"", SELECT_COMMAND, folder);
    return imap->sendCommand(cmd);
}

std::string selectParse(const ImapResponse &response)
{
    return checkSuccess(response) ? "success" : "fail";
}
