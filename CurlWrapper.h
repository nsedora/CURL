//
// Created by Buck Shlegeris on 12/11/16.
//

#ifndef WEB_CRAWLER_CURLWRAPPER_H
#define WEB_CRAWLER_CURLWRAPPER_H

#include <string>
#include "PageNode.h"

struct CurlResult {
    long response_code;
    std::string response_body;
};

class CurlWrapper {
    static size_t handle(char *data, size_t size, size_t nmemb, std::string *output);

public:
    static void request(CurlResult *result, const std::string& uri, request_type_t request_type);
};

#endif //WEB_CRAWLER_CURLWRAPPER_H
