workspace "Albedo"
	architecture "x64"
	startproject "AlbedoEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] 		= "%{wks.location}/Albedo/dependencies/GLFW/include"
IncludeDir["Glad"] 		= "%{wks.location}/Albedo/dependencies/Glad/include"
IncludeDir["imgui"] 	= "%{wks.location}/Albedo/dependencies/imgui"
IncludeDir["glm"] 		= "%{wks.location}/Albedo/dependencies/glm"
IncludeDir["stb_image"] = "%{wks.location}/Albedo/dependencies/stb_image"
IncludeDir["entt"] 		= "%{wks.location}/Albedo/dependencies/entt/include"
IncludeDir["FileWatch"] = "%{wks.location}/Albedo/dependencies/FileWatch/include"
IncludeDir["tinyobj"] 	= "%{wks.location}/Albedo/dependencies/tinyobj/include"
IncludeDir["mono"] 	= "%{wks.location}/Albedo/dependencies/mono/include"
IncludeDir["yaml_cpp"]  = "%{wks.location}/Albedo/dependencies/yaml-cpp/include"
IncludeDir["ImGuizmo"]  = "%{wks.location}/Albedo/dependencies/ImGuizmo"
IncludeDir["Physx"]  	= "%{wks.location}/Albedo/dependencies/Physx/include"
IncludeDir["Box2d"]  	= "%{wks.location}/Albedo/dependencies/Box2d/include"
IncludeDir["Assimp"]  	= "%{wks.location}/Albedo/dependencies/assimp/include"
IncludeDir["tinygltf"]  	= "%{wks.location}/Albedo/dependencies/tinygltf/include"
IncludeDir["reactphysics3d"]  	= "%{wks.location}/Albedo/dependencies/reactphysics3d/include"

LibraryDir = {}
LibraryDir["reactphysics3d"] 		= "%{wks.location}/Albedo/dependencies/reactphysics3d/lib/%{cfg.buildcfg}"
LibraryDir["Physx"] 		= "%{wks.location}/Albedo/dependencies/Physx/lib/%{cfg.buildcfg}"
LibraryDir["Box2d"] 		= "%{wks.location}/Albedo/dependencies/Box2d/lib/%{cfg.buildcfg}"
LibraryDir["mono"] 		= "%{wks.location}/Albedo/dependencies/mono/lib/%{cfg.buildcfg}"
LibraryDir["Assimp"] 		= "%{wks.location}/Albedo/dependencies/assimp/libs"

Library = {}
Library["Box2d"] 				   = "%{LibraryDir.Box2d}/box2d.lib"
Library["Physx"] 				   = "%{LibraryDir.Physx}/PhysX_static_64.lib"
Library["PhysXPvdSDK"] 			   = "%{LibraryDir.Physx}/PhysXPvdSDK_static_64.lib"
Library["PhysXCommon"] 			   = "%{LibraryDir.Physx}/PhysXCommon_static_64.lib"
Library["PhysXCooking"] 		   = "%{LibraryDir.Physx}/PhysXCooking_static_64.lib"
Library["PhysXVehicle"] 		   = "%{LibraryDir.Physx}/PhysXVehicle_static_64.lib"
Library["PhysXExtensions"] 		   = "%{LibraryDir.Physx}/PhysXExtensions_static_64.lib"
Library["PhysXFoundation"] 		   = "%{LibraryDir.Physx}/PhysXFoundation_static_64.lib"
Library["PhysXCharacterKinematic"] = "%{LibraryDir.Physx}/PhysXCharacterKinematic_static_64.lib"
Library["mono_sgen"] 			   = "%{LibraryDir.mono}/mono-2.0-sgen.lib"
Library["mono_eglib"] 				   = "%{LibraryDir.mono}/eglib.lib"
Library["mono_libgc"] 				   = "%{LibraryDir.mono}/libgcmonosgen.lib"
Library["mono_libmini"] 				   = "%{LibraryDir.mono}/libmini-sgen.lib"
Library["mono_runtime"] 				   = "%{LibraryDir.mono}/libmonoruntime-sgen.lib"
Library["mono_utils"] 				   = "%{LibraryDir.mono}/libmonoutils.lib"
Library["mono_posix"] 				   = "%{LibraryDir.mono}/MonoPosixHelper.lib"
Library["Assimp"] 				   = "%{LibraryDir.Assimp}/assimp.lib"
Library["reactphysics3d"] 				   = "%{LibraryDir.reactphysics3d}/reactphysics3d.lib"

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
		"%{prj.name}/dependencies/stb_image/**.cpp",
		"%{prj.name}/dependencies/FileWatch/include/**.hpp",
		"%{prj.name}/dependencies/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/dependencies/ImGuizmo/ImGuizmo.cpp",
		"%{prj.name}/dependencies/tinyobj/include/tiny_obj_loader.h",
		"%{prj.name}/dependencies/tinyobj/include/tiny_obj_loader.cpp",
		"%{prj.name}/dependencies/tinygltf/include/tiny_gltf.h",
		"%{prj.name}/dependencies/tinygltf/include/json.hpp",
		"%{prj.name}/dependencies/tinygltf/include/stb_image_write.h",
		"%{prj.name}/dependencies/tinygltf/tiny_gltf.cpp"
	}

	defines
	{
		"PX_PHYSX_STATIC_LIB",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.tinyobj}",
		"%{IncludeDir.tinygltf}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.FileWatch}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Physx}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.Box2d}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.Assimp}",
		"%{prj.name}/dependencies/spdlog/include"
	}

	links
	{
		-- YAML
		"yaml-cpp",
		-- ImGUI
		"imgui",
		-- OpenGL
		"Glad",
		"GLFW",
		"opengl32.lib",
		-- Box2d
		"%{Library.Box2d}",
		-- reactphysics3d
		"%{Library.reactphysics3d}",
		-- Physx
		"%{Library.Physx}",
		"%{Library.PhysXPvdSDK}",
		"%{Library.PhysXCommon}",
		"%{Library.PhysXCooking}",
		"%{Library.PhysXVehicle}",
		"%{Library.PhysXExtensions}",
		"%{Library.PhysXFoundation}",
		"%{Library.PhysXCharacterKinematic}",
		-- mono libs
		"%{Library.mono_sgen}",
		"%{Library.mono_eglib}",
		"%{Library.mono_libgc}",
		"%{Library.mono_libmini}",
		"%{Library.mono_runtime}",
		"%{Library.mono_utils}",
		"%{Library.mono_posix}",
		-- mono DLLs
		"mono-2.0-sgen.dll",
		"MonoPosixHelper.dll",
		-- Assimp
		"%{Library.Assimp}",
		"assimp.dll"
	}

	filter "files:Albedo/dependencies/ImGuizmo/ImGuizmo.cpp"
	flags { "NoPCH" }
	filter "files:Albedo/dependencies/tinyobj/include/tiny_obj_loader.cpp"
	flags { "NoPCH" }
	filter "files:Albedo/dependencies/stb_image/stb_image.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"Albedo_Platform_Windows",
			"Albedo_Build_DLL",
			"GLFW_INCLUDE_NONE",
		}

	filter "configurations:Debug"
		defines "Albedo_DEBUG"
		buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MT"
		optimize "on"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MT"
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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Physx}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.Box2d}",
		"%{IncludeDir.tinyobj}",
		"%{IncludeDir.tinygltf}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}"
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
		buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MT"
		optimize "on"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MT"
		optimize "on"

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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.tinyobj}",
		"%{IncludeDir.tinygltf}",
		"%{IncludeDir.Physx}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.Assimp}",
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
		buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		defines "Albedo_RELEASE"
		buildoptions "/MT"
		optimize "on"

	filter "configurations:Dist"
		defines "Albedo_DIST"
		buildoptions "/MT"
		optimize "on"

project "AlbedoScripting"
	location "AlbedoScripting" 
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/AlbedoEditor/Resources/Scripts")
	objdir ("%{wks.location}/AlbedoEditor/Resources/Scripts/Intermediates")

	files 
	{
		"%{prj.name}/Source/Albedo/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"