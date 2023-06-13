project "AlbedoScripting"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir	 ("../AlbedoEditor/Resources/Scripts")
	objdir ("../AlbedoEditor/Resources/Scripts/Intermediates")

	files
	{
		"Source/Albedo/**.cs",
		"Properties/**.cs"
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
