include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")

project "Editor"
	kind "ConsoleApp"
    
    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")   

	prebuildcommands 
    {
        'python "%{LuminaEngineDirectory}/Scripts/RunReflection.py" "%{wks.location}\\Lumina.sln"'
    }
	
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
		"Source/**.cpp",
		"Source/**.h",
		
		reflection_unity_file,
	}

	includedirs
	{ 
	    "Source",
	    
	    "%{LuminaEngineDirectory}/Lumina/",
		"%{LuminaEngineDirectory}/Lumina/Engine/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/",
	    "%{LuminaEngineDirectory}/Lumina/Engine/Source/Runtime/",

	    reflection_dir,

		includedependencies(),
	}