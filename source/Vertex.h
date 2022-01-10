#pragma once
#include "EMath.h"

struct IVertex
{
	Elite::FPoint3 position;
	Elite::FPoint2 uv;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
};

struct Vertex
{
	Elite::FPoint4 position{};
	Elite::RGBColor color{ 1,1,1 };
	Elite::FVector2 uv{};
	Elite::FVector3 normal{};
	Elite::FVector3 tangent{};
	float weight{}; // barycentric
};