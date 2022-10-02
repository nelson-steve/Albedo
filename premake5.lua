workspace "Albedo"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Albedo"
	location "Albedo"
	kind "SharedLib"
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
		"%{prj.name}/dependencies/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"Albedo_Platform_Windows",
			"Albedo_Build_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "Albedo_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "Albedo_DIST"
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
		"Albedo/src"
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
		symbols "On"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		optimize "On"