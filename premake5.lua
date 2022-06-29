workspace "uhero"
	configurations { "debug", "release" }

	project "pong"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"

		targetdir "bin/%{cfg.buildcfg}"

		files { "src/**.hpp", "src/**.cpp" }
		
		includedirs { "ext/" }
		files { "ext/**.cpp" }
		files { "ext/**.c" }

		libdirs { "libs/" }
		links { "Box2D" }

		-- glm defines
		defines { "GLM_FORCE_RADIANS" }

		exceptionhandling "Off"
		warnings "Extra"
		vectorextensions "sse3"

		filter { "system:linux" }
			defines { "OS_LINUX=1" }
			links { "m", "SDL2", "dl", "GL" }
		filter { "system:Windows" }
			defines { "OS_WINDOWS=2" }
			links { "SDL2main", "SDL2", "opengl32" }
		
		filter { "configurations:debug" }
			defines { "DEBUG", "PROFILE" }
			symbols "On"
		filter { "configurations:release" }
			defines { "NDEBUG" }
			optimize "Speed"
