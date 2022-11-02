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
      systemversion "latest"

group "Dependencies"
	include "premake"
group ""
include "ReddyEngine"
include "ReddyGame"

-- Dependencies

IncludeDir = {}