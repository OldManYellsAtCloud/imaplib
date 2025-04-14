#ifndef CURLREQUEST_H
#define CURLREQUEST_H

#include <string>
#include <curl/curl.h>
#include "curlresponse.h"

struct RawResponse {
    CurlResponse header;
    CurlResponse body;
};

class CurlRequest
{
private:
    CURL *curl;
    CurlResponse header, body;
    std::string serverAddress_;
    CurlRequest();

public:
    static CurlRequest& getCurlRequest(){
        static CurlRequest cr;
        return cr;
    }
    ~CurlRequest();
    void prepareRequest(const std::string &extraUrl, const std::string &cmd);
    void configure(const std::string &serverAddress, const std::string &userName,
                const std::string &password);
    RawResponse performRequest();

};

#endif // CURLREQUEST_H
