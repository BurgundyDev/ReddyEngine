project "ReddyEngine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

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
	  	"include/**.cpp"
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

   filter "configurations:Debug"
	  defines { "DEBUG" }
	  runtime "Release"
	  runtime "Debug"
	  symbols "On"

   filter "configurations:Release"
	  defines { "RELEASE" }
	  symbols "Off"
	  optimize "On"