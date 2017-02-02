workspace "seal-lang"
    objdir "obj/%{cfg.system}/%{prj.name}/%{cfg.platform}/%{cfg.buildcfg}"
    targetdir "bin/%{cfg.system}/%{cfg.platform}/%{cfg.buildcfg}"
    pic "On"
    warnings "Extra"

    configurations { "Debug", "Release" }
    platforms { "x64" }

    vpaths {
        Headers = "**.h",
        Source = "**.cpp",
    }

    defines { "_USE_MATH_DEFINES" }

filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }

filter "configurations:Release"
    defines { "NDEBUG" }
    flags { "Symbols" }
    optimize "On"

filter "system:windows"
    defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN" }
    characterset "MBCS"

filter "not system:windows"
    toolset "clang"
    buildoptions { "-std=c++14" }

filter "action:vs*"
    buildoptions { "/std:c++latest" }

project "seal-lang"
    kind "SharedLib"
    language "C++"

    files {
        "src/*.cpp",
        "src/*.h",
        "include/*.h",
    }

project "seal-tests"
    kind "ConsoleApp"
    language "C++"

    files {
        "tests/*.cpp",
        "tests/*.h",
    }

