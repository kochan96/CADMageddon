workspace "CADMageddon"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "CADMageddon/vendor/glfw/include"
IncludeDir["Glad"] = "CADMageddon/vendor/glad/include"
IncludeDir["ImGui"] = "CADMageddon/vendor/imgui"
IncludeDir["glm"] = "CADMageddon/vendor/glm"
IncludeDir["entt"] = "CADMageddon/vendor/entt/include"
IncludeDir["tinyxml"] = "CADMageddon/vendor/tinyxml2"

group "Dependencies"
    include "CADMageddon/vendor/glfw"
    include "CADMageddon/vendor/glad"
    include "CADMageddon/vendor/imgui"
    include "CADMageddon/vendor/tinyxml2"
group ""

project "CADMageddon"
    location "CADMageddon"
    kind "ConsoleApp"
    language "C++"
    cppdialect "c++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/assets/**",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

    includedirs
    {
        "%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.tinyxml}",
    }

    links 
	{ 
		"GLFW",
        "Glad",
        "ImGui",
        "tinyxml2",
		"opengl32.lib"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines
        {
        }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"