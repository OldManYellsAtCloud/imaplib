#include "curlrequest.h"
#include <loglib/loglib.h>

static size_t storeCurlData(char *ptr, size_t size, size_t nmemb, void *userdata){
    CurlResponse *cr = (CurlResponse*)userdata;
    std::string s(ptr, nmemb);
    return cr->storeResponse(ptr, nmemb);
}

CurlRequest::CurlRequest() {
    curl = curl_easy_init();
}


CurlRequest::~CurlRequest()
{
    curl_easy_cleanup(curl);
}

void CurlRequest::prepareRequest(const std::string &extraUrl, const std::string &cmd)
{
    if (serverAddress_.empty()){
        LOG_ERROR("Curl: server address empty! Did you call configure?");
        return;
    }

    header.reset();
    body.reset();
    std::string finalUrl = std::format("{}/{}", serverAddress_, extraUrl);
    LOG_INFO_F("Configuring request: {}, command: {}", finalUrl, cmd);
    curl_easy_setopt(curl, CURLOPT_URL, finalUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeCurlData);

    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, storeCurlData);

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmd.empty() ? NULL : cmd.c_str());
}

void CurlRequest::configure(const std::string &serverAddress, const std::string &userName, const std::string &password)
{
    LOG_INFO_F("Configuring Curl: {}", serverAddress);
    serverAddress_ = serverAddress;
    curl_easy_setopt(curl, CURLOPT_USERNAME, userName.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
}

RawResponse CurlRequest::performRequest()
{
    auto res = curl_easy_perform(curl);

    LOG_INFO_F("Curl request res: {}", curl_easy_strerror(res));
    LOG_DEBUG_F("Headers: {}", header.getResponse());
    LOG_DEBUG_F("Body: {}", body.getResponse());

    struct RawResponse r {
        .header = header,
        .body = body
    };
    return r;
}

