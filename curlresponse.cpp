#include "curlresponse.h"

CurlResponse::CurlResponse() {}

size_t CurlResponse::storeResponse(char *ptr, size_t nmemb)
{
    r.append(ptr, nmemb);
    length += nmemb;
    return nmemb;
}

std::string CurlResponse::getResponse()
{
    return r;
}

size_t CurlResponse::getResponseSize()
{
    return length;
}

void CurlResponse::reset()
{
    length = 0;
    r = "";
}
