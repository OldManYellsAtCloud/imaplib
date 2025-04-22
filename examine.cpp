#include "examine.h"
#include <loglib/loglib.h>
#include <regex>

namespace {
std::string getRegexMatch(const std::string &s, const std::string &pattern){
    std::regex r(pattern);
    std::smatch match;
    std::string ret = "ERROR";
    if (!std::regex_search(s, match, r)){
        LOG_ERROR_F("Examine - getRegexMatch - pattern {} failed on text: {}",
            pattern, s);
        return ret;
    }
    ret = match[1].str();
    LOG_DEBUG_F("Examine - getRegexMatch - pattern {} matched: {}", pattern, ret);
    return ret;
}


std::string getUidValidity(const std::string &s){
    return getRegexMatch(s, "UIDVALIDITY ([0-9]+)");
}

std::string getUidNext(const std::string &s){
    return getRegexMatch(s, "UIDNEXT ([0-9]+)");
}

std::string getExists(const std::string &s){
    return getRegexMatch(s, "([0-9]+) EXISTS");
}

std::string getRecent(const std::string &s){
    return getRegexMatch(s, "([0-9]+) RECENT");
}
} // anonymous namespace

ImapResponse examineRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("folderName")){
        LOG_ERROR_F("Examine - examineRaw: missing foldername parameter!");
        return {};
    }
    std::string folderName = params.at("folderName");
    LOG_DEBUG_F("Examine: {}", folderName);
    std::string cmd = std::format("{} {}", EXAMINE_COMMAND, folderName);
    return imap->sendCommand(cmd);
}

std::map<std::string, std::string> examineParse(const ImapResponse &result)
{
    std::map<std::string, std::string> ret;
    ret["UIDVALIDITY"] = getUidValidity(result.second);
    ret["UIDNEXT"] = getUidNext(result.second);
    ret["EXISTS"] = getExists(result.second);
    ret["RECENT"] = getRecent(result.second);
    return ret;
}
