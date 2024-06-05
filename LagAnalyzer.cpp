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


int main(int argc, char** argv) {
    IPAddressDatabase ipData = GetLocalIPData();
    ipData["DNS"] = GetDnsServers();

    DisplayIPData(ipData);
    
    return 0;
}

int parseArgs(int argc, char** argv)
{
    SetConsoleTitleA("LagAnalyzer");
    printf("LagAnalyzer Copyright(C) 2024 0xKate\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    printf("LagAnalyzer is distributed under the GNU GPLv3 license.\n");
    printf("See https://github.com/0xKate/LagAnalyzer/LICENSE.txt for more info.\n");
    printf("Source code @: https://github.com/0xKate/LagAnalyzer\n");

    cxxopts::Options argParser("LagAnalyzer.exe");

    argParser.add_options()
        ("t,target", "The target IP address", cxxopts::value<std::string>()->default_value(NULL))
        ("v,verbose", "Print out more info then typically needed.", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage")
        ;

    std::string targetIP;

    auto argResult = argParser.parse(argc, argv);
    auto verbose = argResult["verbose"].as<bool>();

    if (argResult.count("help"))
    {
        std::cout << argParser.help() << std::endl;
        return 101;
    }

    if (argResult.count("target")) {
        targetIP = argResult["target"].as<std::string>();
        if (verbose)
            std::cout << "TargetIP: " << targetIP << std::endl;
    }



    return EXIT_SUCCESS;
}