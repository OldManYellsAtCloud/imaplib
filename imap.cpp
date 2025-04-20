#include <loglib/loglib.h>
#include "imap.h"
#include "capability.h"
#include "authenticate.h"
#include "logout.h"
#include "inboxlabel.h"

Imap::Imap(const std::string &name, ImapConnection *imapConnection): name_{name},
                imapConnection_{imapConnection}
{
    init();
}

void Imap::init()
{
    imapRequestWithArgsDirectory[AUTHENTICATE] = authenticateRaw;
    parserDirectory[AUTHENTICATE] = authenticateParse;

    imapRequestDirectory[LOGOUT] = logoutRaw;
    parserDirectory[LOGOUT] = logoutParse;

    imapRequestDirectory[CAPABILITY] = capabilityRaw;
    parserDirectory[CAPABILITY] = capabilityParse;

    imapRequestDirectory[INBOX_LABEL] = InboxLabels::inboxLabelRaw;
    parserDirectory[INBOX_LABEL] = InboxLabels::inboxLabelParse;

    imapRequestDirectory[LIST] = imapRequestDirectory[INBOX_LABEL];
    parserDirectory[LIST] = parserDirectory[INBOX_LABEL];
}

std::variant<POSSIBLE_RESPONSE_TYPES> Imap::getResponse(REQUEST request,
                const std::optional<std::map<std::string, std::string> > &params)
{
    ImapResponse ir;
    if (params.has_value()){
        ir = imapRequestWithArgsDirectory[request](imapConnection_, params.value());
    } else {
        ir = imapRequestDirectory[request](imapConnection_);
    }

    std::variant<POSSIBLE_RESPONSE_TYPES> res = parserDirectory[request](ir);
    return res;

}

std::string Imap::getString(REQUEST request, const std::optional<std::map<std::string, std::string> > &params)
{
    std::string ret;
    auto result = getResponse(request, params);

    if (!std::holds_alternative<std::string>(result)){
        LOG_ERROR_F("Imap - GetString - Invalid request: {}", (int)request);
        return ret;
    }
    ret = std::get<std::string>(result);
    LOG_DEBUG_F("Imap - GetString - {}: {}", (int)request, ret);
    return ret;
}

std::vector<std::string> Imap::getStringVector(REQUEST request, const std::optional<std::map<std::string, std::string> > &params)
{
    std::vector<std::string> ret;
    auto result = getResponse(request, params);
    if (!std::holds_alternative<std::vector<std::string>>(result)){
        LOG_ERROR_F("Imap - GetStringVector - Invalid request: {}", (int)request);
        return ret;
    }
    ret = std::get<std::vector<std::string>>(result);

    for (const std::string &s: ret)
        LOG_DEBUG_F("Imap - GetStringVector - {}: {}", (int)request, s);

    return ret;
}

std::vector<InboxLabels::InboxLabel> Imap::getInboxLabelVector(REQUEST request, const std::optional<std::map<std::string, std::string> > &params)
{
    std::vector<InboxLabels::InboxLabel> ret;
    auto result = getResponse(request, params);
    if (!std::holds_alternative<std::vector<InboxLabels::InboxLabel>>(result)){
        LOG_ERROR_F("Imap - getInboxLabelVector - Invalid request: {}", (int)request);
        return ret;
    }
    ret = std::get<std::vector<InboxLabels::InboxLabel>>(result);

    for (const InboxLabels::InboxLabel &i: ret)
        LOG_DEBUG_F("Imap - getInboxLabelVector - {}: name: {}, flags: {}", (int)request, i.name, i.flags);

    return ret;
}


