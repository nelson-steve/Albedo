workspace "Albedo"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Albedo/dependencies/GLFW/include"
IncludeDir["Glad"] = "Albedo/dependencies/Glad/include"
IncludeDir["ImGui"] = "Albedo/dependencies/imgui"
IncludeDir["glm"] = "Albedo/dependencies/glm"

include "Albedo/dependencies/GLFW"
include "Albedo/dependencies/Glad"
include "Albedo/dependencies/imgui"

project "Albedo"
	location "Albedo"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "AlbedoPreCompiledHeader.h"
	pchsource "Albedo/src/AlbedoPreCompiledHeader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/dependencies/glm/glm/**.hpp",
		"%{prj.name}/dependencies/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/dependencies/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"Albedo_Platform_Windows",
			"Albedo_Build_DLL",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_GLAD"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "Albedo_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MDd"
		optimize "On"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MDd"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Albedo/dependencies/spdlog/include",
		"Albedo/src",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Albedo"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"Albedo_Platform_Windows"
		}

	filter "configurations:Debug"
		defines "Albedo_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MDd"
		optimize "On"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MDd"
		optimize "On"