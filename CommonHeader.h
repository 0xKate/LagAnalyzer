#pragma once


#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include <unordered_map>
#include <regex>

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define WORKING_BUFFER_SIZE 15000

typedef std::vector<std::string> IPAddressList;
typedef std::unordered_map<std::string, IPAddressList> IPAddressDatabase;