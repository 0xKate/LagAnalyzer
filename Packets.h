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

#include <cstdint>


/*
Type: 
 - ICMP type, see § Control messages.
Code:
 - ICMP subtype, see § Control messages.
Checksum:
 - Internet checksum (RFC 1071) for error checking, calculated from the ICMP header and data with value 0 substituted for this field.
Rest of header:
 - Four-byte field, contents vary based on the ICMP type and code.

*/

// ICMP Packet Structure
struct IcmpPacket {
    uint8_t type;       // ICMP message type
    uint8_t code;       // ICMP message code
    uint16_t checksum;  // ICMP checksum
    uint16_t identifier; // Identifier
    uint16_t sequenceNumber; // Sequence number
    // Data payload (variable length)
};