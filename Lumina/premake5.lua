include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Lumina"
    kind "SharedLib"
    
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{LuminaEngineDirectory}/Binaries/" .. outputdir)
    objdir ("%{LuminaEngineDirectory}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")


    pchheader "pch.h"
    pchsource "Engine/Source/pch.cpp"

    defines
    {
        "LUMINA_ENGINE_DIRECTORY=%{LuminaEngineDirectory}",
        "LUMINA_ENGINE",

        "EASTL_USER_DEFINED_ALLOCATOR=1",

        "GLFW_INCLUDE_NONE",
        "GLFW_STATIC",

        "LUMINA_RENDERER_VULKAN",

        "LUMINA_RPMALLOC",

        "JPH_DEBUG_RENDERER",
        "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
        "JPH_EXTERNAL_PROFILE",
        "JPH_ENABLE_ASSERTS",
    }
    
    prebuildcommands
    {
        --"\"%{LuminaEngineDirectory}/Binaries/Release-windows-x86_64/Reflector.exe\" \"%{wks.location}%{wks.name}.sln\" && echo Reflection completed."
    }

    postbuildcommands
    {
        '{COPYFILE} "%{LuminaEngineDirectory}/External/RenderDoc/renderdoc.dll" "%{cfg.targetdir}"',
    }
    


    files
    {
        "Engine/Source/**.h",
        "Engine/Source/**.cpp",
        
        "%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/**.h",
        "%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/**.cpp",
        
        "Engine/ThirdParty/EnkiTS/src/**.h",
        "Engine/ThirdParty/EnkiTS/src/**.cpp",

        "Engine/ThirdParty/JoltPhysics/Jolt/**.cpp",
        "Engine/ThirdParty/JoltPhysics/Jolt/**.h",
        "Engine/ThirdParty/JoltPhysics/Jolt/**.inl",
        "Engine/ThirdParty/JoltPhysics/Jolt/**.gliffy",
        "Engine/ThirdParty/xxhash/xxhash.c",

        "Engine/ThirdParty/rpmalloc/**.h",
        "Engine/ThirdParty/rpmalloc/**.c",
        "Engine/ThirdParty/RenderDoc/renderdoc_app.h",
        "Engine/ThirdParty/imgui/imgui_demo.cpp",
        "Engine/ThirdParty/imgui/implot_demo.cpp",
        "Engine/ThirdParty/meshoptimizer/src/**.cpp",
        "Engine/ThirdParty/meshoptimizer/src/**.h",
        "Engine/ThirdParty/vk-bootstrap/src/**.h",
        "Engine/ThirdParty/vk-bootstrap/src/**.cpp",
        "Engine/ThirdParty/VulkanMemoryAllocator/include/**.h",
        "Engine/ThirdParty/json/include/**.h",
        "Engine/ThirdParty/json/src/**.cpp",
        "Engine/ThirdParty/ImGuizmo/**.h",
        "Engine/ThirdParty/ImGuizmo/**.cpp",
        "Engine/ThirdParty/SPIRV-Reflect/**.h",
        "Engine/ThirdParty/SPIRV-Reflect/**.c",
        "Engine/ThirdParty/SPIRV-Reflect/**.cpp",
        "Engine/ThirdParty/fastgltf/src/**.cpp",
        "Engine/ThirdParty/fastgltf/deps/simdjson/**.h",
        "Engine/ThirdParty/fastgltf/deps/simdjson/**.cpp",
    }

    includedirs
    { 
        "Engine/Source",
        "Engine/Source/Runtime",
        "Engine/ThirdParty/",

        reflection_directory(),
        includedependencies(),
    }
    
    libdirs
    {
        "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/lua",
        "%{VULKAN_SDK}/lib",
    }

    links
    {
        "GLFW",
        "ImGui",
        "EA",
        "Tracy",
        "lua54",
        "shaderc_combined",
    }

    filter "configurations:Debug"
        removelinks { "shaderc_combined" }
        links { "shaderc_combinedd" }

    filter "files:Engine/ThirdParty/**.cpp"
        flags { "NoPCH" }