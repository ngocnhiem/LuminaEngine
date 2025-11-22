include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Editor"
	kind "ConsoleApp"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")   
	
    libdirs
    {
        "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/lua/"
    }

	links
	 {
		"Lumina",
		"ImGui",
    	"EA",
		"Tracy",
		"lua54",
	 }
	 
	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"%{wks.location}/Intermediates/Reflection/Editor/**.h",
        "%{wks.location}/Intermediates/Reflection/Editor/**.cpp",
	}

	includedirs
	{ 
	    "Source",
	    
	    "%{LuminaEngineDirectory}/Lumina/",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",
	    "%{LuminaEngineDirectory}/Intermediates/Reflection/Lumina/",

	    reflection_directory();
		includedependencies();
	}