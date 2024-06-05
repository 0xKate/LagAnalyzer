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

#include "cxxopts.hpp"

#include "NetHelpers.h"

#include "Ping.h"

//extern bool g_verboseLogging;

int DisplayTime(float duration) {
    // Get the current system time
    auto now = std::chrono::system_clock::now();

    // Convert the system time to time since epoch
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Get the local time with timezone
    std::tm local_time;
    localtime_s(&local_time, &now_time);

    // Add 30 seconds to the current time
    now += std::chrono::seconds((int)duration + 1);

    // Convert the updated time to time since epoch
    now_time = std::chrono::system_clock::to_time_t(now);

    // Get the updated local time with timezone
    localtime_s(&local_time, &now_time);

    // Display the updated local time
    std::cout << "The test will end by: " << std::put_time(&local_time, "%H:%M:%S") << " (+"<< duration + 1<<" seconds)" << std::endl << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    SetConsoleTitleA("LagAnalyzer");
    printf("LagAnalyzer Copyright(C) 2024 0xKate\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    printf("LagAnalyzer is distributed under the GNU GPLv3 license.\n");
    printf("See https://github.com/0xKate/LagAnalyzer/LICENSE.txt for more info.\n");
    printf("Source code @: https://github.com/0xKate/LagAnalyzer\n\n");

    cxxopts::Options argParser("LagAnalyzer.exe");

    argParser.add_options()
        ("t,target", "The target IP address", cxxopts::value<std::string>()->default_value("1.1.1.1"))
        ("v,verbose", "Print out more info then typically needed.", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage")
        ;

    

    auto argResult = argParser.parse(argc, argv);
    // g_verboseLogging = argResult["verbose"].as<bool>();

    if (argResult.count("help"))
    {
        std::cout << argParser.help() << std::endl;
        return 101;
    }

    InitializeWinsock();
    
    IPAddressDatabase ipData = GetLocalIPData();
    ipData["DNS"] = GetDnsServers();

    std::string target = argResult["target"].as<std::string>();
    if (isValidIPAddress(target))
    {
        ipData["Target"].push_back(target);
        //if (g_verboseLogging)
        //    std::cout << "TargetIP: " << target << std::endl;
    }
    else
    {
        ipData["Target"] = ResolveHostname(target);
    }


    if (ipData.empty()) {
        std::cerr << "No IP addresses found." << std::endl;
    }
    else {
        //DisplayIPData(ipData);

        std::vector<std::unique_ptr<Ping>> pings;

        for (const auto& entry : ipData) {
            const std::string& interfaceName = entry.first;
            const IPAddressList& ipAddresses = entry.second;

            // Output interface name
            // std::cout << "Server: " << interfaceName << std::endl;

            //Output IP addresses for the interface
            for (const std::string& ipAddress : ipAddresses) {
                // std::cout << " -IP Address: " << ipAddress << std::endl;

                if (isValidIPAddress(ipAddress))
                {
                    std::unique_ptr<Ping> p = std::make_unique<Ping>(ipAddress, interfaceName);
                    p->start();
                    pings.push_back(std::move(p));
                }
            }
            //DisplayTime(pings.back().getCount() * pings.back().getInterval());
        }

         
        std::this_thread::sleep_for(std::chrono::milliseconds(7000));


        // Wait for all pings to complete
        bool allComplete = false;
        while (!allComplete) {
            allComplete = true;
            for (auto& ping : pings) {
                if (!ping->isComplete()) {
                    allComplete = false;
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for a short time before checking again
        }
        
        for (auto& ping : pings) {
            ping->join(); // Wait for the thread associated with each ping to finish
        }

        // Now that all threads have finished, it's safe to access the results
        for (auto& ping : pings) {
            auto r = ping->getResult();
            //for (auto& x : r)
            //{
            //    std::cout << "  - Ping " << x.seq << ": Reply from " << ipv4ToString(x.ipAddress) << ": time=" << x.roundTripTime << "ms TTL=" << static_cast<int>(x.ttl) << std::endl;
            //}

            ping->printResults();

        }
        
        //calculateLatencyIncreases(pings);

        
    }

    CleanupWinsock();

    system("pause");

    return EXIT_SUCCESS;
}