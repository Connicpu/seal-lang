workspace "seal-lang"
    objdir "obj/%{cfg.system}/%{prj.name}/%{cfg.platform}/%{cfg.buildcfg}"
    targetdir "bin/%{cfg.system}/%{cfg.platform}/%{cfg.buildcfg}"
    pic "On"
    warnings "Extra"

    configurations { "Debug", "Release" }
    platforms { "x64" }

    vpaths {
        Headers = { "**.h" },
        Inlines = { "**.inl" },
        Source = { "**.cpp" },
    }

    defines { "_USE_MATH_DEFINES" }

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
    symbols "On"

filter "system:windows"
    defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN" }
    characterset "MBCS"

filter "not system:windows"
    toolset "clang"
    buildoptions { "-std=c++1z" }

filter "action:vs*"
    buildoptions { "/std:c++latest" }

project "seal-lang"
    kind "StaticLib"
    language "C++"

    includedirs { "include" }

    files {
        "src/*.cpp",
        "src/*.h",
        "src/*.inl",
        "include/*.h",
        "include/*.inl",
    }

project "seal-tests"
    kind "ConsoleApp"
    language "C++"

    includedirs { "include" }
    links { "seal-lang" }

    files {
        "tests/*.cpp",
        "tests/*.h",
    }

