workspace "Albedo"
	architecture "x64"
	startproject "AlbedoEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	-- solution_items
	-- {
	-- 	".editorconfig"
	-- }

	-- flags
	-- {
	-- 	"MultiProcessorCompile"
	-- }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] 		= "%{wks.location}/Albedo/dependencies/GLFW/include"
IncludeDir["Glad"] 		= "%{wks.location}/Albedo/dependencies/Glad/include"
IncludeDir["imgui"] 	= "%{wks.location}/Albedo/dependencies/imgui"
IncludeDir["glm"] 		= "%{wks.location}/Albedo/dependencies/glm"
IncludeDir["stb_image"] = "%{wks.location}/Albedo/dependencies/stb_image"
IncludeDir["entt"] 		= "%{wks.location}/Albedo/dependencies/entt/include"
IncludeDir["yaml_cpp"]  = "%{wks.location}/Albedo/dependencies/yaml-cpp/include"

group "Dependencies"
	include "Albedo/dependencies/GLFW"
	include "Albedo/dependencies/Glad"
	include "Albedo/dependencies/imgui"
	include "Albedo/dependencies/yaml-cpp"
group ""

project "Albedo"
	location "Albedo"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "AlbedoPreCompiledHeader.h"
	pchsource "Albedo/src/AlbedoPreCompiledHeader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/dependencies/glm/glm/**.hpp",
		"%{prj.name}/dependencies/glm/glm/**.inl",
		"%{prj.name}/dependencies/stb_image/**.h",
		"%{prj.name}/dependencies/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"Albedo_DEBUG"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/dependencies/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}"
	}

	links
	{
		"GLFW",
		"Glad",
		"imgui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"Albedo_Platform_Windows",
			"Albedo_Build_DLL",
			"GLFW_INCLUDE_NONE",
		}

		--postbuildcommands
		--{
		--	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		--}

	filter "configurations:Debug"
		defines "Albedo_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MDd"
		optimize "on"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MDd"
		optimize "on"

project "AlbedoEditor"
	location "AlbedoEditor"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "on"

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
		"Albedo/dependencies",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Albedo"
	}

	filter "system:windows"
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

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "on"

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
		"Albedo/dependencies",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Albedo"
	}

	filter "system:windows"
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