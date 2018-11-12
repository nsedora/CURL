//
// Created by Buck Shlegeris on 11/20/16.
//

#include <algorithm>    // for std::find
#include <regex>
#include <sstream>      // for std::stringstream
#include <tuple>        // for std::tie
#include "Uri.h"

static const std::regex kSectionRegex("^[a-zA-Z0-9\\._~!$&'()*+,;=:\\/@\\-?#]*$");

bool operator<(const Uri& x, const Uri& y) {
    return std::tie(x.query_string(), x.path(), x.protocol(), x.host(), x.port(), x.fragment())
         < std::tie(y.query_string(), y.path(), y.protocol(), y.host(), y.port(), y.fragment());
}

std::ostream& operator<<(std::ostream& os, const Uri& uri)
{
    os << uri.protocol_ << "://" << uri.host_;
    if (!uri.port_.empty())
      os << ":" << uri.port_;
    os << uri.path_;
    if (!uri.query_string_.empty())
      os << "?" << uri.query_string_;
    if (!uri.fragment_.empty())
      os << "#" << uri.fragment_;
    return os;
}

Uri::Uri(const std::string &uri) {
    typedef std::string::const_iterator iterator_t;

    if (uri.empty())
      return;

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

    // get fragment start
    iterator_t fragmentStart = std::find(uri.begin(), uriEnd, '#');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');            //"://");

    iterator_t pathStart;

    if (protocolEnd != uriEnd)
    {
        std::string prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == "://")) {
            protocol_.assign(protocolStart, protocolEnd);
            protocolEnd += 3;   //      ://
        } else {
            protocolEnd = uri.begin();  // no protocol
        }

        // host
        iterator_t hostStart = protocolEnd;
        pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

        iterator_t hostEnd = std::find(protocolEnd,
                                       (pathStart != uriEnd) ? pathStart : queryStart,
                                       ':');  // check for port

        host_.assign(hostStart, hostEnd);

        if (host_ == ".") {
            host_ = "";
            pathStart++;
        }

        // port
        if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
        {
            hostEnd++;
            iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
            port_.assign(hostEnd, portEnd);
        }
    }
    else {
        protocol_ = "";
        host_ = "";
        port_ = "";

        pathStart = uri.begin();
    }

    if (queryStart > fragmentStart) {
        queryStart = fragmentStart;
    }

    // path
    if (pathStart != uriEnd) {
        path_.assign(pathStart, queryStart);
    }

    // query
    if (queryStart != uriEnd && queryStart != fragmentStart) {
        query_string_.assign(queryStart + 1, fragmentStart);
    }

    if (fragmentStart != uriEnd) {
        fragment_.assign(fragmentStart + 1, uri.end());
    }

    if (!std::regex_match(host_, kSectionRegex)) {
        throw std::string("Invalid character in host (" + host_ + ")");
    } else if (!std::regex_match(path_, kSectionRegex)) {
        throw std::string("Invalid character in path (" + path_ + ")");
    } else if (!std::regex_match(query_string_, kSectionRegex)) {
        throw std::string("Invalid character in query string (" + query_string_ + ")");
    } else if (!std::regex_match(fragment_, kSectionRegex)) {
        throw std::string("Invalid character in fragment (" + fragment_ + ")");
    }
}

std::string Uri::to_string() const {
    std::stringstream s;
    s << *this;
    return s.str();
}

std::string Uri::to_verbose_string() const {
    std::stringstream s;
    s << "protocol: " << protocol_ << ", host: " << host_ << ", port: " << port_ << ", path: " << path_ << ", query_string: " << query_string_ << ", fragment: " << fragment_;
    return s.str();
}
