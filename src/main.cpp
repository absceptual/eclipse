// C++ headers
#include <chrono>
#include <stdio.h>
#include <thread>

// Windows and other external headers
#include <Windows.h>
#include "sdk/globals.h"

#include "hooks/hooks.h"

#pragma warning(disable : 26495)

inline FILE* startup()
{
	AllocConsole();
	FILE* output;
	freopen_s(&output, "CONOUT$", "w", stdout);

	return output;
}

inline DWORD cleanup(HMODULE mod, FILE* output)
{
	fclose(output);
	FreeConsole();
	FreeLibraryAndExitThread(mod, 0);

	return 0;
}

DWORD __stdcall entry_point(HMODULE mod)
{
	auto output = startup();

	globals::update();
	netvars::dump();


	if (dx::get_d3d9_device(dx::vmt, sizeof(dx::vmt))) 
	{
		memcpy(hooks::o_endscene_bytes, dx::vmt[42], 7);
		hooks::o_endscene = reinterpret_cast<dx::endscene_t>(hooks::trampoline_32(dx::vmt[42], hooks::hk_endscene, 7));
	}

	while (!GetAsyncKeyState(VK_END) & 1)
	{	
		if (GetAsyncKeyState(VK_F1) & 1)
			settings::esp_enabled = !settings::esp_enabled;

		if (GetAsyncKeyState(VK_F2) & 1)
			settings::teammates = !settings::teammates;

		if (GetAsyncKeyState(VK_F3) & 1)
			settings::snaplines = !settings::snaplines;

		if (GetAsyncKeyState(VK_F4) & 1)
			settings::s_box = !settings::s_box;

		if (GetAsyncKeyState(VK_F5) & 1)
			settings::box = !settings::box;

		if (GetAsyncKeyState(VK_F8) & 1)
			settings::snap_from = esp::snap_from::bottom;

		if (GetAsyncKeyState(VK_F9) & 1)
			settings::snap_from = esp::snap_from::top;

		if (GetAsyncKeyState(VK_F6) & 1)
			settings::snap_to = esp::snap_to::head;

		if (GetAsyncKeyState(VK_F7) & 1)
			settings::snap_to = esp::snap_to::origin;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	if (dx::vmt)
		hooks::patch(hooks::o_endscene_bytes, reinterpret_cast<uintptr_t*>(dx::vmt[42]), 7);

	return cleanup(mod, output);
}

BOOL WINAPI DllMain(
	HINSTANCE handle,
	DWORD reason,
	LPVOID lpvReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		auto thread = CreateThread(0x0, 0x0, reinterpret_cast<LPTHREAD_START_ROUTINE>(entry_point), handle, 0, nullptr);
		if (thread)
			CloseHandle(thread);
	}
	return TRUE;
}