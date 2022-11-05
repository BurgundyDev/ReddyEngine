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
	  	"%{IncludeDir.box2d_source}"
	}

   links
	{
	  	"ImGui",
	  	"SDL2",
	  	"jsoncpp",
	  	"tinyxml2",
	  	"box2d",
		"opengl32.lib"
	}

	filter "system:Windows"
		characterset ("MBCS")

   filter "configurations:Debug"
	  defines { "DEBUG" }
	  runtime "Release"
	  runtime "Debug"
	  symbols "On"

   filter "configurations:Release"
	  defines { "RELEASE" }
	  symbols "Off"
	  optimize "On"