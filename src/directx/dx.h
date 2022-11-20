#pragma once
#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")



namespace dx
{

	using endscene_t = HRESULT(APIENTRY*)(IDirect3DDevice9* device);

	inline void* vmt[119]{ };
	inline IDirect3DDevice9* device;

	inline int window_height, window_width;
	static HWND window;
	
	auto get_process_window();
	bool get_d3d9_device(void** vmt, size_t size);
}