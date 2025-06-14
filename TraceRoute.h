#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <string>
#include <netinet/ip_icmp.h>

#ifndef __APPLE__
// Linux already has icmphdr
#include <netinet/ip_icmp.h>
#else
// macOS: define it ourselves
struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo;
        uint32_t gateway;
        struct {
            uint16_t mtu;
        } frag;
    } un;
};
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#endif

class TraceRoute {
public:
    TraceRoute(const std::string& destination);
    void run();

private:
    std::string destination;
    std::string resolveHostname(const std::string& host);
    uint16_t computeChecksum(uint16_t* data, int length);
    void sendProbe(int sockfd, sockaddr_in& dest_addr, int ttl, int seq);
    bool receiveReply(int sockfd, std::string& addrStr, int timeout);

    static const int ICMP_E = 8;
    static const int MAX_HOPS = 30;
    static const int TIMEOUT_SEC = 2;

};

#endif
