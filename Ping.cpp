#pragma once

#include "Ping.h"
#include "NetHelpers.h"


// Constructor
Ping::Ping(const std::string& target) : m_target(target), m_complete(false), m_count(10) {}

void Ping::join()
{
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

// Destructor
Ping::~Ping() {
    this->join();
}

// Method to start the ping
void Ping::start() {
    m_thread = std::thread(&Ping::sendPing, this, m_target);
}

// Method to check if ping is complete
bool Ping::isComplete() const {
    return m_complete;
}

// Method to get the result of the ping
std::vector<EchoReply> Ping::getResult() const {
    return m_pings;
}

void Ping::printResults()
{
    auto x = this->getResult();

    for (const EchoReply& i : x) {
        std::cout << "Ping " << i.seq << ": Reply from " << ipv4ToString(i.ipAddress) << ": time=" << i.roundTripTime << "ms TTL=" << static_cast<int>(i.ttl) << std::endl;
    }
    std::cout << std::endl;

}

void Ping::sendPing(const std::string& ipAddress) {
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create ICMP file" << std::endl;
        return;
    }

    // Convert ip string to binary
    in_addr addr;
    stringToIPv4(ipAddress, &addr);

    // Allocate memory for the ICMP echo reply structure
    DWORD replyBufferSize = sizeof(ICMP_ECHO_REPLY) + 8 + 1; // Extra space for data
    std::unique_ptr<BYTE[]> replyBuffer(new BYTE[replyBufferSize]);

    for (int i = 0; i < m_count; ++i) {
        DWORD replySize = IcmpSendEcho(hIcmpFile, addr.s_addr, nullptr, 0, nullptr, replyBuffer.get(), replyBufferSize, 1000);
        if (replySize == 0) {
            std::cerr << "Failed to send ICMP echo request (attempt " << (i + 1) << "/30)" << std::endl;
        }
        else {
            // Extract fields from the replyBuffer
            EchoReply reply;

            ICMP_ECHO_REPLY* icmpReply = reinterpret_cast<ICMP_ECHO_REPLY*>(const_cast<BYTE*>(replyBuffer.get()));
            reply.ipAddress = icmpReply->Address;
            reply.roundTripTime = icmpReply->RoundTripTime;
            reply.ttl = icmpReply->Options.Ttl;
            reply.seq = i;

            this->m_pings.push_back(reply);
        }

        // Wait for 1 second before sending the next ping
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    IcmpCloseHandle(hIcmpFile);
}
