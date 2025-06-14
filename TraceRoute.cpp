#include "TraceRoute.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <fcntl.h>

TraceRoute::TraceRoute(const std::string& dest) : destination(dest) {}

uint16_t TraceRoute::computeChecksum(uint16_t* data, int length) {
    uint32_t sum = 0;
    while (length > 1) {
        sum += *data++;
        length -= 2;
    }
    if (length == 1) {
        sum += *(uint8_t*)data;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

std::string TraceRoute::resolveHostname(const std::string& host) {
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    if (getaddrinfo(host.c_str(), nullptr, &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ip, sizeof(ip));
    std::string ipStr(ip);
    freeaddrinfo(res);
    return ipStr;
}

void TraceRoute::sendProbe(int sockfd, sockaddr_in& dest_addr, int ttl, int seq) {
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    char sendbuf[64];
    struct icmphdr* icmp = (struct icmphdr*)sendbuf;
    memset(sendbuf, 0, sizeof(sendbuf));
    icmp->type = ICMP_E;
    icmp->code = 0;
    icmp->un.echo.id = getpid();
    icmp->un.echo.sequence = seq;
    icmp->checksum = computeChecksum((uint16_t*)icmp, sizeof(sendbuf));

    sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
}

bool TraceRoute::receiveReply(int sockfd, std::string& addrStr, int timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    struct timeval tv{};
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (select(sockfd + 1, &readfds, nullptr, nullptr, &tv) > 0) {
        char recvbuf[512];
        struct sockaddr_in recv_addr{};
        socklen_t addr_len = sizeof(recv_addr);
        int n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                         (struct sockaddr*)&recv_addr, &addr_len);
        if (n > 0) {
            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(recv_addr.sin_addr), addr_buf, sizeof(addr_buf));
            addrStr = addr_buf;
            return true;
        }
    }
    return false;
}

void TraceRoute::run() {
    std::string ip = resolveHostname(destination);
    std::cout << "TraceRoute to " << destination << " (" << ip << ")\n";

    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    for (int ttl = 1; ttl <= MAX_HOPS; ++ttl) {
        sendProbe(sockfd, dest_addr, ttl, ttl);
        std::string addrStr;
        std::cout << ttl << " ";

        if (receiveReply(sockfd, addrStr, TIMEOUT_SEC)) {
            std::cout << addrStr << std::endl;
            if (addrStr == ip)
                break;
        } else {
            std::cout << "*\n";
        }
    }

    close(sockfd);
}
