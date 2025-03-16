#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"
#include "../../utils/global.h"
#include "../draw_helper/drawer.h"
#include "../../utils/protection/ex.h"

namespace menu {
	void menu ( ) {
	ImGui::SetNextWindowSize ( ImVec2 ( 50, 50 ) );
	ImGui::Begin ( E ( "SBAGGY MW3 BASE :)" ), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar );
		
		ImGui::End ( );
	}
}