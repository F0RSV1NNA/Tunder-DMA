// radar.cpp
#include "Radar.h"
#include "Players.h"
#include <imgui.h>

void DrawRadar() {
    ImGui::Begin("Radar");

    std::vector<Player> players = GetPlayers();
    for (const auto& player : players) {
        ImVec2 playerPos = ImVec2(player.x, player.y);
        ImGui::GetWindowDrawList()->AddCircleFilled(playerPos, 5.0f, IM_COL32(255, 0, 0, 255));
    }

    ImGui::End();
}
