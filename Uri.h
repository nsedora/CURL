//
// Created by Buck Shlegeris on 11/20/16.
//

#ifndef WEB_CRAWLER_URI_H
#define WEB_CRAWLER_URI_H


// modified from http://stackoverflow.com/a/11044337/1360429

#include <iostream> // for ostream
#include <string>

class Uri {
private:
    std::string protocol_, host_, port_, path_, query_string_, fragment_;

public:
    Uri() {}
    explicit Uri(const std::string &uri);

    void set_protocol(const std::string& protocol) { protocol_ = protocol; }
    void set_host(const std::string& host) { host_ = host; }
    void set_port(const std::string& port) { port_ = port; }
    void set_path(const std::string& path) { path_ = path; }
    void set_query_string(const std::string& query_string) { query_string_ = query_string; }
    void set_fragment(const std::string& fragment) { fragment_ = fragment; }

    const std::string& protocol() const { return protocol_; }
    const std::string& host() const { return host_; }
    const std::string& port() const { return port_; }
    const std::string& path() const { return path_; }
    const std::string& query_string() const { return query_string_; }
    const std::string& fragment() const { return fragment_; }

    std::string to_string() const;
    std::string to_verbose_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Uri& uri);
    friend bool operator<(const Uri& x, const Uri& y);
};

#endif //WEB_CRAWLER_URI_H
