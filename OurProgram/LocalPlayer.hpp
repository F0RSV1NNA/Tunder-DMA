#pragma once
#include <iostream>

#include "Memory.h"
#include "Offsets.hpp"

#include "Vector2D.hpp"
#include "Vector3D.hpp"

struct LocalPlayer {
    uint64_t Address;
    uint64_t UnitAddress;

    bool IsDead;
    uint32_t TempTeam = -1;
    uint8_t Team = -1;
    uint32_t TempGuiState = -1;
    uint8_t GuiState = -1;
    Vector3D Position;

    void ResetPointer() {
        Address = 0;
    }

    bool ValidPosition() {
		if (Position.x != 0.0f && Position.y != 0.0f && Position.z != 0.0f) {
            return true;
		} else {
			return false;
		}
	}

    bool IsValid() {
        return mem.IsValidPointer(Address);
    }

    bool IsCombatReady() {
        if (!mem.IsValidPointer(Address)) return false;
        //if (GuiState != 2) return false;
        //if (IsDead) return false;
        return true;
    }

    void Read() {
        Address = mem.Read<uint64_t>(mem.base_address + off_local_player, true);

        if (!IsValid()) return;

        //auto handle = mem.CreateScatterHandle();

        // Scatter read request for Team
        uint64_t teamAddress = Address + off_player_team;
        //mem.AddScatterReadRequest(handle, teamAddress, &Team, sizeof(uint8_t));
        Team = mem.Read<uint8_t>(teamAddress, true);

        // Scatter read request for Gui State
        uint64_t guiStateAddress = Address + off_gui_state;
        //mem.AddScatterReadRequest(handle, guiStateAddress, &GuiState, sizeof(uint8_t));
        GuiState = mem.Read<uint8_t>(guiStateAddress, true);

        // Scatter read request for Unit Ptr
        uint64_t unitPtrAddress = Address + off_unit_ptr;
        //mem.AddScatterReadRequest(handle, unitPtrAddress, &UnitAddress, sizeof(uint64_t));
        UnitAddress = mem.Read<uint64_t>(unitPtrAddress, true);

        // Execute the scatter read
        //mem.ExecuteReadScatter(handle);

        // Close the scatter handle
        //mem.CloseScatterHandle(handle);

        //std::cout << static_cast<int>(Team) << ", " << static_cast<int>(GuiState) << std::endl;

        //std::cout << "LocalPlayer UnitPtr: " << std::hex << UnitAddress << std::dec << std::endl;
        if (mem.IsValidPointer(UnitAddress)) {
            Position = mem.Read<Vector3D>(UnitAddress + off_position);
            //std::cout << "Position: " << Position.x << ", " << Position.y << ", " << Position.z << std::endl;
        }
    }
};