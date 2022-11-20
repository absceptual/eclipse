#pragma once
#include <vector>

#include "../../sdk/globals.h"
#include "../../directx/drawing.h"

#define OPT_COLOR(flag) flag ? D3DCOLOR_XRGB(0, 255, 0) : D3DCOLOR_XRGB(255, 0, 0)

namespace esp
{
	enum snap_to
	{
		head,
		origin
	};

	enum snap_from
	{
		top,
		bottom
	};

	inline float view_matrix[16]{ };
	void run();
	bool world_to_screen(Vector3 position, D3DXVECTOR2& screen);

	void draw_snapline(CEntity* entity);
	void draw_box(CEntity* entity);
	void draw_3d_box(CEntity* entity);
	void draw_bones(CEntity* entity);
	void render_menu();
}

namespace settings
{
	inline bool esp_enabled{ false };
	inline bool s_box{ false };
	inline bool box{ false };
	inline bool teammates{ false };

	inline bool snaplines{ true };
	inline esp::snap_to snap_to{ esp::snap_to::origin };
	inline esp::snap_from snap_from{ esp::snap_from::top };
	inline D3DCOLOR snap_color{ D3DCOLOR_XRGB(0, 255, 0) };
	inline float snap_thick{ 3.0f };


}