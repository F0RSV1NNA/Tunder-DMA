#include <iostream>
#include <thread>
#include <unordered_set>
#include <locale>

#include "Memory.h"
#include "Core.hpp"
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Camera.hpp"

#include "Render.hpp"


// Game Objects
Core* GameCore = new Core();
LocalPlayer* Myself = new LocalPlayer();
Camera* GameCamera = new Camera();

// Players
std::vector<Player*>* PotentialPlayers = new std::vector<Player*>;
std::vector<Player*>* Players = new std::vector<Player*>;

void PlayerAddressScatter(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        int index = players[i]->Index;
        uint64_t address = GameCore->PlayerListAddress + (0x8 * i);
        mem.AddScatterReadRequest(handle, address, &players[i]->Address, sizeof(uint64_t));
    }

    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}

void PlayerDataScatter(std::vector<Player*>& players) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (!players[i]->IsValid()) {
            continue;
        }

        // Read Team
        uint64_t teamAddress = players[i]->Address + off_player_team;
        players[i]->Team = mem.Read<uint8_t>(teamAddress, true);

        // Read GuiState
        uint64_t guiStateAddress = players[i]->Address + off_gui_state;
        players[i]->GuiState = mem.Read<uint8_t>(guiStateAddress, true);

        // Read Unit Ptr
        uint64_t unitPtrAddress = players[i]->Address + off_unit_ptr;
        players[i]->UnitAddress = mem.Read<uint64_t>(unitPtrAddress, true);
    }
}

void PlayerUnitScatter(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];

        if (!mem.IsValidPointer(player->Address) || !mem.IsValidPointer(player->UnitAddress) || player->GuiState != 2) {
            player->ResetPlayer();
            continue;
        }

        // Scatter read request for Position
        uint64_t positionAddress = player->UnitAddress + off_position;
        mem.AddScatterReadRequest(handle, positionAddress, &player->Position, sizeof(Vector3D));

        // Scatter read request for Rotation Matrix
        uint64_t rotationAddress = player->UnitAddress + off_rotation;
        mem.AddScatterReadRequest(handle, rotationAddress, &player->RotationMatrix, sizeof(player->RotationMatrix));
    }

    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}



// Core
void UpdateCore() {
    static bool PlayersPopulated = false;
    try {
        while (true) {
            // Clear Players
            //Players->clear();

            //## Core Update ##//
            GameCore->Read();
            if (!GameCore->IsValid()) {
                continue;
            }

            //## Local Player Update ##//
            Myself->Read();
            if (!Myself->IsValid() || !Myself->ValidPosition() || Myself->GuiState != 2) {
                continue;
            }

            //## Player Update ##//
            // Populate Players
            PlayerAddressScatter(*PotentialPlayers);
            for (int i = 0; i < PotentialPlayers->size(); i++) {
                Player* p = PotentialPlayers->at(i);
                if (p->Address != 0)
                    Players->push_back(p);
            }

            // Update Players
            PlayerDataScatter(*Players);
            PlayerUnitScatter(*Players);
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
        return;
    }
}

int main()
{
    std::cout << "-----------------------------" << std::endl;
	std::cout << "WarThunderDMA" << std::endl;
	std::cout << "-----------------------------" << std::endl;

	// Initialize DMA
	if (!mem.Init("aces.exe", true, false))
	{
		std::cout << "Failed to initilize DMA" << std::endl;
        std::cout << "Press ENTER to continue...";
        std::cin.get();
	}
    std::cout << "DMA initilized" << std::endl;

    try {
        for (int i = 0; i < 32; i++)
            PotentialPlayers->push_back(new Player(i, Myself, GameCore));

        std::cout << "-----------------------------" << std::endl;

        // Threads
        std::thread coreThread(UpdateCore);
        std::thread renderThread(Render, Myself, Players, GameCamera);
        coreThread.join();
    }
    catch (...) {}

    std::cout << "Press ENTER to exit...";
    std::cin.get();  // Wait for user to press Enter
    return 0;
}