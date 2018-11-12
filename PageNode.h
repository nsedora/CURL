//
// Created by Buck Shlegeris on 11/20/16.
//

#ifndef WEB_CRAWLER_PAGENODE_H
#define WEB_CRAWLER_PAGENODE_H

#include "Uri.h"

enum node_status_t {
    NONE, ENQUEUED, SUCCESS, FAILURE
};

enum request_type_t {
    GET, HEAD
};

class PageNode {
    Uri uri_;
    request_type_t request_type_;
    node_status_t node_status_;
    std::string error_;
    int response_code_;

public:
    PageNode(const Uri& uri) : uri_(uri), node_status_(NONE) {}

    const Uri& uri() const { return uri_; }
    request_type_t request_type() const { return request_type_; }
    node_status_t node_status() const { return node_status_; }
    const std::string& error() const { return error_; }
    int response_code() const { return response_code_; }

    void set_request_type(request_type_t request_type) { request_type_ = request_type; }
    void set_node_status(node_status_t node_status) { node_status_ = node_status; }
    void set_error(const std::string& error) { error_ = error; }
    void set_response_code(int response_code) { response_code_ = response_code; }
};

#endif //WEB_CRAWLER_PAGENODE_H
