workspace "ReddyGame"

   configurations
   {
      "Debug",
      "Release"
   }

   flags
	{
		"MultiProcessorCompile"
	}

   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

   architecture "x86_64"

   filter "system:Windows"
      defines { "WIN32" }
      systemversion "latest"

group "Dependencies"
	include "premake"
   include "ReddyEngine/SDL2-Premake"
   include "ReddyEngine/imgui"
   include "ReddyEngine/stb-premake"
   include "ReddyEngine/glm"
   include "ReddyEngine/jsoncpp"
group ""
include "ReddyEngine"
include "ReddyGame"

-- Dependencies

IncludeDir = {}
IncludeDir["SDL2"] = "%{wks.location}/ReddyEngine/SDL2-Premake/include"
IncludeDir["imgui"] = "%{wks.location}/ReddyEngine/imgui"
IncludeDir["stb"] = "%{wks.location}/ReddyEngine/stb-premake"
IncludeDir["glm"] = "%{wks.location}/ReddyEngine/glm"
IncludeDir["jsoncpp"] = "%{wks.location}/ReddyEngine/jsoncpp/include"
IncludeDir["lib_json"] = "%{wks.location}/ReddyEngine/jsoncpp/src/lib_json"
