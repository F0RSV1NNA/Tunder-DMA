#ifndef PLAYERS_H
#define PLAYERS_H

#include <string>
#include <vector>
#include <cstdint>
#include "Memory.h"
#include "Offsets.h"

class Player {
public:
    std::string name;
    uintptr_t baseAddress;
    float x, y, z;
    int team;

    Player(uintptr_t baseAddr);
    void Update();
};

std::vector<Player> GetPlayers();

#endif