#pragma once
#include <string>
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "Core.hpp"
#include "Memory.h"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Matrix.hpp"
#include "Camera.hpp"

struct Player {
    LocalPlayer* Myself;
    Core* Map;

    int Index;
    uint64_t Address = 0;
    uint64_t UnitAddress = 0;
    bool Valid = false;

    char NameBuffer[8] = { 0 };
    std::string Name;
    uint32_t TempTeam = -1;
    uint8_t Team = -1;
    uint32_t TempGuiState = -1;
    uint8_t GuiState = -1;
    bool IsDead;
    bool IsLocal;
    bool IsAlly() {
        return Team == Myself->Team;
    }
    bool IsHostile() {
        return Team != Myself->Team;
    }

    Vector3D Position;
    float RotationMatrix[3][3]; // New field for rotation matrix

    float DistanceToLocalPlayer;
    float Distance2DToLocalPlayer;

    Player(int PlayerIndex, LocalPlayer* Me, Core* Map) {
        this->Index = PlayerIndex;
        this->Myself = Me;
        this->Map = Map;
    }

    void ResetPlayer() {
        Address = 0;
        Valid = false;
        Team = -1;
        GuiState = -1;
    }

    void ValidCheck() {
        if (Valid) {
            if (Valid > 0x7FFFFFFEFFFF || Valid < 0x7FF000000000) {
                Address = 0;
                Valid = false;
            }
        }
    }

    bool IsValid() {
        return mem.IsValidPointer(Address);
    }

    float GetYawFromRotationMatrix() {
        // Extract yaw angle from the rotation matrix
        return atan2f(RotationMatrix[1][0], RotationMatrix[0][0]);
    }

    void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera);
};