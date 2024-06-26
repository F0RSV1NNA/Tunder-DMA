#include "Players.h"
#include "Memory.h"
#include "Offsets.h"

Player::Player(uintptr_t baseAddr) : baseAddress(baseAddr), x(0), y(0), z(0), team(0) {
    char nameBuffer[256];
    mem.Read(baseAddress + PlayerNameOffset, nameBuffer, sizeof(nameBuffer));
    name = std::string(nameBuffer);
}

void Player::Update() {
    // Use scatter read for position and team
    VMMDLL_SCATTER_HANDLE handle = mem.CreateScatterHandle();
    mem.AddScatterReadRequest(handle, baseAddress + PlayerInformationOffset + PositionOffset, &x, sizeof(x));
    mem.AddScatterReadRequest(handle, baseAddress + PlayerInformationOffset + PositionOffset + sizeof(float), &y, sizeof(y));
    mem.AddScatterReadRequest(handle, baseAddress + PlayerInformationOffset + PositionOffset + 2 * sizeof(float), &z, sizeof(z));
    mem.AddScatterReadRequest(handle, baseAddress + PlayerInformationOffset + TeamOffset, &team, sizeof(team));
    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);
}

std::vector<Player> GetPlayers() {
    std::vector<Player> players;
    uintptr_t baseAddress = mem.GetBaseDaddy("aces.exe");
    int playerCount;
    mem.Read(baseAddress + PlayerCountOffset, &playerCount, sizeof(playerCount));

    uintptr_t playerListPtr;
    mem.Read(baseAddress + PlayerListOffset, &playerListPtr, sizeof(playerListPtr));

    for (int i = 0; i < playerCount; ++i) {
        uintptr_t playerBase;
        mem.Read(playerListPtr + i * sizeof(uintptr_t), &playerBase, sizeof(playerBase));
        Player player(playerBase);
        player.Update();
        players.push_back(player);
    }

    return players;
}
