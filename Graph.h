//
// Created by Buck Shlegeris on 11/20/16.
//

#ifndef WEB_CRAWLER_GRAPH_H
#define WEB_CRAWLER_GRAPH_H

#include <string>
#include <map>
#include <set>
#include <memory>
#include "PageNode.h"

class Graph {
public:
    // Public for testing
    std::map<Uri, std::shared_ptr<PageNode> > nodes_;
    std::map<Uri, std::set<Uri> > outgoing_links_;
    std::map<Uri, std::set<Uri> > incoming_links_;

    Graph() = default;
    ~Graph() = default;

    bool add_node(const Uri& uri);
    std::shared_ptr<PageNode> operator[](const Uri& uri);
    std::shared_ptr<PageNode> operator[](const std::string& uri);
    void add_neighbor(const Uri& from_uri, const Uri& to_uri);

    const std::set<Uri>& parents(const Uri& uri) const;
    unsigned long size() const { return nodes_.size(); }
};


#endif //WEB_CRAWLER_GRAPH_H
