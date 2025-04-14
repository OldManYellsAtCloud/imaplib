#include "capability.h"
#include <loglib/loglib.h>

Capability::Capability() : ImapCommand{}
{
    command = "CAPABILITY";
}

ImapResult Capability::rawPerform(ImapConnection *imap)
{
    return imap->sendCommand(command);
}

std::string Capability::cleanResponse(const std::string& response, size_t successStart) {
    const std::string commandString = std::format("* {} ", command);
    std::string ret = response.substr(commandString.length(), successStart - commandString.length());
    return ret;
}

std::vector<std::string> Capability::perform(ImapConnection *imap)
{
    ImapResult res = rawPerform(imap);
    std::vector<std::string> capabilities;

    if (res.second.empty()) {
        LOG_ERROR("Capability - Could not retrieve any capabilities");
        return capabilities;
    }

    size_t successStart = checkSuccess(res);

    if (successStart == std::string::npos) {
        LOG_ERROR_F("Capability - Unsuccessful request: {}", res.second);
        return capabilities;
    }

    std::string cleanedResponse = cleanResponse(res.second, successStart);

    size_t idx = 0, prev_idx = 0;
    while ((idx = cleanedResponse.find(" ", prev_idx)) != std::string::npos) {
        capabilities.push_back(cleanedResponse.substr(prev_idx, idx - prev_idx));
        prev_idx = idx + 1;
    }

    capabilities.push_back(cleanedResponse.substr(prev_idx));
    return capabilities;
}
