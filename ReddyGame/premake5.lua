project "ReddyGame"
   kind "ConsoleApp"
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
      "include/**.cpp"
   }

   includedirs
	{
		"src",
		"include",
      "%{wks.location}/ReddyEngine/src",
      "%{wks.location}/ReddyEngine/include",
      "%{IncludeDir.stb}",
      "%{IncludeDir.SDL2}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.jsoncpp}",
      "%{IncludeDir.lib_json}",
      "%{IncludeDir.tinyxml2}",
      "%{IncludeDir.lua}"
	}

   links
	{
		"ReddyEngine"
	}

   debugdir "%{wks.location}/"

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