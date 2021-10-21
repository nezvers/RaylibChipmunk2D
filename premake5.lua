workspace "GameTemplate"
	configurations { "Debug", "Release", "Debug.DLL", "Release.DLL" }
	platforms { "x64"}
	defaultplatform "x64"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Debug.DLL"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter "configurations:Release.DLL"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	targetdir "bin/%{cfg.buildcfg}/"
		
project "raylib"
		
	filter "configurations:Debug OR Release"
		kind "StaticLib"
    
	filter "configurations:Debug.DLL OR Release.DLL"
		kind "SharedLib"
		defines {"BUILD_LIBTYPE_SHARED"}
		
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		links {"winmm"}
	
	location "build"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "raylib/src", "raylib/src/external/glfw/include"}
	vpaths 
	{
		["Header Files"] = { "raylib/src/**.h"},
		["Source Files/*"] = {"raylib/src/**.c"},
	}
	files {"raylib/src/*.h", "raylib/src/*.c"}
	
	defines{"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
		
project "Chipmunk2D"
		
	filter "configurations:Debug OR Release"
		kind "StaticLib"
    
	filter "configurations:Debug.DLL OR Release.DLL"
		kind "SharedLib"
		defines {"BUILD_LIBTYPE_SHARED"}
		
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		links {"winmm"}
    
	location "build"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "Chipmunk2D/include/"}
	vpaths 
	{
		["Header Files"] = { "Chipmunk2D/include/chipmunk/**.h"},
		["Source Files/*"] = {"Chipmunk2D/src/**.c"},
	}
	files {"Chipmunk2D/include/chipmunk/**.h", "Chipmunk2D/src/**.c"}
	
	

project "TheGame"
	kind "ConsoleApp"
	location "build"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	defines{"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
	
	vpaths 
	{
		["Header Files"] = { "src/headers/*.h"},
		["Source Files"] = {"src/*.c"},
	}
	files {"src/*.c", "src/headers/*.h"}

	links {"raylib", "winmm", "kernel32", "opengl32", "gdi32"}
	
	includedirs { "raylib/src", "Chipmunk2D/include/", "include", "src/headers" }
	
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		dependson {"raylib", "Chipmunk2D"}
		links {"raylib.lib", "Chipmunk2D.lib", "winmm", "kernel32"}
		libdirs {"bin/%{cfg.buildcfg}"}
	
	filter "action:gmake*"
		links {"pthread", "GL", "m", "dl", "rt", "X11"}






