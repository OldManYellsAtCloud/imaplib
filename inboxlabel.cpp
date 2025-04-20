#include "inboxlabel.h"
#include <loglib/loglib.h>
#include <regex>


namespace {

std::string extractFlagStringFromBrackets(const std::string &s){
    size_t flagStart = s.find("(");
    size_t flagEnd = s.find(")", flagStart);

    if (flagStart == std::string::npos || flagEnd == std::string::npos){
        LOG_ERROR_F("InboxLabel: Can't parse flags: {}", s);
        return "";
    }

    return s.substr(flagStart, flagEnd - flagStart);;
}

int getFlags(std::string s){
    int flags = 0;

    std::string flagString = extractFlagStringFromBrackets(s);

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

std::string extractName(std::string s){
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

std::string removeStatusReportFromEnd(const ImapResponse& ir){
    size_t end = ir.second.find(std::format("{} OK", ir.first));
    std::string ret = ir.second.substr(0, end);
    return ret;
}

std::vector<std::string> splitString(const std::string &s, const std::string &sep){
    size_t idx = 0, prev_idx = 0;
    std::vector<std::string> ret;

    do {
        idx = s.find(sep, prev_idx);
        std::string token = idx == std::string::npos ?
                           s.substr(prev_idx) :
                           s.substr(prev_idx, idx - prev_idx);

        prev_idx = idx + sep.length();
        if (token.empty())
            continue;

        ret.push_back(token);
    } while (idx != std::string::npos);

    return ret;
}
} // namespace

ImapResponse InboxLabels::inboxLabelRaw(ImapConnection *imap)
{
    ImapResponse ret = imap->sendCommand(INBOX_LABEL_COMMAND);
    return ret;
}


std::vector<InboxLabels::InboxLabel> InboxLabels::inboxLabelParse(ImapResponse response)
{
    std::vector<InboxLabels::InboxLabel> ret;

    if (!checkSuccess(response)) {
        LOG_ERROR_F("InboxLabel - Unsuccessful request: {}", response.second);
        return ret;
    }

    std::string responseWithoutStatus = removeStatusReportFromEnd(response);
    std::vector<std::string> responseLines = splitString(responseWithoutStatus, "\r\n");

    for (const std::string &line: responseLines){
        struct InboxLabel label {
            .name = extractName(line),
            .flags = getFlags(line)
        };
        ret.push_back(label);
    }

    return ret;
}
