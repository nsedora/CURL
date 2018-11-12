//
// Created by Buck Shlegeris on 12/11/16.
//

#ifdef __CYGWIN__
# include <sys/select.h> // for fd_set
#endif
#include <curl/curl.h>
#include <iostream>
#include <mutex>
#include "CurlWrapper.h"

static std::once_flag sCurlInitOnce;

void CurlWrapper::request(CurlResult *result, const std::string& uri, request_type_t request_type) {
    std::call_once(sCurlInitOnce, [](){ curl_global_init(CURL_GLOBAL_ALL); });

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10-second timeout

    result->response_code = 0;
    result->response_body.clear();

    if (request_type == GET) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result->response_body);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrapper::handle);
    } else {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result->response_code);
    curl_easy_cleanup(curl);
}

size_t CurlWrapper::handle(char *data, size_t size, size_t nmemb, std::string *output) {
    output->append(data, size * nmemb);
    return size * nmemb;
}
