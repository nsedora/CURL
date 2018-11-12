//
// Created by Buck Shlegeris on 11/20/16.
//

#include <algorithm>
#include <memory>
#include <vector>
#include "Graph.h"

static const std::set<Uri> kEmptySet;

bool Graph::add_node(const Uri& uri) {
    auto insert = nodes_.insert(std::make_pair(uri, std::shared_ptr<PageNode>()));
    if (insert.second) {
        insert.first->second = std::make_shared<PageNode>(uri);
        insert.first->second->set_node_status(ENQUEUED);
    }
    return insert.second;
}

void Graph::add_neighbor(const Uri& from_uri, const Uri& to_uri) {
    outgoing_links_[from_uri].insert(to_uri);
    incoming_links_[to_uri].insert(from_uri);
}

const std::set<Uri>& Graph::parents(const Uri& uri) const {
    auto find = incoming_links_.find(uri);
    return (find == incoming_links_.end()) ? kEmptySet : find->second;
}

std::shared_ptr<PageNode> Graph::operator[](const Uri& uri) {
    auto find = nodes_.find(uri);
    return (find == nodes_.end()) ? std::make_shared<PageNode>(uri) : find->second;
}

std::shared_ptr<PageNode> Graph::operator[](const std::string& uri) {
    return (*this)[Uri(uri)];
}
