#include "drawing.h"

void drawing::draw_filled_rect(int x, int y, int width, int height, D3DCOLOR color)
{
	if (!dx::device)
		return;

	D3DRECT rectangle{ x, y, x + width, y + height };
	dx::device->Clear(1, &rectangle, D3DCLEAR_TARGET, color, 0, 0);
}

void drawing::draw_rect(D3DXVECTOR2 tl, D3DXVECTOR2 tr, D3DXVECTOR2 bl, D3DXVECTOR2 br, D3DCOLOR color, float thickness)
{
	if (!dx::device)
		return;

	drawing::draw_line(tl, tr, color, thickness);
	drawing::draw_line(tr, br, color, thickness);
	drawing::draw_line(br, bl, color, thickness);
	drawing::draw_line(bl, tl, color, thickness);
}

void drawing::draw_line(D3DXVECTOR2 from, D3DXVECTOR2 to, D3DCOLOR color, float thickness)
{
	if (!drawing::p_line)
		D3DXCreateLine(dx::device, &drawing::p_line);

	drawing::p_line->SetWidth(thickness);
	drawing::p_line->Begin();

	const D3DXVECTOR2 positions[]{ from, to };
	drawing::p_line->Draw(positions, 2, color);

	drawing::p_line->End();
}

void drawing::draw_text(const wchar_t* text, int x, int y, D3DCOLOR color, DWORD format)
{
	if (!drawing::p_font)
		D3DXCreateFont(dx::device, drawing::font_height, drawing::font_width, FW_BOLD	, 14, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &drawing::p_font);

	RECT position;
	position.left = x;
	position.top = y;
	position.right = x + (wcslen(text) * drawing::font_width) + 10;
	position.bottom = y + (wcslen(text) * drawing::font_height)	;

	drawing::p_font->DrawTextW(NULL, text, wcslen(text), &position, format, color);


}


