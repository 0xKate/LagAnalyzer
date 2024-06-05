#pragma once

// Prevent Windows from defining min and max macros
#define NOMINMAX 

// Buffer size definition
#define WORKING_BUFFER_SIZE 15000

// Standard library includes
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
#include <algorithm>
#include <limits>
#include <ctime>
#include <iomanip>
#include <iostream>

// Windows-specific includes
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <icmpapi.h>

// Link necessary libraries
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

// Type definitions
typedef std::vector<std::string> IPAddressList;
typedef std::unordered_map<std::string, IPAddressList> IPAddressDatabase;

