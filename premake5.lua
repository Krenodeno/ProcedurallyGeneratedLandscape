--premake5.lua

workspace "ProceduralGeneration"
	configurations { "Debug", "Release" }

	location "build/"
	targetdir "build/bin/"
	objdir "build/obj/"
	debugdir "./"

	language "C++"
	cppdialect "C++11"
	architecture "x86_64"

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		targetsuffix "-d"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter {}

	vpaths {
		["Headers"] = { "**.h", "**.hpp", "**.inl" },
		["Sources"] = { "**.cpp" },
		["Shaders"] = { "**.vert", "**.frag" }
	}

	includedirs { "extern/include/" }
	libdirs { "extern/lib/" }

	links "FastNoiseSIMD"
	links { "SDL2", "SDL2_image" }
	links "freeimageplus"

-- Definition du projet

project "ANewWorld"
	kind "ConsoleApp"

	includedirs { "include/" }
	files { "src/**.cpp" }

project "test"
	kind "ConsoleApp"

	includedirs { "include/" }
	files { "src/**.cpp", "tests/**.cpp" }
	removefiles { "src/main.cpp" }

	links "ANewWorld"
