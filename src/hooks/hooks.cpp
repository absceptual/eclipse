
#include "hooks.h"

void hooks::patch(void* src, void* dst, size_t size)
{
	DWORD o_protection{ };
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &o_protection);

	memcpy(dst, src, size);
	VirtualProtect(dst, size, o_protection, &o_protection);
}

auto hooks::detour_32(void* src, void* func, size_t size)
{
	if (size < 5)
		return false;

	DWORD o_protection;
	if (!VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &o_protection))
		return false;

	memset(src, 0x90, size); // Set all bytes to 0x90 (NOP)
	*reinterpret_cast<uint8_t*>(src) = 0xE9; // jmp 
	*reinterpret_cast<uint32_t*>(uintptr_t(src) + 1) = uintptr_t(func) - uintptr_t(src) - 5; // rel32

	VirtualProtect(src, size, o_protection, &o_protection);
	return true;
}

uintptr_t hooks::trampoline_32(void* src, void* func, size_t size)
{
	// Create gateway
	auto gateway = reinterpret_cast<uintptr_t>(VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!gateway)
		return 0x0;

	DWORD o_protection;
	if (!VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &o_protection))
		return 0x0;

	memcpy(reinterpret_cast<void*>(gateway), src, size);
	*reinterpret_cast<uint8_t*>(uintptr_t(gateway) + size) = 0xE9; // jmp 
	*reinterpret_cast<uint32_t*>(uintptr_t(gateway + size) + 1) = uintptr_t(src) - uintptr_t(gateway) - 5; // rel32
	hooks::detour_32(src, func, size);

	return gateway;
}

void APIENTRY hooks::hk_endscene(IDirect3DDevice9* device)
{
	if (!dx::device)
		dx::device = device;

	
	D3DVIEWPORT9 viewport;
	dx::device->GetViewport(&viewport);

	dx::window_height = viewport.Height;
	dx::window_width = viewport.Width;

	esp::render_menu();
	esp::run();
	hooks::o_endscene(dx::device);
}

