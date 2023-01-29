project "SDL2main"
	kind          "StaticLib"
	language      "C"
	systemversion "latest"
	vectorextensions "SSE" -- Necessary to run x32.

	-- Output directory
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir  ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	flags 
	{
		"NoRuntimeChecks", 
		"NoBufferSecurityCheck"
	}

	includedirs "include"

	files
	{
		"src/main/windows/*.c"
	}

	defines 
	{
		"_WINDOWS", 
		"WIN32"
	}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "On"