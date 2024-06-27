#pragma once
#include <vector>
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Camera.hpp"

void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera);
