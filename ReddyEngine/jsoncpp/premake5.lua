project "jsoncpp"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/lib_json/**.h",
    	"src/lib_json/**.hpp",
    	"src/lib_json/**.c",
    	"src/lib_json/**.cpp",
        "include/**.h",
    	"include/**.hpp",
    	"include/**.c",
    	"include/**.cpp"
	}

	includedirs
	{
		"include"
	}
	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"