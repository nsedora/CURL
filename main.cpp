#include <iostream>
#include <string>
#include "Crawler.h"
#include "HtmlHelper.h"
#include "CurlWrapper.h"

using namespace std;

void already_passing_tests();
void failing_test_1();
void failing_test_2();
void failing_test_3();
void failing_test_4();

int main(int argc, char *argv[])
{
    //if (argc != 2) {
    //    std::cout << "wrong number of arguments, usage: ./crawler TEST_NUMBER" << std::endl;
    //    return 1;
    //}

    int command = 4;

    switch (command) {
        case 0: already_passing_tests(); break;
        case 1: failing_test_1(); break;
        case 2: failing_test_2(); break;
        case 3: failing_test_3(); break;
        case 4: failing_test_4(); break;
        default: std::cout << "That command is not valid; commands have to be in {0, 1, 2, 3, 4}\n";
    }

    return 0;
}

void tbassert(bool assertion, const std::string& msg) {
    std::cerr << (assertion ? "Test passed: " : "Test failed: ") << msg << std::endl;
}

void already_passing_tests() {
    const std::string url1 = "https://www.forbes.com/sites/christophersteiner/2016/09/29/how-to-hire-better-engineers-ignore-school-degrees-and-past-projects/#ceda3f8360bf";
    tbassert(Uri(url1).protocol() == "https", "URI protocol parsing works");
    tbassert(Uri(url1).host() == "www.forbes.com", "URI host parsing works");
    tbassert(Uri(url1).port() == "", "URI port parsing works");
    tbassert(Uri(url1).path() == "/sites/christophersteiner/2016/09/29/how-to-hire-better-engineers-ignore-school-degrees-and-past-projects/", "URI path parsing works");
    tbassert(Uri(url1).query_string() == "", "URI query parsing works");
    tbassert(Uri(url1).fragment() == "ceda3f8360bf", "URI fragment parsing works");

    tbassert(HtmlHelper::absolutize_path("./page3", "/web-crawler-test-site/test4/cynical.html") == "/web-crawler-test-site/test4/page3",
           "absolutize is returning the right thing");
    tbassert(Uri("SVG_logo.svg").path() == "SVG_logo.svg", "parsing works");
    tbassert(Uri("./SVG_logo.svg").path() == "./SVG_logo.svg", "parsing works");
    tbassert(HtmlHelper::absolutize_path("./SVG_logo.svg", "/") == std::string("/SVG_logo.svg"),
        "absolutize path is returning the right thing"
    );

    Crawler crawler(5);
    Graph& graph = crawler.crawl("http://triplebyte.github.io/web-crawler-test-site/already-passing-tests/", true);

    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/already-passing-tests/page2"]->node_status() == SUCCESS, "Crawler can find main page");
    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/already-passing-tests/page2-real"]->node_status() == SUCCESS, "Crawler can find linked page");
    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/already-passing-tests/page2-fake"]->node_status() == FAILURE, "Crawler can notice nonexistent linked page");
}

void failing_test_1() {
    Crawler crawler(10);
    Graph& graph = crawler.crawl("http://triplebyte.github.io/web-crawler-test-site/test1/", true);

    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/test1/SVG_logo.svg"]->request_type() == HEAD,
           "Expected url to be crawled with HEAD request");
}

void failing_test_2() {
    Crawler crawler(10);
    Graph& graph = crawler.crawl("http://triplebyte.github.io/web-crawler-test-site/test2/", true);

    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/test2/page2.html"]->node_status() == SUCCESS,
           "Crawler correctly follows some links");
}

void failing_test_3() {
    Crawler crawler(5);
    Graph& graph = crawler.crawl("http://triplebyte.github.io/web-crawler-test-site/test3/", true);

    // The bug here is that the crawler will hang.
    // Don't sit around waiting for it to finish.

    tbassert(graph["http://blah.com:7091"]->node_status() != NONE,
           "Crawler does not hang on a weird page");
}

void failing_test_4() {
    Crawler crawler(5);
    Graph& graph = crawler.crawl("http://triplebyte.github.io/web-crawler-test-site/test4/", true);

    for (const auto& node : graph.nodes_) {
        std::cout << node.first << std::endl;
    }

    tbassert(graph["http://triplebyte.github.io/web-crawler-test-site/test4/page3"]->node_status() == SUCCESS,
           "Crawler successfully crawls another weird page");
}
