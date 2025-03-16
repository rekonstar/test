#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui.h"
#include <map>

class drawer {
public:

	void draw_line ( const ImVec2& from, const ImVec2& to, ImColor color, float thickness )
	{
		ImGui::GetForegroundDrawList ( )->AddLine ( from, to, color, thickness );
	}
	void draw_box ( const float x, const float y, const float width, const float height, const ImColor color, float thickness )
	{
		draw_line ( ImVec2 ( x, y ), ImVec2 ( x + width, y ), color, thickness );
		draw_line ( ImVec2 ( x, y ), ImVec2 ( x, y + height ), color, thickness );
		draw_line ( ImVec2 ( x, y + height ), ImVec2 ( x + width, y + height ), color, thickness );
		draw_line ( ImVec2 ( x + width, y ), ImVec2 ( x + width, y + height ), color, thickness );
	}

	auto draw_rect_filled ( float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags ) -> VOID
	{
		ImGui::GetForegroundDrawList ( )->AddRectFilled ( ImVec2 ( x0, y0 ), ImVec2 ( x1, y1 ), color, rounding, rounding_corners_flags );
	}
};

drawer draw;
