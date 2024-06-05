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


Ping::Ping(const std::string& target) : m_interval(1.0f), m_target(target), m_complete(false), m_count(4) {}

void Ping::join()
{
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

Ping::~Ping() {
    this->join();
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
    m_thread = std::thread(&Ping::sendPing, this, m_target);
}

bool Ping::isComplete() const {
    return m_complete;
}

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

    for (int i = 0; i < this->m_count; ++i) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_interval * 1000)));
    }

    IcmpCloseHandle(hIcmpFile);

    this->m_complete = true;
}
