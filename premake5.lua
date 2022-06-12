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

		-- glm defines
		defines { "GLM_FORCE_RADIANS" }

		warnings "Extra"
		vectorextensions "sse3"

		filter { "system:linux" }
			defines { "PLT_LINUX" }
			links { "m", "SDL2", "dl", "GL" }
		filter { "system:Windows" }
			defines { "PLT_WINDOWS" }
			links { "SDL2main", "SDL", "opengl32" }
		
		filter { "configurations:debug" }
			defines { "DEBUG", "PROFILE" }
			symbols "On"
		filter { "configurations:release" }
			defines { "NDEBUG" }
			optimize "Speed"
