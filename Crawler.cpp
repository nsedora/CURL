//
// Created by Buck Shlegeris on 12/3/16.
//

#include "Crawler.h"
#include "HtmlHelper.h"
#include "CurlWrapper.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>

Crawler::Crawler(int max_threads) : num_threads_(0), max_threads_(max_threads), verbosity_(false) {}

Graph& Crawler::crawl(const std::string& initialUrlString, bool display_results) {
    Uri initial_uri = Uri(initialUrlString);

    domain_ = initial_uri;
    domain_.set_path("");
    domain_.set_query_string("");
    domain_.set_fragment("");

    {
        // std::lock_guard acquires the lock in its constructor, and releases it in the destructor (RAII pattern).
        // This is a convenient and safe way to hold a lock for the duration of a scoped block.
        std::lock_guard<std::mutex> lock(mutex_);
        enqueue(initial_uri);
    }

    for (;;) {
        size_t queue_size, currently_being_explored_size;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_size = queue_.size();
            currently_being_explored_size = currently_being_explored_.size();
        }
        if (!queue_size && !currently_being_explored_size)
            break;
        if (verbosity_)
            std::cout << "Crawling, queue has length " << queue_size
                      << ", currently_being_explored has size " << currently_being_explored_size << "\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (display_results) {
        std::cout << "\n\nDone crawling! We explored " << graph_.size() << " urls!\n\n";

        if (errors_.empty()) {
            std::cout << "No errors found!\n\n";
        } else {
            std::cout << "Here are all the complaints found:\n\n";
            for (const auto& error : errors_)
                std::cout << error << "\n";
            std::cout << "\n";
        }
    }

    return graph_;
}

// Must be called with `mutex_` held.
void Crawler::enqueue(const Uri& uri) {
    if (!graph_.add_node(uri))
        return;

    std::cout << "Enqueueing " << uri << " (queue has size " << queue_.size() << ")\n";
    if (num_threads_ < max_threads_) {
        num_threads_++;
        spawn_crawling_thread(uri);
    } else {
        queue_.push_back(uri);
    }
}

// Must be called with `mutex_` held.
void Crawler::spawn_crawling_thread(const Uri& uri) {
    currently_being_explored_.insert(uri);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::thread t(uri_should_be_crawled_as_node(uri) ?
                  &Crawler::crawl_with_get_request :
                  &Crawler::crawl_with_head_request, this, uri);
    t.detach();
}

bool Crawler::uri_should_be_crawled_as_node(const Uri& uri) {
    if (domain_.host() != uri.host()) {
        return false;
    }

    // prevent you from starting to crawl FTP if you're looking at HTTP
    if (domain_.protocol() != uri.protocol()) {
        return false;
    }

    std::set<std::string> filetype_list = {"pdf", "jpg", "gif", "js", "css", "png"};

    std::string::size_type i = uri.path().find_last_of('.');
    if (i > 0) {
        std::string extension = uri.path().substr(i+1, uri.path().length());

        if (filetype_list.find(extension) != filetype_list.end())
            return false;
    }

    return true;
}

void Crawler::crawl_with_get_request(Uri uri) {
    std::shared_ptr<PageNode> node;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        node = graph_[uri];
    }
    node->set_request_type(GET);

    CurlResult result;
    CurlWrapper::request(&result, uri.to_string(), GET);
    long http_code = result.response_code;

    std::cout << "GET " << uri << ": " << http_code << "\n";

    if (http_code >= 400 || http_code == 0) {
        Uri parent_uri;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (Uri uri_ : graph_.parents(uri)) {
                parent_uri = uri_; break;
            }
        }

        std::stringstream msg;
        msg << "When crawling " << uri << ", got a " << http_code << " (linked from " << parent_uri << ")";
        note_error(msg.str());

        node->set_node_status(FAILURE);
    } else {
        // todo: require that the response type was text/something

        std::vector<Uri> neighbors;
        HtmlHelper::get_neighbors(&neighbors, &errors_, result.response_body, uri);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& neighbor_uri : neighbors) {
                graph_.add_neighbor(uri, neighbor_uri);
                enqueue(neighbor_uri);
            }
        }
        node->set_node_status(SUCCESS);
    }

    finalize_crawl(uri);
}

void Crawler::crawl_with_head_request(Uri uri) {
    std::shared_ptr<PageNode> node;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        node = graph_[uri];
    }
    node->set_request_type(HEAD);

    CurlResult result;
    CurlWrapper::request(&result, uri.to_string(), HEAD);
    long http_code = result.response_code;

    std::cout << "HEAD " << uri << ": " << http_code << "\n";

    if (http_code >= 400 || http_code == 0) {
        Uri parent_uri;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (Uri uri_ : graph_.parents(uri)) {
                parent_uri = uri_; break;
            }
        }

        std::stringstream msg;
        msg << "When crawling " << uri << ", got a " << http_code << " (linked from " << parent_uri << ")";
        note_error(msg.str());

        node->set_node_status(FAILURE);
    } else {

        node->set_node_status(SUCCESS);
    }
    finalize_crawl(uri);
}

void Crawler::finalize_crawl(const Uri& uri) {
    std::cout << "finalizing " << uri << "\n";

    std::lock_guard<std::mutex> lock(mutex_);
    currently_being_explored_.erase(uri);

    if (queue_.empty()) {
        num_threads_--;
    } else {
        Uri queue_url = queue_.front();
        queue_.pop_front();

        spawn_crawling_thread(queue_url);
    }
}

void Crawler::note_error(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    errors_.push_back(error);
}

void Crawler::set_verbosity(bool verbosity) {
    verbosity_ = verbosity;
}
