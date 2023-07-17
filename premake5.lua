include("C:/Repos/Libraries/LibUtil.lua")

workspace "BBSLayout"
    configurations { "Debug", "Release" }
    platforms { "Win32" }
    filter "configurations:Debug"
        defines {"DEBUG"} 
        symbols "on" 
        optimize "off"
        runtime "debug"
    filter "configurations:Release"
        defines {"NDEBUG"} 
        optimize "on"
        runtime "release"
        flags {"NoRuntimeChecks", "LinkTimeOptimization"}
    filter({})
    staticruntime "off"
    targetdir "%{prj.location}/bin/%{cfg.buildcfg}/%{cfg.platform}"
    objdir "%{prj.location}/obj/%{cfg.buildcfg}/%{cfg.platform}"

    project "BBSLayout"
        location "src"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        system "Windows"
        architecture "x86"
        files { "src/**.h", "src/**.c", "src/**.cpp", "src/**.hpp", "ext/**.h", "ext/**.c", "ext/**.cpp", "ext/**.hpp", "imgui/**.h", "imgui/**.cpp" }
        files { "imgui/imgui.natvis"} -- todo: toolchain filter.
        includedirs {"src/", "ext/", "imgui/"}
        vpaths {
            ["Source/*"] = { "src/**.h", "src/**.c", "src/**.hpp", "src/**.cpp" },
            ["Libs/*"] = {  "ext/**.h", "ext/**.c", "ext/**.hpp", "ext/**.cpp" },
            ["imgui/*"] = { "imgui/**" },
            --["Shaders"] = { "**.glsl", "**.vert", "**.frag" },
            ["glad/*"] = { "glad"},
        }
        flags {"NoPCH"}
        
        AddLibrary("glm", "0.9.9.6")
        AddLibrary("glad", "0.1.33-gl3.3-compat-anisotropic")
        AddLibrary("glfw", "3.3-dll")
        AddLibrary("assimp", "5.0.1")
        