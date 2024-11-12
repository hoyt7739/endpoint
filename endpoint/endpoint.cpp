#include "endpoint.h"
#include "bluetooth.h"
#include "tcp.h"
#include "websocket.h"

using namespace std::placeholders;

template<class data_t, class func_t>
bool full_trans(data_t buffer, int size, func_t func) {
    if (!buffer || size <= 0) {
        return false;
    }

    int count = 0;
    while (count < size) {
        int piece = func(buffer + count, size - count);
        if (piece <= 0) {
            return false;
        }

        count += piece;
    }

    return true;
}

endpoint* endpoint::create(endpoint_type type, const std::string& info) {
    switch (type) {
    case BLUETOOTH:
        return new bluetooth(info);
    case TCP:
        return new tcp(info);
    case WEBSOCKET:
        return new websocket(info);
    }

    return nullptr;
}

void endpoint::destroy(endpoint* ep) {
    delete ep;
}

bool endpoint::full_send(const char* buffer, int size) {
    return full_trans(buffer, size, std::bind(&endpoint::send, this, _1, _2));
}

bool endpoint::full_recv(char* buffer, int size) {
    return full_trans(buffer, size, std::bind(&endpoint::recv, this, _1, _2));
}
