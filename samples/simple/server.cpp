#include <future>
#include "common.h"

int main(int argc, const char* argv[]) {
    endpoint::endpoint_type type = endpoint::TCP;
    if (1 < argc) {
        type = (endpoint::endpoint_type)atoi(argv[1]);
    }

    std::string info;
    if (2 < argc) {
        info = argv[2];
    }

    endpoint* ep = endpoint::create(type, info);
    if (!ep) {
        std::cout << "create endpoint failed." << std::endl;
        return -1;
    }

    std::future<void> connect_future;
    bool res = ep->listen([ep, &connect_future] {
        std::cout << "client connected, " << ep->remote_info() << std::endl;
        connect_future = std::async(std::launch::async, &handle_recv, ep, true);
    });
    if (!res) {
        std::cout << "listen failed." << std::endl;
        return -1;
    }

    std::cout << "server started, " << ep->info() << std::endl;

    handle_send(ep, true);

    endpoint::destroy(ep);

    return 0;
}
