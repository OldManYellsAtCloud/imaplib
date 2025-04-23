#include "fetch.h"
#include <format>
#include <loglib/loglib.h>

namespace {
    ImapResponse fetchMail(ImapConnection *imap, const std::string &id, const std::string &cmd) {
        //std::string finalCommand = std::format("{} {}", cmd, id);
        // FETCH 64705 BODY.PEEK[]
        // FETCH 64705 (BODY.PEEK[HEADER.FIELDS (DATE FROM TO SUBJECT CONTENT-TYPE)])
        std::string finalCommand = "FETCH 6943 (BODY.PEEK[HEADER.FIELDS (DATE FROM TO SUBJECT CONTENT-TYPE)] BODY.PEEK[TEXT])";
        return imap->sendCommand(finalCommand);
    }

    ImapResponse fetchMail(ImapConnection *imap, const int &id, const std::string &cmd) {
        return fetchMail(imap, std::to_string(id), cmd);
    }

    size_t getHeaderSentinelStart(const std::string &response){
        const std::string headerSentinel = "BODY[HEADER.FIELDS";
        return response.find(headerSentinel);
    }

    size_t getBodySentinelStart(const std::string &response){
        const std::string bodySentinel = "BODY[TEXT]";
        return response.find(bodySentinel);
    }

    size_t getHeaderStart(const std::string &response){
        size_t headerStart = getHeaderSentinelStart(response);
        if (headerStart == std::string::npos){
            LOG_WARNING("fetch - getHeaderStart - Can't find start of header");
            LOG_WARNING(response);
            return headerStart;
        }

        headerStart = response.find("\r\n", headerStart) + 2;
        LOG_DEBUG_F("fetch - getHeaderStart - headerStart: {}", headerStart);
        return headerStart;
    }

    size_t getBodyStart(const std::string &response){
        size_t bodyStart = getBodySentinelStart(response);
        if (bodyStart == std::string::npos){
            LOG_WARNING("fetch - getBodyStart - Can't find start of body");
            LOG_WARNING(response);
            return bodyStart;
        }

        bodyStart = response.find("\r\n", bodyStart) + 2;
        LOG_DEBUG_F("fetch - getBodyStart - bodyStart: {}", bodyStart);
        return bodyStart;
    }

    size_t getEndOfResponse(const std::string &response){
        size_t end = response.rfind("\r\n)");
        if (end != std::string::npos)
            end -= strlen("\r\n)");
        return end;
    }

    std::string getHeaders(const std::string &response){
        size_t headerStart = getHeaderStart(response);
        if (headerStart == std::string::npos){
            LOG_WARNING("fetch - getHeaders - Couldn't parse headers");
            LOG_WARNING(response);
            return "";
        }

        size_t headerEnd = response.find("\r\n\r\n", headerStart);
        return response.substr(headerStart, headerEnd - headerStart);
    }

    std::string getBody(const std::string &response){
        size_t bodyStart = getBodyStart(response);
        size_t headerSentinelStart = getHeaderSentinelStart(response);

        if (headerSentinelStart > bodyStart){
            return response.substr(bodyStart, headerSentinelStart - bodyStart);
        } else {
            size_t bodyEnd = getEndOfResponse(response);
            return response.substr(bodyStart, bodyEnd - bodyStart);
        }
    }
}


ImapResponse fetchMailRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("id")){
        LOG_ERROR_F("fetch - fetchMailRaw - missing id parameter");
        return ImapResponse{};
    }
    std::string id = params.at("id");
    return fetchMail(imap, id, FETCH_COMMAND);
}

ImapResponse fetchMailUidRaw(ImapConnection *imap, const std::map<std::string, std::string> &params)
{
    if (!params.contains("uid")){
        LOG_ERROR_F("fetch - fetchMailUidRaw - missing uid parameter");
        return ImapResponse{};
    }
    std::string uid = params.at("uid");
    return fetchMail(imap, uid, FETCH_UID_COMMAND);
}

std::map<std::string, std::string> fetchMailParser(const ImapResponse &response)
{
    std::map<std::string, std::string> ret;
    ret["headers"] = getHeaders(response.second);
    ret["body"] = getBody(response.second);
    return ret;
}
