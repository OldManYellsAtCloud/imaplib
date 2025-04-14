#include "inboxlabel.h"
#include <loglib/loglib.h>
#include <regex>


InboxLabels::InboxLabels()
{
    command = "LIST \"/\" \"*\"";
}

std::pair<std::string, std::string> InboxLabels::rawPerform(ImapConnection *imap)
{
    std::pair<std::string, std::string> ret = imap->sendCommand(command);
    return ret;
}

int getFlags(std::string s){
    int flags = 0;

    size_t flagStart = s.find("(");
    size_t flagEnd = s.find(")", flagStart);

    if (flagStart == std::string::npos || flagEnd == std::string::npos){
        LOG_ERROR_F("InboxLabel: Can't parse flags: {}", s);
        return flags;
    }

    std::string flagString = s.substr(flagStart, flagEnd - flagStart);

    if (flagString.find("\\HasNoChildren") != std::string::npos)
        flags |= INBOX_LABEL_HAS_NO_CHILDREN;
    if (flagString.find("\\HasChildren") != std::string::npos)
        flags |= INBOX_LABEL_HAS_CHILDREN;
    if (flagString.find("\\Noselect") != std::string::npos)
        flags |= INBOX_LABEL_NO_SELECT;
    if (flagString.find("\\Trash") != std::string::npos)
        flags |= INBOX_LABEL_TRASH;
    if (flagString.find("\\Drafts") != std::string::npos)
        flags |= INBOX_LABEL_DRAFTS;
    if (flagString.find("\\Important") != std::string::npos)
        flags |= INBOX_LABEL_IMPORTANT;
    if (flagString.find("\\Sent") != std::string::npos)
        flags |= INBOX_LABEL_SENT;
    if (flagString.find("\\Junk") != std::string::npos)
        flags |= INBOX_LABEL_JUNK;
    if (flagString.find("\\Flagged") != std::string::npos)
        flags |= INBOX_LABEL_FLAGGED;

    if (flags == 0){
        LOG_WARNING_F("InboxLabel: Could not find any usable flags: {}", s);
    }

    return flags;
}

std::string getName(std::string s){
    std::regex nameRegex("([^\"]*)\"$");
    std::smatch nameMatch;
    std::string ret = "ERROR";
    if (!std::regex_search(s, nameMatch, nameRegex)){
        LOG_ERROR_F("InboxLabel: Can't extract label name: x{}x", s);
        return ret;
    }
    ret = nameMatch[1].str();
    LOG_DEBUG_F("InboxLabel: Extracted labelname: {}", ret);
    return ret;
}

std::string InboxLabels::cleanResponse(const std::string& response, size_t successStart) {
    std::string ret = response.substr(0, successStart);
    return ret;
}

std::vector<InboxLabel> InboxLabels::perform(ImapConnection *imap)
{
    std::pair<std::string, std::string> r = rawPerform(imap);
    std::vector<InboxLabel> ret;

    if (r.second.empty()) {
        LOG_ERROR("InboxLabel - Could not retrieve any labels");
        return ret;
    }

    size_t successStart = checkSuccess(r);

    if (successStart == std::string::npos) {
        LOG_ERROR_F("InboxLabel - Unsuccessful request: {}", r.second);
        return ret;
    }

    std::string cleanedResponse = cleanResponse(r.second, successStart);
    LOG_INFO_F("Cleaned response: {}", cleanedResponse);
    size_t idx = 0, prev_idx = 0;

    do {
        idx = cleanedResponse.find("\r\n", prev_idx);
        std::string line = idx == std::string::npos ?
                           cleanedResponse.substr(prev_idx) :
                           cleanedResponse.substr(prev_idx, idx - prev_idx);

        prev_idx = idx + 2;
        if (line.empty())
            continue;

        struct InboxLabel label {
            .name = getName(line),
            .flags = getFlags(line)
        };
        ret.push_back(label);
    } while (idx != std::string::npos);

    return ret;
}
