#pragma once
#include <cstdint>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include "../../utils/kernel/kernel.h"
#include "../../utils/math/vectors.h"
#include "offsets_steam.h"
#include "lele_core.h"

struct client_bits_t
{
    int array [ 7 ];
};

namespace engine {

    namespace player {
        inline auto local_player_index ( QWORD client_base, QWORD client_info ) -> int
        {
            auto index = Driver::Read<uintptr_t> ( client_info + offsets::local_index );
            return Driver::Read<int> ( index + offsets::local_index_pos );
        }


        inline auto is_player_valid ( QWORD player_address ) -> bool
        {
            return Driver::Read<bool> ( player_address + offsets::player::valid );
        }

        inline auto get_position ( QWORD player_address ) -> Vector3
        {
            auto local_position = Driver::Read<uintptr_t> ( player_address + offsets::player::pos );
            return Driver::Read<Vector3> ( local_position + 0x50 );
        }
    }

    namespace game {
        inline auto is_user_in_game ( ) -> bool
        {
            return Driver::Read<int> ( mem::base_address + offsets::game_mode ) > 1;
        }

        inline auto player_count ( ) -> int
        {
            return Driver::Read<int> ( mem::base_address + offsets::game_mode );
        }

        auto retrieve_camera_posistion ( ) -> Vector3
        {
            auto player_camera = Driver::Read<uintptr_t> ( mem::base_address + offsets::camera_base );
            if ( !player_camera )
                return {};

            Vector3 received = Driver::Read<Vector3> ( player_camera + offsets::camera_pos );
            return received;
        }
        auto get_angles ( ) -> Vector2
        {
            auto camera_angle = Driver::Read<uintptr_t> ( mem::base_address + offsets::camera_base );
            if ( !camera_angle )
                return {};

            Vector2 received = Driver::Read<Vector2> ( camera_angle + offsets::camera_pos + 0xC );
            return received;
        }

        auto world_to_screen ( Vector3 world_location, Vector2& out, Vector3 camera_pos, int screen_width, int screen_height, Vector2 fov, Vector3 matricies [ 3 ] ) -> bool
        {
            auto local = world_location - camera_pos;


            auto trans = Vector3 {
                local.Dot ( matricies [ 1 ] ),
                local.Dot ( matricies [ 2 ] ),
                local.Dot ( matricies [ 0 ] )
            };

            if ( trans.z < 0.01f ) {
                return false;
            }

            out.x = ( ( float ) screen_width / 2.0 ) * ( 1.0 - ( trans.x / fov.x / trans.z ) );
            out.y = ( ( float ) screen_height / 2.0 ) * ( 1.0 - ( trans.y / fov.y / trans.z ) );

            if ( out.x < 1 || out.y < 1 || ( out.x > decrypt_refdef->ref_def_nn.width ) || ( out.y > decrypt_refdef->ref_def_nn.height ) ) {
                return false;
            }

            return true;
        }

        auto w2s ( Vector3 world_position, Vector2& screen_position ) -> bool
        {
            return world_to_screen ( world_position, screen_position, retrieve_camera_posistion ( ), decrypt_refdef->ref_def_nn.width, decrypt_refdef->ref_def_nn.height, decrypt_refdef->ref_def_nn.tan_half_fov, decrypt_refdef->ref_def_nn.axis );
        }

        int is_visible ( uint32_t p_index )
        {
            auto sighted_enemy_fools = Driver::Read<client_bits_t> ( ( uintptr_t ) decrypt_client_info ( ) + offsets::visible_bit );
            auto bitmask = 0x80000000 >> ( p_index & 0x1F );
            return sighted_enemy_fools.array [ p_index >> 5 ] & bitmask;
        }

        uint8_t team_id_check ( uintptr_t player ) {
            return Driver::Read<uint8_t> ( player + offsets::player::team );
        }
    }


    namespace bones {
        auto get_bone ( const uintptr_t pointer, const Vector3& bone_position, const int Bone ) -> Vector3
        {
            Vector3 position = Driver::Read<Vector3> ( pointer + ( ( uint64_t ) Bone * 0x20 ) + 0x10 );
            position.x += bone_position.x;
            position.y += bone_position.y;
            position.z += bone_position.z;
            return position;
        }

        auto retrieve_bone_position_vec ( const uintptr_t Client_Information ) -> Vector3
        {
            return Driver::Read<Vector3> ( Client_Information + offsets::bone::bone_base );
        }

        auto bone_pointer ( uint64_t base, uint64_t index ) -> uint64_t
        {
            auto bone = Driver::Read<uintptr_t> ( base + ( index * offsets::bone::size ) + offsets::bone::offset );
            return bone;
        }

        bool bones_to_screen ( const Vector3* BonePosArray, Vector2* ScreenPosArray, const long Count )
        {
            for ( long i = 0; i < Count; ++i )
            {
                if ( !game::w2s ( BonePosArray [ i ], ScreenPosArray [ i ] ) )
                    return false;
            }
            return true;
        }
    }
    

    namespace utils {
        auto distace_to_str ( float dist ) -> std::string
        {
            std::stringstream strs;
            strs << dist;
            std::string temp_str = strs.str ( );
            const char* text = ( const char* ) temp_str.c_str ( );
            return ( std::string ) text + "m";
        }


        auto units_to_m ( float units ) -> float {
            return units * 0.0254;
        }
    }

}