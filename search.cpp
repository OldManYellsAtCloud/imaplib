#include "search.h"
#include <loglib/loglib.h>
#include <format>

namespace {
bool validateResultSpecifier(const std::string &resultSpecifier){
    return resultSpecifier == "MIN" ||
           resultSpecifier == "MAX" ||
           resultSpecifier == "ALL" ||
           resultSpecifier == "COUNT" ||
           resultSpecifier == "SAVE";
}

std::vector<std::string> splitResponse(const std::string &response){
    size_t idx = 0, prev_idx = 0;
    std::vector<std::string> ret;
    while ((idx = response.find(" ", prev_idx)) != std::string::npos){
        ret.emplace_back(response.substr(prev_idx, idx - prev_idx));
        prev_idx = idx + 1;
    }
    ret.emplace_back(response.substr(prev_idx));
    return ret;
}

std::string removeCommandFromFront(const std::string &s){
    std::string cmdUsed = UID_SEARCH_COMMAND;
    size_t startOfSearchCommand = s.find(cmdUsed);
    if (startOfSearchCommand == std::string::npos){
        cmdUsed = SEARCH_COMMAND;
        startOfSearchCommand = s.find(cmdUsed);
    }
    return s.substr(startOfSearchCommand + cmdUsed.length() + 1);
}

std::string removeStatusMessageFromEnd(const std::string &s, const std::string &serial){
    size_t startOfStatus = s.find(std::format("{} OK", serial));
    return s.substr(0, startOfStatus - 2);
}

ImapResponse genericSearchRaw(ImapConnection *imap, const std::map<std::string, std::string> &params, const std::string &searchCommand)
{
    if (!params.contains("resultSpecifier")){
        LOG_ERROR("searchRaw - Missing resultSpecifier parameter");
        return ImapResponse{};
    }

    const std::string resultSpecifier = params.at("resultSpecifier");

    if (!validateResultSpecifier(resultSpecifier)){
        LOG_ERROR_F("searchRaw - Invalid resultSpecifier: {}", resultSpecifier);
        return ImapResponse{};
    }

    if (!params.contains("searchSpecifier")){
        LOG_ERROR("searchRaw - Missing searchSpecifier parameter");
        return ImapResponse{};
    }

    const std::string searchSpecifier = params.at("searchSpecifier");

    std::string cmd = std::format("{} {} {}", searchCommand, resultSpecifier, searchSpecifier);
    return imap->sendCommand(cmd);
}

}

ImapResponse searchRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    return genericSearchRaw(imap, params, SEARCH_COMMAND);
}

ImapResponse uidSearchRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    return genericSearchRaw(imap, params, UID_SEARCH_COMMAND);
}

std::vector<std::string> searchParse(const ImapResponse &result)
{
    if (!checkSuccess(result)){
        LOG_ERROR_F("searchParse - Unsuccessful response: {}", result.second);
        return std::vector<std::string>{};
    }
    std::string cleanedResponse = removeCommandFromFront(result.second);
    cleanedResponse = removeStatusMessageFromEnd(cleanedResponse, result.first);
    return splitResponse(cleanedResponse);
}
