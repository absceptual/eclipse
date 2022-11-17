// C++ headers
#include <chrono>
#include <stdio.h>
#include <thread>

// Windows and other external headers
#include <Windows.h>
#include "sdk/entity.h"

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
	auto entitylist = interfaces::get<IClientEntityList>("client.dll", "VClientEntityList003");

	netvars::dump();
	printf("[+] dumped netvars!\n");

	while (!GetAsyncKeyState(VK_END) & 1)
	{	
		auto entity = reinterpret_cast<CEntity*>(entitylist->GetClientEntity(2));
		if (entity)
			printf("[?] entity 2 health: %d\n", entity->health());

			


		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

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