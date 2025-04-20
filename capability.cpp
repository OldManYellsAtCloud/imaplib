#include "capability.h"
#include <loglib/loglib.h>


namespace {
std::string removeCommandFromStart(const std::string &response) {
    const std::string commandString = std::format("* {} ", CAPABILITY_COMMAND);
    size_t start = commandString.length();
    return response.substr(start);
}

std::string removeCommandResultFromEnd(const std::string &response, const std::string &serial) {
    const std::string successConfirmation = std::format("{} OK", serial);
    size_t end = response.find(successConfirmation);

    if (end == std::string::npos) {
        LOG_ERROR_F("Capability - removeCommandResultFromEnd - "
                    "Could not find success confirmation: {}", response);
        return "";
    }

    return response.substr(0, end);
}

}

ImapResponse capabilityRaw(ImapConnection *imap)
{
    return imap->sendCommand(CAPABILITY_COMMAND);
}

std::vector<std::string> capabilityParse(const ImapResponse &result)
{
    std::vector<std::string> capabilities;

    if (result.second.empty()) {
        LOG_ERROR("Capability - Could not retrieve any capabilities");
        return capabilities;
    }

    if (!checkSuccess(result)) {
        LOG_ERROR_F("Capability - Unsuccessful request: {}", result.second);
        return capabilities;
    }

    std::string cleanedResponse = removeCommandFromStart(result.second);
    cleanedResponse = removeCommandResultFromEnd(cleanedResponse, result.first);

    size_t idx = 0, prev_idx = 0;
    while ((idx = cleanedResponse.find(" ", prev_idx)) != std::string::npos) {
        capabilities.push_back(cleanedResponse.substr(prev_idx, idx - prev_idx));
        prev_idx = idx + 1;
    }

    capabilities.push_back(cleanedResponse.substr(prev_idx));
    return capabilities;
}
