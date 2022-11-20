#include "esp.h"

void esp::run()
{
	for (int i = 1; i < *globals::max_players; ++i)
	{
		auto entity = reinterpret_cast<CEntity*>(globals::entitylist->GetClientEntity(i));
		if (!entity || !entity->is_valid() || uintptr_t(entity) == *reinterpret_cast<uintptr_t*>(globals::localplayer))
			continue;

		//if (entity->team() == globals::localplayer->team() && !settings::teammates)
		//	continue;

		if (settings::snaplines)
			esp::draw_snapline(entity);

		if (settings::s_box)
			esp::draw_box(entity);

		if (settings::box)
			esp::draw_3d_box(entity);
		

		for (int i = 0; i < MAXSTUDIOBONES; ++i)
		{
			D3DXVECTOR2 screen;
			if (esp::world_to_screen(entity->get_bone_position(i), screen))
			{
				char buffer[128];
				_itoa_s(i, buffer, 10);

				wchar_t w_buffer[128];
				size_t written;
				mbstowcs_s(&written, w_buffer, buffer, sizeof(w_buffer));

				drawing::draw_text(w_buffer, screen.x, screen.y, D3DCOLOR_XRGB(255, 255, 255));
			}

		}

		std::vector<int> bones{ 76, 4, 69  };
		for (int i = 0; i < bones.size() + 1; ++i)
		{
			if (i == bones.size())
				break;

			D3DXVECTOR2 screen, screen2;
			if (esp::world_to_screen(entity->get_bone_position(bones[i]), screen) && esp::world_to_screen(entity->get_bone_position(bones[i + 1]), screen2))
			{
				char buffer[128];
				_itoa_s(i, buffer, 10);

				wchar_t w_buffer[128];
				size_t written;
				mbstowcs_s(&written, w_buffer, buffer, sizeof(w_buffer));

				drawing::draw_line(screen, screen2, D3DCOLOR_XRGB(255, 255, 255), 2.0f);
				drawing::draw_text(w_buffer, screen.x, screen.y, D3DCOLOR_XRGB(255, 255, 255));
			}
		}
	}
}

bool esp::world_to_screen(Vector3 position, D3DXVECTOR2& screen)
{
	memcpy(esp::view_matrix, reinterpret_cast<void*>(globals::client + globals::view_matrix), sizeof(float) * 16);
	Vector4 proj;

	// mv * p to get mvproj
	proj.x = position.x * esp::view_matrix[0] + position.y * esp::view_matrix[1] + position.z * esp::view_matrix[2] + esp::view_matrix[3];
	proj.y = position.x * esp::view_matrix[4] + position.y * esp::view_matrix[5] + position.z * esp::view_matrix[6] + esp::view_matrix[7];
	proj.z = position.x * esp::view_matrix[8] + position.y * esp::view_matrix[9] + position.z * esp::view_matrix[10] + esp::view_matrix[11];
	proj.w = position.x * esp::view_matrix[12] + position.y * esp::view_matrix[13] + position.z * esp::view_matrix[14] + esp::view_matrix[15];
	
	if (proj.w < 0.1f)
		return false;

	// clip coordinates
	Vector3 ndc;
	ndc.x = proj.x / proj.w;
	ndc.y = proj.y / proj.w;

	screen.x = (dx::window_width / 2 * ndc.x) + (ndc.x + dx::window_width / 2);
	screen.y = -(dx::window_height / 2 * ndc.y) + (ndc.y + dx::window_height / 2);
	return true;
}

void esp::draw_snapline(CEntity* entity)
{
	Vector3 to;
	D3DXVECTOR2 from;

	switch (settings::snap_from) 
	{
		case esp::snap_from::bottom:
			from = D3DXVECTOR2(dx::window_width / 2, dx::window_height);
			break;
		case esp::snap_from::top:
			from = D3DXVECTOR2(dx::window_width / 2, 0);
			break;
	}

	switch (settings::snap_to)
	{
	case esp::snap_to::origin:
		to = entity->origin();
		break;
	case esp::snap_to::head:
		to = entity->head();
		break;
	}

	D3DXVECTOR2 screen;
	if (esp::world_to_screen(to, screen))
	{
		drawing::draw_line(from, screen, settings::snap_color, settings::snap_thick);
	}
}

void esp::draw_box(CEntity* entity)
{
	auto origin = entity->origin();
	auto head = entity->head();

	D3DXVECTOR2 s_origin, s_head;
	if (esp::world_to_screen(origin, s_origin) && esp::world_to_screen(head, s_head))
	{
		float height = abs(s_head.y - s_origin.y);
		
		D3DXVECTOR2 tl, tr, bl, br;
		tl.x = s_head.x - height / 4;
		tr.x = s_head.x + height / 4;
		tl.y = tr.y = s_head.y;

		bl.x = s_origin.x - height / 4;
		br.x = s_origin.x + height / 4;
		bl.y = br.y = s_origin.y;

		drawing::draw_rect(tl, tr, bl, br, D3DCOLOR_XRGB(255, 255, 255), 3.0f);
	}
}

void esp::draw_3d_box(CEntity* entity)
{
	static int length = 40;
	static int width  = 40;
	static int height = 80;

	Vector3 bottom[4]{ };
	Vector3 top[4]{ };

	Vector3 bl, br, btl, btr;
	Vector3 tl, tr, ttl, ttr;

	// All other positions will be offsets of the top (bottom) left
	bottom[0] = entity->get_bone_position(1);
	bottom[0].x += 20;
	bottom[0].y += 20;

	bottom[1] = bottom[0];
	bottom[1].y -= length;
	
	bottom[2] = bottom[1];
	bottom[2].x -= width;

	bottom[3] = bottom[2];
	bottom[3].y += length;

	// We are using the same positions but just adding height

	top[0] = bottom[0];
	top[1] = bottom[1];
	top[2] = bottom[2];
	top[3] = bottom[3];
	
	top[0].z = top[1].z = top[2].z = top[3].z += height;
	
	D3DXVECTOR2 s_bottom[4]{ };
	D3DXVECTOR2 s_top[4]{ };

	
	for (int i = 0; i < 4; ++i) {
		if (!world_to_screen(bottom[i], s_bottom[i]))
			return;
	}
		

	for (int i = 0; i < 4; ++i) {
		if (!world_to_screen(top[i], s_top[i]))
			return;
	}
	
	// Bottom box
	for (int i = 0; i < 4; ++i)
	{
		// Connect final line to original point
		if (i == 3)
			drawing::draw_line(s_bottom[3], s_bottom[0], D3DCOLOR_XRGB(255, 255, 255), 3.0f);
		else
			drawing::draw_line(s_bottom[i], s_bottom[i + 1], D3DCOLOR_XRGB(255, 255, 255), 3.0f);
	}

	// Top box
	for (int i = 0; i < 4; ++i)
	{

		if (i == 3)
			drawing::draw_line(s_top[3], s_top[0], D3DCOLOR_XRGB(255, 255, 255), 3.0f);
		else
			drawing::draw_line(s_top[i], s_top[i + 1], D3DCOLOR_XRGB(255, 255, 255), 3.0f);
	}

	// Connect top to bottom
	for (int i = 0; i < 4; ++i)
	{
		drawing::draw_line(s_bottom[i], s_top[i], D3DCOLOR_XRGB(255, 255, 255), 3.0f);
	}

}

void esp::render_menu()
{
	if (!settings::esp_enabled)
		return;

	drawing::draw_text(L"[F1] Toggle Menu", (dx::window_width / 2) - 50 , 40, D3DCOLOR_XRGB(255, 255, 255), DT_LEFT);
	drawing::draw_text(L"[F2] Show Teammates", (dx::window_width / 2) - 50, 50, OPT_COLOR(settings::teammates), DT_LEFT);
	drawing::draw_text(L"[F3] Show Snaplines", (dx::window_width / 2) - 50, 60, OPT_COLOR(settings::snaplines), DT_LEFT);
	drawing::draw_text(L"[F4] Show ESP", (dx::window_width / 2) - 50, 70, OPT_COLOR(settings::s_box), DT_LEFT);
	drawing::draw_text(L"[F5] Show 3D Box", (dx::window_width / 2) - 50, 80, OPT_COLOR(settings::box), DT_LEFT);
	// drawing::draw_text(L"[F10] Show Bones", (dx::window_width / 2) - 50, 80, OPT_COLOR(settings::box), DT_LEFT);

	drawing::draw_text(L"[F6] Snap to Head", (dx::window_width / 2) - 50, 110, D3DCOLOR_XRGB(255, 255, 255), DT_LEFT);
	drawing::draw_text(L"[F7] Snap to Origin", (dx::window_width / 2) - 50, 120, D3DCOLOR_XRGB(255, 255, 255), DT_LEFT);
	drawing::draw_text(L"[F8] Snap from Bottom", (dx::window_width / 2) - 50, 130, D3DCOLOR_XRGB(255, 255, 255), DT_LEFT);
	drawing::draw_text(L"[F9] Snap from Top", (dx::window_width / 2) - 50, 140, D3DCOLOR_XRGB(255, 255, 255), DT_LEFT);
}