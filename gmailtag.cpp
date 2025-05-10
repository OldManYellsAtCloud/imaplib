#include "gmailtag.h"
#include <loglib/loglib.h>
#include <regex>

namespace {
    ImapResponse fetchGmailTags(ImapConnection *imap, const std::string &id, const std::string &cmd){
        std::string finalCommand = std::vformat(std::string_view(cmd), std::make_format_args(id));
        return imap->sendCommand(finalCommand);
    }

    std::string extractTag(const std::string &s){
        std::string ret;

        std::regex tagRegex("X-GM-LABELS \(([^\)]*)\)");
        std::smatch tagMatch;
        if (!std::regex_search(s, tagMatch, tagRegex)) {
            LOG_ERROR_F("gmailTag - extractTag - Can't extract tag: {}", s);
            return ret;
        }
        ret = tagMatch[1].str();
        LOG_DEBUG_F("gmailTag - extractTag - extracted tag: {}", ret);
        return ret;
    }
}


ImapResponse gmailTagsRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("id")){
        LOG_ERROR_F("GmailTag - gmailTagsRaw - Missing id parameter!");
        return {};
    }

    std::string id = params.at("id");
    return fetchGmailTags(imap, id, GMAIL_TAG_COMMAND);
}



ImapResponse gmailTagsUidRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("uid")){
        LOG_ERROR_F("GmailTag - gmailTagsRawUid - Missing uid parameter!");
        return {};
    }

    std::string uid = params.at("uid");
    return fetchGmailTags(imap, uid, GMAIL_TAG_UID_COMMAND);
}

std::string gmailTagsParse(const ImapResponse &response)
{
    std::string ret;
    if (!checkSuccess(response)){
        LOG_ERROR_F("gmailTag - Unsuccessful request: {}", response.second);
        return ret;
    }

    return extractTag(response.second);
}
