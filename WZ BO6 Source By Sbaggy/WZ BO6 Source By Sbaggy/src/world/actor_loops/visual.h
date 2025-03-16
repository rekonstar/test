#pragma once
#include "../helper/visual_help.h"
#include <unordered_map>
#include <vector>

void actor_loop ( ) {
	if ( engine::game::is_user_in_game ( ) ) {

		int player_count = engine::game::player_count ( );

		auto client_info = decrypt_client_info ( );
		auto client_base = decrypt_client_base ( client_info );

		auto index = engine::player::local_player_index ( client_base, client_info );
		auto local_player = client_base + ( index * offsets::player::size );

		decrypt_refdef->ref_def_nn = Driver::Read<ref_def_t> ( decrypt_refdef->retrieve_ref_def ( ) );
		ref_def_t& ref_def = decrypt_refdef->ref_def_nn;

		auto bone_base = decrypt_bone_base ( );
		Vector3 bone_pos = engine::bones::retrieve_bone_position_vec ( client_info );

		for ( int i = 0; i < player_count; i++ ) {
			auto player = client_base + ( i * offsets::player::size );

			Vector3 ent_position = engine::player::get_position ( player );

			auto bone_index = get_bone_index ( i );
			auto bone_ptr = engine::bones::bone_pointer ( bone_base, bone_index );

			bool is_visible = engine::game::is_visible ( i );

			ImColor visual_color;
			if ( is_visible ) { visual_color = ImColor (0, 255,0,255 ); } else { visual_color = ImColor ( 255, 0, 0, 255 ); }

			if ( ent_position.x == 0 || ent_position.y == 0 || ent_position.z == 0 || ! engine::player::is_player_valid( player ) || !bone_ptr ) continue;
			auto ent_distance = engine::utils::units_to_m ( engine::player::get_position ( local_player ).distance_to ( ent_position ) );

			/*
			* THIS IS IF U WANT TO USE THE PLAYER POSITION
			*/
			Vector2 ent_screen;
			if ( engine::game::w2s ( ent_position, ent_screen ) ) {
				ImGui::GetBackgroundDrawList ( )->AddText ( ImVec2 ( ent_screen.x - ( ImGui::CalcTextSize ( engine::utils::distace_to_str ( ( int ) ent_distance ).c_str ( ) ).x / 2 + 1 ), ent_screen.y ), visual_color, engine::utils::distace_to_str ( ( int ) ent_distance ).c_str ( ) );
				ImGui::GetForegroundDrawList ( )->AddLine ( ImVec2 ( 1920 / 2, 1080 ), ImVec2 ( ent_screen.x, ent_screen.y ), visual_color, 1.f );
			}

			/*
			* THIS IF U WANT TO USE THE PLAYER BONE | 7 IS THE HEAD
			*/
			auto head = engine::bones::get_bone ( bone_ptr, bone_pos, 7 );
			Vector2 head_screen;
			if ( engine::game::w2s ( head, head_screen ) ) {
				ImGui::GetForegroundDrawList ( )->AddLine ( ImVec2 ( 1920 / 2, 1080 ), ImVec2 ( head_screen.x, head_screen.y ), visual_color, 1.f );
			}
		}
	}
}