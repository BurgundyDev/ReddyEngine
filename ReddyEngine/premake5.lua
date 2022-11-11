project "ReddyEngine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files
   	{
		"src/**.h",
		"src/**.hpp",
	  	"src/**.c",
	  	"src/**.cpp",
	  	"include/**.h",
	  	"include/**.hpp",
	  	"include/**.c",
	  	"include/**.cpp",

		-- ImGui backends
		"imgui/backends/imgui_impl_sdl.cpp",
		"imgui/backends/imgui_impl_opengl3.cpp",
		"tinyfiledialogs/tinyfiledialogs.c"
   	}

   includedirs
	{
		"src",
		"include",
	  	"%{IncludeDir.stb}",
	  	"%{IncludeDir.SDL2}",
	  	"%{IncludeDir.glm}",
	  	"%{IncludeDir.imgui}",
	  	"%{IncludeDir.jsoncpp}",
	  	"%{IncludeDir.lib_json}",
	  	"%{IncludeDir.tinyxml2}",
	  	"%{IncludeDir.box2d}",
	  	"%{IncludeDir.box2d_source}",
		"lua"
	}

   links
	{
	  	"ImGui",
	  	"SDL2",
	  	"jsoncpp",
	  	"tinyxml2",
	  	"box2d",
		"lua",
		"opengl32.lib"
	}

	filter "system:Windows"
		characterset ("MBCS")

	filter "configurations:Debug"
		defines { "DEBUG", "REDDY_DEBUG" }
		runtime "Debug"
		symbols "On"
  
	filter "configurations:Release"
		defines { "RELEASE", "REDDY_RELEASE" }
		runtime "Release"
		symbols "Off"
		optimize "On"
	 
	filter "configurations:Final"
		defines { "FINAL", "REDDY_FINAL" }
		runtime "Release"
		symbols "Off"
		optimize "On"