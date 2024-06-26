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


// Convert IPv4 address to string
std::string ipv4ToString(DWORD ipAddress);

// Convert string to IPv4 struct
void stringToIPv4(const std::string& ipAddress, struct in_addr* addr);

// Convert IPv6 address to string
std::string ipv6ToString(const IN6_ADDR& ipAddress);

// Check if string is valid IP address
bool isValidIPAddress(const std::string& str);

// Get a Vector<string> list of dns servers
IPAddressList GetDnsServers();

// Get a dict<string, Vector<string> of ip addresses
IPAddressDatabase GetLocalIPData();

// Print out an IP Database
void DisplayIPData(IPAddressDatabase ipData);

// Initialize Winsock at the start of the program
bool InitializeWinsock();

// Cleanup Winsock at the end of the program
void CleanupWinsock();

// Resolve hostname to a list of addresses
IPAddressList ResolveHostname(const std::string& hostname);