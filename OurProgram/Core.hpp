#pragma once
#include "Offsets.hpp"
#include "Memory.h"

struct Core {

    uint64_t PlayerListAddress;
    int PlayerCount = 0;

    void Read() {
        PlayerListAddress = mem.Read<uint64_t>(mem.base_address + off_player_list, true);
        std::cout << "PlayerListAddress: " << std::hex << PlayerListAddress << std::dec << std::endl;
        PlayerCount = mem.Read<uint8_t>(mem.base_address + off_player_count, true);
        std::cout << "PlayerCount: " << PlayerCount << std::endl;
    }

    bool IsValid() {
        if (!mem.IsValidPointer(PlayerListAddress) || PlayerCount < 2) {
            return false;
        }
        return true;
    }
};