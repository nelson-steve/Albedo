#pragma once

namespace Albedo {

	enum class DrawType
	{
		Albedo_POINTS,
		Albedo_LINE_STRIP,
		Albedo_LINE_LOOP,
		Albedo_LINES,
		Albedo_LINE_STRIP_ADJACENCY,
		Albedo_LINES_ADJACENCY,
		Albedo_TRIANGLE_STRIP,
		Albedo_TRIANGLE_FAN,
		Albedo_TRIANGLES,
		Albedo_TRIANGLE_STRIP_ADJACENCY,
		Albedo_TRIANGLES_ADJACENCY,
		Albedo_PATCHES
	};

	struct RendererConfig
	{
		DrawType Type = DrawType::Albedo_TRIANGLES;
		bool PolygonMode = false;
	};

}