#pragma once
#include "entity.h"

namespace globals
{
	// Core modules
	inline uintptr_t engine{ 0x0 };
	inline uintptr_t client{ 0x0 };
	

	// Classes
	inline CEntity* localplayer{ nullptr };
	inline IClientEntityList* entitylist{ nullptr };

	// Offsets
	inline size_t view_matrix = 0x4DEDCA4;
	inline size_t bone_matrix = 0x26A8;

	// Globals
	inline uintptr_t client_state{ 0x0 };
	inline int* max_players{ nullptr };

	// Functions
	inline void update();
}

void globals::update(void)
{
	globals::client = reinterpret_cast<uintptr_t>(GetModuleHandle(L"client.dll"));
	globals::engine = reinterpret_cast<uintptr_t>(GetModuleHandle(L"engine.dll"));

	globals::entitylist = interfaces::get<IClientEntityList>("client.dll", "VClientEntityList003");
	globals::localplayer = reinterpret_cast<CEntity*>(globals::client + 0xDE7964);

	globals::client_state = *reinterpret_cast<uintptr_t*>(globals::engine + 0x59F194);
	globals::max_players = reinterpret_cast<int*>(globals::client_state + 0x388);
}