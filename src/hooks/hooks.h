#pragma once
#include <cstdint>
#include <iostream>

#include "../directx/drawing.h"
#include "../hacks/esp/esp.h"

namespace hooks
{
	auto detour_32(void* src, void* dst, size_t size);
	void patch(void* src, void* dst, size_t size);
	uintptr_t trampoline_32(void* src, void* dst, size_t size);

	inline std::uint8_t o_endscene_bytes[7]{ };
	inline dx::endscene_t o_endscene = nullptr;
	void APIENTRY hk_endscene(IDirect3DDevice9* device);
}

