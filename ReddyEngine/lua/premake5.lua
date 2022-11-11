project "lua"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files
   	{
		"**.h",
        "**.c",
   	}

    removefiles
    {
        "luac.c"
    }

   includedirs
	{

	}

   links
	{

	}

	filter "system:Windows"
		characterset ("MBCS")
    
    filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"
  
	filter "configurations:Release"
		defines { "RELEASE" }
		runtime "Release"
		symbols "Off"
		optimize "On"
	 
	filter "configurations:Final"
		defines { "FINAL" }
		runtime "Release"
		symbols "Off"
		optimize "On"
