#pragma once

// Core
constexpr uint64_t off_gamemode_array = 0x4EF9A80;
constexpr uint64_t off_hud = 0x4F4D328;
constexpr uint64_t off_game_context = 0x4F52178;
constexpr uint64_t off_spectated_unit = 0x4F5EFB0;
constexpr uint64_t off_local_unit = 0x4F5EFB8;
constexpr uint64_t off_current_map = 0x4F94FC0;
constexpr uint64_t off_current_mission = 0x4F95718;
constexpr uint64_t off_current_gamemode_index = 0x4F95DC0;
constexpr uint64_t off_view_matrix = 0x4FBE2F0;
constexpr uint64_t off_player_list = 0x4FD4AE8;
constexpr uint64_t off_player_count = 0x4FD4AF8;
constexpr uint64_t off_local_player = 0x4FD4B00;
constexpr uint64_t off_net = 0x4FD5448;
constexpr uint64_t off_radar_enable = 0x4FF12D4;
constexpr uint64_t off_entity_manager = 0x4FF3378;

// Player
constexpr uint64_t off_player_team = 0x210;
constexpr uint64_t off_gui_state = 0x500;
constexpr uint64_t off_unit_ptr = 0x700;

// Unit
constexpr uint64_t off_byte_size = 0x2500;
constexpr uint64_t off_bb_min = 0x318;
constexpr uint64_t off_bb_max = 0x324;
constexpr uint64_t off_reload_time = 0x978;
constexpr uint64_t off_rotation = 0xB94 + 0x10;
constexpr uint64_t off_position = 0xBB8 + 0x10;
constexpr uint64_t off_air_velocity_ptr = 0xBD8;
constexpr uint64_t off_air_velocity_offset = 0xB20;
constexpr uint64_t off_air_acceleration_offset = 0x9A0;
constexpr uint64_t off_invul_state = 0x1218;
constexpr uint64_t off_flags = 0x12A8;
constexpr uint64_t off_is_visible_byte = 0x12A2;
constexpr uint64_t off_state = 0x12F8;
constexpr uint64_t off_type = 0x1300;
constexpr uint64_t off_entity_ptr = 0x1308;
constexpr uint64_t off_unit_team = 0x1398;
constexpr uint64_t off_class_ptr = 0x13C0;
constexpr uint64_t off_unit_info = 0x13A8;
constexpr uint64_t off_weapon_info = 0x1458;
constexpr uint64_t off_vehicle_name_ptr = 0x8;
constexpr uint64_t off_vehicle_class_ptr = 0x38;
constexpr uint64_t off_short_vehicle_name_ptr = 0x8;
constexpr uint64_t off_ground_velocity = 0x3644;
constexpr uint64_t off_ground_velocity_ptr = 0x2178;
constexpr uint64_t off_ground_velocity_offset = 0x54;

