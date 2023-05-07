project "mono"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"include/**.h"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        systemversion "latest"
        -- staticruntime "on"

    filter "configurations:Debug"
        buildoptions "/MTd"
        runtime "Debug"
        -- symbols "on"

    filter "configurations:Release"
        buildoptions "/MT"
        runtime "Release"
        -- optimize "on"
