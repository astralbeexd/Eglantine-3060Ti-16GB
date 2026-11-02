#include <atomic>
#include <iostream>
#include <thread>
#include "common/httplib.h"

int main() {
    httplib::Server target;
    httplib::Server origin;
    std::atomic<bool> cookie_leaked{false};
    std::atomic<bool> header_injected{false};
    target.Get(".*", [&](const httplib::Request& request, httplib::Response& response) {
        if (request.has_header("Cookie")) cookie_leaked.store(true);
        if (request.has_header("X-Injected")) header_injected.store(true);
        response.set_content("ok", "text/plain");
    });
    const int target_port = target.bind_to_any_port("127.0.0.1");
    if (target_port <= 0) return 2;
    origin.Get("/redirect", [&](const httplib::Request&, httplib::Response& response) {
        response.set_redirect("http://localhost:" + std::to_string(target_port) + "/final");
    });
    const int origin_port = origin.bind_to_any_port("127.0.0.1");
    if (origin_port <= 0) return 3;
    std::thread target_thread([&] { target.listen_after_bind(); });
    std::thread origin_thread([&] { origin.listen_after_bind(); });
    target.wait_until_ready();
    origin.wait_until_ready();
    bool redirect_ok = false;
    {
        httplib::Client client("127.0.0.1", origin_port);
        client.set_address_family(AF_INET);
        client.set_connection_timeout(2);
        client.set_read_timeout(2);
        client.set_follow_location(true);
        client.set_default_headers({{"Cookie", "security_test=synthetic_only"}});
        auto response = client.Get("/redirect");
        redirect_ok = response && response->status == 200;
        if (!response) std::cerr << "HTTP transport: " << httplib::to_string(response.error()) << '\n';
    }
    {
        httplib::Client client("127.0.0.1", target_port);
        client.set_address_family(AF_INET);
        client.set_connection_timeout(2);
        client.set_read_timeout(2);
        auto stream = client.open_stream("GET", "/safe HTTP/1.1\r\nX-Injected: yes\r\nX-Tail:");
    }
    origin.stop();
    target.stop();
    origin_thread.join();
    target_thread.join();
    if (!redirect_ok || cookie_leaked || header_injected) {
        std::cerr << "FAIL: redirect=" << redirect_ok << " cookie=" << cookie_leaked
                  << " injected=" << header_injected << '\n';
        return 1;
    }
    std::cout << "PASS: cross-origin cookie stripped; streamed path cannot inject headers\n";
}
