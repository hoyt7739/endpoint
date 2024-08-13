#include "tcp.h"
#include "socket.h"
#include <cstring>
#include <future>
#include <vector>
#if defined K_WINDOWS
#include <iphlpapi.h>
#elif defined K_LINUX
#include <ifaddrs.h>
#include <net/if.h>
#endif

const unsigned short DEFAULT_PORT = 28800;
const int BROADCAST_INTERVAL = 10;

struct tcp_data {
    socket_t listen_socket = INVALID_SOCKET;
    socket_t connect_socket = INVALID_SOCKET;
    std::future<void> listen_future;
    std::future<void> broadcast_future;
    std::condition_variable_any m_cond_closed;
};

static void fill_sockaddr(sockaddr_in& addr, const std::string& info) {
    addr.sin_family = AF_INET;
    size_t delim_pos = info.find(':');
    if (std::string::npos != delim_pos) {
        addr.sin_port = htons(std::stoi(info.substr(delim_pos + 1)));
        std::string ip(info.substr(0, delim_pos));
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    } else {
        addr.sin_port = htons(DEFAULT_PORT);
        inet_pton(AF_INET, info.c_str(), &addr.sin_addr);
    }
}

static std::string get_info(const sockaddr_in& addr) {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, buffer, sizeof(buffer));
    return buffer + (':' + std::to_string(ntohs(addr.sin_port)));
}

tcp::tcp(const std::string& info) : endpoint(info), m_data(new tcp_data) {
    socket_data::init();
}

tcp::~tcp() {
    close();
    delete m_data;
}

bool tcp::listen(connected_notify notify) {
    m_data->listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == m_data->listen_socket) {
        close();
        return false;
    }

    sockaddr_in local_addr = {0};
    fill_sockaddr(local_addr, m_info);
    if (0 != bind(m_data->listen_socket, (sockaddr*)&local_addr, sizeof(sockaddr_in))) {
        close();
        return false;
    };

    if (0 != ::listen(m_data->listen_socket, SOMAXCONN)) {
        close();
        return false;
    }

    m_info = get_info(local_addr);

    m_data->listen_future = std::async(std::launch::async, [this, notify] {
        sockaddr_in remote_addr = {0};
        socklen_t addr_len = sizeof(sockaddr_in);
        while (true) {
            socket_t connect_socket = accept(m_data->listen_socket, (sockaddr*)&remote_addr, &addr_len);
            if (INVALID_SOCKET == connect_socket) {
                break;
            }

            if (is_connected()) {
                closesocket(connect_socket);
                continue;
            }

            m_data->connect_socket = connect_socket;
            m_remote_info = get_info(remote_addr);
            if (notify) {
                notify();
            }
        }
    });

    if (INADDR_LOOPBACK != local_addr.sin_addr.s_addr) {
        m_data->broadcast_future = std::async(std::launch::async, &tcp::broadcast, this);
    }

    return true;
}

bool tcp::connect(const std::string& remote_info, connected_notify notify) {
    m_data->connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == m_data->connect_socket) {
        disconnect();
        return false;
    }

    sockaddr_in remote_addr = {0};
    fill_sockaddr(remote_addr, remote_info);
    if (0 != ::connect(m_data->connect_socket, (sockaddr*)&remote_addr, sizeof(sockaddr_in))) {
        disconnect();
        return false;
    }

    sockaddr_in local_addr = {0};
    socklen_t addr_len = sizeof(sockaddr_in);
    if (0 != getsockname(m_data->connect_socket, (sockaddr*)&local_addr, &addr_len)) {
        disconnect();
        return false;
    }

    m_info = get_info(local_addr);
    m_remote_info = get_info(remote_addr);

    if (notify) {
        notify();
    }

    return true;
}

bool tcp::is_listening() const {
    return INVALID_SOCKET != m_data->listen_socket;
}

bool tcp::is_connected() const {
    return INVALID_SOCKET != m_data->connect_socket;
}

void tcp::disconnect() {
    if (is_connected()) {
        closesocket(m_data->connect_socket);
        m_data->connect_socket = INVALID_SOCKET;
    }
}

void tcp::close() {
    disconnect();

    if (is_listening()) {
        closesocket(m_data->listen_socket);
        m_data->listen_socket = INVALID_SOCKET;
    }

    m_data->m_cond_closed.notify_all();
}

int tcp::send(const char* buffer, int size) {
    if (!is_connected()) {
        return 0;
    }

    return ::send(m_data->connect_socket, buffer, size, 0);
}

int tcp::recv(char* buffer, int size) {
    if (!is_connected()) {
        return 0;
    }

    int res = ::recv(m_data->connect_socket, buffer, size, 0);
    if (res <= 0) {
        disconnect();
    }

    return res;
}

void tcp::broadcast() {
    std::vector<in_addr> sin_addrs;
#if defined K_WINDOWS
    ULONG size = 0;
    GetAdaptersInfo(nullptr, &size);
    if (!size) {
        return;
    }

    PIP_ADAPTER_INFO infos = (PIP_ADAPTER_INFO) new char[size];
    if (ERROR_SUCCESS == GetAdaptersInfo(infos, &size)) {
        for (PIP_ADAPTER_INFO info = infos; info; info = info->Next) {
            for (PIP_ADDR_STRING string = &info->IpAddressList; string; string = string->Next) {
                in_addr address;
                inet_pton(AF_INET, string->IpAddress.String, &address);
                if (INADDR_ANY != address.s_addr && INADDR_LOOPBACK != address.s_addr) {
                    in_addr mask, broadcast;
                    inet_pton(AF_INET, string->IpMask.String, &mask);
                    broadcast.s_addr = address.s_addr | ~mask.s_addr;
                    sin_addrs.push_back(broadcast);
                }
            }
        }
    }

    delete[] (char*)infos;
#elif defined K_LINUX
    ifaddrs* ifas = nullptr;
    if (0 == getifaddrs(&ifas)) {
        for (ifaddrs* ifa = ifas; ifa; ifa = ifa->ifa_next) {
            if ((IFF_BROADCAST & ifa->ifa_flags) && ifa->ifa_broadaddr && AF_INET == ifa->ifa_broadaddr->sa_family) {
                sin_addrs.push_back(((sockaddr_in*)ifa->ifa_broadaddr)->sin_addr);
            }
        }
        freeifaddrs(ifas);
    }
#endif

    if (sin_addrs.empty()) {
        return;
    }

    socket_t broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID_SOCKET == broadcast_socket) {
        return;
    }

    int opt = 1;
    setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

    char name[256] = {0};
    gethostname(name, sizeof(name));
    int name_len = (int)strlen(name);

    sockaddr_in broadcast_addr = {0};
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(DEFAULT_PORT);

    struct {
        void unlock() {}
        void lock() {}
    } lock;

    while (is_listening()) {
        if (!is_connected()) {
            for (in_addr sin_addr : sin_addrs) {
                broadcast_addr.sin_addr = sin_addr;
                sendto(broadcast_socket, name, name_len, 0, (sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
            }
        }
        m_data->m_cond_closed.wait_for(lock, std::chrono::seconds(BROADCAST_INTERVAL));
    }

    closesocket(broadcast_socket);
}
