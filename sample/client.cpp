#include <future>
#include "common.h"

int main(int argc, const char* argv[]) {
    endpoint::endpoint_type type = endpoint::TCP;
    if (1 < argc) {
        type = (endpoint::endpoint_type)atoi(argv[1]);
    }

    std::string remote_info;
    if (2 < argc) {
        remote_info = argv[2];
    }

    endpoint* ep = endpoint::create(type);
    if (!ep) {
        std::cout << "create endpoint failed." << std::endl;
        return -1;
    }

    std::future<void> connect_future;
    bool res = ep->connect(remote_info, [ep, &connect_future] {
        std::cout << "connected to server, " << ep->remote_info() << std::endl;
        connect_future = std::async(std::launch::async, &handle_recv, ep, true);
    });
    if (!res) {
        std::cout << "connect failed." << std::endl;
        return -1;
    }

    handle_send(ep, true);

    endpoint::destroy(ep);

    return 0;
}
