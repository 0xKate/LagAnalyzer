/*

  _                   _                _
 | |    __ _  __ _   / \   _ __   __ _| |_   _ _______ _ __
 | |   / _` |/ _` | / _ \ | '_ \ / _` | | | | |_  / _ | '__|
 | |__| (_| | (_| |/ ___ \| | | | (_| | | |_| |/ |  __| |
 |_____\__,_|\__, /_/   \_|_| |_|\__,_|_|\__, /___\___|_|
             |___/                       |___/

*/
/*
    Copyright (C) 2024 0xKate

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "Ping.h"
#include "NetHelpers.h"


Ping::Ping(const std::string& target, const std::string& m_interface) : m_interval(1.0f), m_target(target), m_interface(m_interface), m_complete(false), m_count(4) {}


Ping::~Ping() {
    this->join();
}

std::string Ping::getAddress()
{
    return this->m_target;
}

std::string Ping::getInterface()
{
    return this->m_interface;
}

float Ping::getInterval()
{
    return this->m_interval;
}

int Ping::getCount()
{
    return this->m_count;
}

void Ping::start() {
    this->m_thread = std::thread(&Ping::sendPing, this, m_target);
    this->m_thread.detach();
}

void Ping::join()
{
    if (this->m_thread.joinable()) {
        this->m_thread.join();
    }
}

bool Ping::isComplete() const {
    return this->m_complete;
}

std::vector<EchoReply> Ping::getResult() const {
    return this->m_pings;
}

void Ping::printResults()
{
    if (this->m_pings.empty()) {
        std::cout << "No results available." << std::endl;
        return;
    }

    ULONG minRTT = std::numeric_limits<ULONG>::max();
    ULONG maxRTT = 0;
    ULONG totalRTT = 0;

    std::cout << "Server: " << this->m_interface << std::endl;
    std::cout << " -IP Address: " << this->m_target << std::endl;  
    
    for (const auto& reply : this->m_pings) {

        if (false)
        std::cout << "  - Ping " << reply.seq << ": Reply from " << ipv4ToString(reply.ipAddress) << ": time=" << reply.roundTripTime << "ms TTL=" << static_cast<int>(reply.ttl) << std::endl;

        minRTT = std::min(minRTT, reply.roundTripTime);
        maxRTT = std::max(maxRTT, reply.roundTripTime);
        totalRTT += reply.roundTripTime;
    }

    float averageRTT = static_cast<float>(totalRTT) / this->m_pings.size();

    std::cout << "  - Minimum Round Trip Time: " << minRTT << "ms" << std::endl;
    std::cout << "  - Maximum Round Trip Time: " << maxRTT << "ms" << std::endl;
    std::cout << "  - Average Round Trip Time: " << averageRTT << "ms" << std::endl;

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

    DWORD timeout = static_cast<DWORD>(m_interval * 1000); // Convert interval to milliseconds

    for (int i = 0; i < this->m_count; ++i) {
        DWORD replySize = IcmpSendEcho(hIcmpFile, addr.s_addr, nullptr, 0, nullptr, replyBuffer.get(), replyBufferSize, timeout);
        this->m_sent++;
        if (replySize == 0) {
            std::cerr << "Failed to send ICMP echo request (attempt " << (i + 1) << "/" << this->m_count<< ")" << std::endl;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_interval * 1000)));
    }

    IcmpCloseHandle(hIcmpFile);

    this->m_complete = true;
}
