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
    Ping(const std::string& target, const std::string& m_interface);
    ~Ping();
    std::string getAddress();
    std::vector<EchoReply> getResult() const;
    std::string getInterface();
    float getInterval();
    int getCount();
    bool isComplete() const;
    void start();
    void join();
    void printResults();

private:
    int m_count = 4;
    int m_sent = 0;
    int m_recv = 0;
    float m_interval = 1.0f;
    bool m_complete = false;
    std::thread m_thread;
    std::string m_target;
    std::string m_interface;
    std::vector<EchoReply> m_pings;
    
    // Method to send ICMP echo request
    void sendPing(const std::string& ipAddress);
};

