workspace "pong"
	configurations { "debug", "release" }

	project "pong"
		kind "ConsoleApp"
		language "C"
		cdialect "C11"
		architecture "x86_64"

		targetdir "bin/%{cfg.buildcfg}"

		files { "src/**.h", "src/**.c" }
		
		includedirs { "ext/" }
		files { "ext/**.c" }

		libdirs { "ext/winlibs" }
		links {  }

		warnings "Extra"
		vectorextensions "sse3"

		filter { "system:linux" }
			defines { "PLT_LINUX" }
			links { "m", "dl", "SDL2", "GL" }
		filter { "system:Windows" }
			defines { "PLT_WINDOWS" }
			links { "SDL2main", "SDL2", "opengl32" }
		
		filter { "configurations:debug" }
			defines { "DEBUG", "PROFILE" }
			symbols "On"
		filter { "configurations:release" }
			defines { "NDEBUG" }
			optimize "Speed"