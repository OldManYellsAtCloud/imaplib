#ifndef CURLRESPONSE_H
#define CURLRESPONSE_H

#include <string>
#include <list>

class CurlResponse
{
    std::string r;
    std::list<char> l;
    size_t length;
public:
    CurlResponse();
    ~CurlResponse(){}
    size_t storeResponse(char* ptr, size_t nmemb);
    std::string getResponse();
    size_t getResponseSize();
    void reset();
};

#endif // CURLRESPONSE_H
