//
// Created by Buck Shlegeris on 12/3/16.
//

#ifndef WEB_CRAWLER_CRAWLER_H
#define WEB_CRAWLER_CRAWLER_H

#include <deque>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include "Graph.h"

class Crawler {
    int num_threads_;
    int max_threads_;
    bool verbosity_;

    Uri domain_;

    std::mutex mutex_;
    std::deque<Uri> queue_;
    std::set<Uri> currently_being_explored_;
    std::vector<std::string> errors_;
    Graph graph_;

public:
    Crawler(int max_threads);
    Graph& crawl(const std::string& initialUrlString, bool displayResults);

    void set_verbosity(bool verbosity);

private:
    Crawler(const Crawler&) = delete;
    Crawler& operator=(const Crawler&) = delete;

    void enqueue(const Uri&  uri);
    void spawn_crawling_thread(const Uri& uri);

    bool uri_should_be_crawled_as_node(const Uri& uri);

    void crawl_with_get_request(Uri uri);
    void crawl_with_head_request(Uri uri);
    void note_error(const std::string& err);

    void finalize_crawl(const Uri& uri);
};

#endif //WEB_CRAWLER_CRAWLER_H
