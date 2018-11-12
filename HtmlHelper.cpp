//
// Created by Buck Shlegeris on 11/21/16.
//

#include <sstream>
#include <regex>
#include <algorithm>
#include <iostream>
#include "HtmlHelper.h"

static void replace_all(std::string *str, const std::string &from, const std::string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str->find(from, start_pos)) != std::string::npos) {
        str->replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::string HtmlHelper::clean_up_href(const std::string& href) {
    std::string result = href;
    replace_all(&result, " ", "%20");
    return result;
}

std::string HtmlHelper::absolutize_path(const std::string& path, const std::string& base_path) {
    if (path == std::string("")) {
        return "";
    } else if (path[0] == '/' && path[1] != '.') {
        // That condition means "If the path starts with a '/' and its second character is not a '.'.
        // So it matches "/foo/bar" but not "foo/bar" or "/./foo/bar".

        return path;
    } else {
        std::vector<std::string> sections;
        {
            std::stringstream initial_path(base_path);
            std::string section;
            while (std::getline(initial_path, section, '/')) {
                if (section.length() > 0) {
                    sections.push_back(section);
                }
            }

            if (base_path[base_path.length() - 1] != '/') {
                sections.pop_back();
            }

            std::stringstream new_path(path);
            while (std::getline(new_path, section, '/')) {
                if (section.length() > 0) {
                    sections.push_back(section);
                }
            }
        }

        std::vector<std::string> out;
        for (const auto& section : sections) {
            if (section == "." || section == "") {
                // do nothing
            } else if (section == "..") {
                out.pop_back();
            } else {
                out.push_back(section);
            }
        }

        std::stringstream res;
        copy(out.begin(), out.end(), std::ostream_iterator<std::string>(res, "/"));
        std::string res_str = res.str();
        res_str.pop_back();

        return "/" + res_str;
    }
}

void HtmlHelper::get_neighbors(std::vector<Uri> *urls,
                               std::vector<std::string> *errors,
                               const std::string& body_str, const Uri& uri) {
    std::vector<std::string> paths_to_follow;
    get_url_strings_from_doc(&paths_to_follow, body_str);

    get_urls_from_page(urls, errors, paths_to_follow, uri);
}

void HtmlHelper::get_urls_from_page(
        std::vector<Uri> *urls,
        std::vector<std::string> *errors,
        const std::vector<std::string>& paths_to_follow,
        const Uri& parent_uri) {

    for (const auto& path : paths_to_follow) {
        std::string href = clean_up_href(path);

        if (href[0] == '#') {
            continue;
        }
        if (href.substr(0, 6) == std::string("mailto")) {
            continue;
        }

        Uri href_uri;
        try {
            href_uri = Uri(href);
        } catch (std::string e) {
            errors->push_back(e + " in url " + href);
            continue;
        }

        // default protocol is the parent's protocol
        if (href_uri.protocol().empty()) href_uri.set_protocol(parent_uri.protocol());

        if (href_uri.host().empty() && href_uri.path().substr(0, 2) == "//") {
          // For URLs without protocol, e.g. "//d12zodkr4t8o3m.cloudfront.net/nfavicon.ico"
          std::string::size_type find = href_uri.path().find('/', 2);
          href_uri.set_host(href_uri.path().substr(2, find - 2));
          href_uri.set_path(href_uri.path().substr(find));
        }
        if (href_uri.host().empty() || href_uri.host() == ".") {
            href_uri.set_host(parent_uri.host());
            href_uri.set_port(parent_uri.port());
            href_uri.set_path(absolutize_path(href_uri.path(), parent_uri.path()));
        }

        href_uri.set_fragment("");

        urls->push_back(href_uri);
    }
}

void HtmlHelper::get_url_strings_from_doc(std::vector<std::string> *out, const std::string& body_str) {
    std::vector<std::regex> regexes({
                    std::regex("<a [^>]*href=\"([^\"]*)"),
                    std::regex("<link [^>]*href=\"([^\"]*)\""),
                    std::regex("<script [^>]*src=\"([^\"]*)\"")});

    std::sregex_iterator end;
    for (const auto& regex : regexes) {
        std::sregex_iterator iter(body_str.cbegin(), body_str.cend(), regex);
        for (; iter != end; ++iter)
            out->push_back((*iter)[1]);
    }
}
