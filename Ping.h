#pragma once

#include "CommonHeader.h"

struct EchoReply
{
public:
    ULONG ipAddress;
    ULONG roundTripTime;
    BYTE ttl;
    INT seq;
};

class Ping {
public:
    // Constructor
    Ping(const std::string& target);

    // Destructor
    ~Ping();

    // Method to start the ping
    void start();

    // Joins the thread
    void join();

    // Method to check if ping is complete
    bool isComplete() const;

    // Method to get the result of the ping
    std::vector<EchoReply> getResult() const;

    void printResults();

private:
    std::string m_target;
    std::vector<EchoReply> m_pings;
    int m_count = 4;
    bool m_complete;
    std::thread m_thread;

    // Method to send ICMP echo request
    void sendPing(const std::string& ipAddress);
};

