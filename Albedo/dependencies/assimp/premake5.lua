project "Assimp"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        -- "include/assimp/Importer.hpp",
        -- "include/assimp/postprocess.h",
        -- "include/assimp/scene.h",
        "include/assimp/**.h",
        "include/assimp/**.hpp",
        "include/assimp/**.inl"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
