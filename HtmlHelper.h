//
// Created by Buck Shlegeris on 11/21/16.
//

#ifndef WEB_CRAWLER_HTMLHELPER_H
#define WEB_CRAWLER_HTMLHELPER_H

#include <string>
#include <vector>
#include "Uri.h"

class HtmlHelper {
    static std::string clean_up_href(const std::string& href);

    static void get_urls_from_page(std::vector<Uri> *urls,
                                   std::vector<std::string> *errors,
                                   const std::vector<std::string>& paths_to_follow, const Uri& parent_uri);
    static void get_url_strings_from_doc(std::vector<std::string> *urls, const std::string& body_str);

public:
    static void get_neighbors(std::vector<Uri> *urls, std::vector<std::string> *errors,
                              const std::string& body_str, const Uri& uri);

    static std::string absolutize_path(const std::string& path, const std::string& base_path);
};


#endif //WEB_CRAWLER_HTMLHELPER_H
