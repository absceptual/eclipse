#pragma once
#include "netvars.h"

struct Vector2 { float x, y; };
struct Vector3 { float x, y, z; };
struct Vector4 { float x, y, z, w; };

using matrix3x4_t = float[3][4];

// CEntity related offsets
constexpr size_t dormant = 0xED;
constexpr size_t view_offset = 0x110;

class CEntity
{
public:
	// Variables & offsets
	NETVAR(health, int, "DT_BasePlayer.m_iHealth")
	NETVAR(origin, Vector3, "DT_BaseEntity.m_vecOrigin")

	bool is_dormant() { return *reinterpret_cast<bool*>(uintptr_t(this) + dormant); }
	auto head()
	{
		// ud head alignment
		return get_bone_position(8);
	}

public:
	bool is_valid()
	{
		return !is_dormant() && health() >= 1;
	}

	Vector3 get_bone_position(int id)
	{
		Vector3 position;

		auto matrix = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + 0x26A8);
		position.x = *reinterpret_cast<float*>(matrix + 0x30 * id + 0x0C);
		position.y = *reinterpret_cast<float*>(matrix + 0x30 * id + 0x1C);
		position.z = *reinterpret_cast<float*>(matrix + 0x30 * id + 0x2C);
		return position;
	
	}
};