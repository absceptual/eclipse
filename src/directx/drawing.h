#pragma once
#include "dx.h"



namespace drawing
{
	inline ID3DXLine* p_line;
	inline ID3DXFont* p_font;

	inline int font_height = 12;
	inline int font_width = 7;
	
	void draw_filled_rect(int x, int y, int width, int height, D3DCOLOR color);
	void draw_rect(int x, int y, int width, int height, D3DCOLOR color);
	void draw_rect(D3DXVECTOR2 tl, D3DXVECTOR2 tr, D3DXVECTOR2 bl, D3DXVECTOR2 br, D3DCOLOR color, float thickness);

	void draw_line(D3DXVECTOR2 from, D3DXVECTOR2 to, D3DCOLOR color, float thickness);
	void draw_text(const wchar_t* text, int x, int y, D3DCOLOR color, DWORD format = DT_LEFT);
}