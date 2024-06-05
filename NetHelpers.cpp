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

#include "NetHelpers.h"

bool isValidIPAddress(const std::string& str) {
    // Regular expression to match IP address
    std::regex ipRegex("^\\b(?:\\d{1,3}\\.){3}\\d{1,3}\\b$");
    return std::regex_match(str, ipRegex);
}

// Function to convert IPv4 address to string
std::string ipv4ToString(DWORD ipAddress) {
    char ipAddrStr[INET_ADDRSTRLEN];
    InetNtopA(AF_INET, &ipAddress, ipAddrStr, INET_ADDRSTRLEN);
    return std::string(ipAddrStr);
}

void stringToIPv4(const std::string& ipAddress, struct in_addr* addr)
{
    int result = inet_pton(AF_INET, ipAddress.c_str(), addr);
    if (result != 1) {
        std::cerr << "Invalid IP address: " << ipAddress << std::endl;
    }
}


// Function to convert IPv6 address to string
std::string ipv6ToString(const IN6_ADDR& ipAddress) {
    char ipAddrStr[INET6_ADDRSTRLEN];
    InetNtopA(AF_INET6, &ipAddress, ipAddrStr, INET6_ADDRSTRLEN);
    return std::string(ipAddrStr);
}


// Function to get all IP addresses (local, gateway, DNS)
std::vector<std::string> GetDnsServers() {
    std::vector<std::string> ipAddresses;
    ULONG outBufLen = WORKING_BUFFER_SIZE;
    std::unique_ptr<BYTE[]> buffer(new BYTE[outBufLen]);

    // Get adapter info
    DWORD dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get()), &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buffer.reset(new BYTE[outBufLen]);
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get()), &outBufLen);
    }

    if (dwRetVal != NO_ERROR) {
        std::cerr << "Failed to get adapter addresses. Error: " << dwRetVal << std::endl;
        return ipAddresses;
    }

    // Iterate through adapters
    for (PIP_ADAPTER_ADDRESSES pCurrAdapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.get());
        pCurrAdapter != nullptr;
        pCurrAdapter = pCurrAdapter->Next) {

        // Get DNS server addresses
        for (PIP_ADAPTER_DNS_SERVER_ADDRESS pDnsServer = pCurrAdapter->FirstDnsServerAddress;
            pDnsServer != nullptr;
            pDnsServer = pDnsServer->Next) {
            sockaddr* saAddr = pDnsServer->Address.lpSockaddr;
            if (saAddr->sa_family == AF_INET) {
                ipAddresses.push_back(ipv4ToString(((sockaddr_in*)saAddr)->sin_addr.S_un.S_addr));
            }
        }
    }

    return ipAddresses;
}


// Function to get gateway IP addresses
IPAddressDatabase GetLocalIPData() {

    IPAddressDatabase ipData;
    ULONG bufferSize = WORKING_BUFFER_SIZE;
    std::unique_ptr<BYTE[]> buffer(new BYTE[bufferSize]);

    // Get adapter info
    PIP_ADAPTER_INFO pAdapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.get());
    DWORD dwRetVal = GetAdaptersInfo(pAdapterInfo, &bufferSize);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buffer.reset(new BYTE[bufferSize]);
        pAdapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.get());
        dwRetVal = GetAdaptersInfo(pAdapterInfo, &bufferSize);
    }

    if (dwRetVal != NO_ERROR) {
        std::cerr << "Failed to get adapter info. Error: " << dwRetVal << std::endl;
        return ipData;
    }

    // Iterate through adapters
    for (PIP_ADAPTER_INFO pCurrAdapter = pAdapterInfo; pCurrAdapter != nullptr; pCurrAdapter = pCurrAdapter->Next) {
        // Get gateway addresses
        PIP_ADDR_STRING pGateway = &pCurrAdapter->GatewayList;
        while (pGateway != nullptr) {
            ipData["Gateway"].push_back(pGateway->IpAddress.String);
            pGateway = pGateway->Next;
        }
        // Get addresses
        PIP_ADDR_STRING pAddress = &pCurrAdapter->IpAddressList;
        while (pAddress != nullptr) {
            ipData["Address"].push_back(pAddress->IpAddress.String);
            pAddress = pAddress->Next;
        }
    }

    return ipData;
}


void DisplayIPData(IPAddressDatabase ipData)
{
    if (ipData.empty()) {
        std::cerr << "No IP addresses found." << std::endl;
    }
    else {
        for (const auto& entry : ipData) {
            const std::string& interfaceName = entry.first; // Key (interface name)
            const IPAddressList& ipAddresses = entry.second; // Value (vector of IP addresses)

            // Output interface name
            std::cout << "Server: " << interfaceName << std::endl;

            // Output IP addresses for the interface
            for (const std::string& ipAddress : ipAddresses) {
                std::cout << "  IP Address: " << ipAddress << std::endl;
            }
        }
        std::cout << std::endl;
    }
}


bool InitializeWinsock() {
    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return (wsaStartupResult == 0);
}


void CleanupWinsock() {
    WSACleanup();
}

std::vector<std::string> ResolveHostname(const std::string& hostname) {
    std::vector<std::string> ipAddresses;

    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    // Clear the hints struct
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Use stream socket
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol

    // Resolve the hostname
    int getAddrInfoResult = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (getAddrInfoResult != 0) {
        std::cerr << "getaddrinfo failed: " << getAddrInfoResult << std::endl;
        return ipAddresses;
    }

    // Iterate through the result linked list and collect IPv4 addresses
    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        char ipstr[INET_ADDRSTRLEN];

        // Get the pointer to the address if it's IPv4
        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
            void* addr = &(ipv4->sin_addr);

            // Check if addr is still nullptr
            if (addr != nullptr) {
                // Convert the IPv4 address to a string and add it to the vector
                inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
                ipAddresses.push_back(ipstr);
            }
        }
    }

    // Free the memory allocated for the result
    freeaddrinfo(result);

    return ipAddresses;
}
