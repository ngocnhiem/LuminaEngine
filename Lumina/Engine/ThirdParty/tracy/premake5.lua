include(os.getenv("LUMINA_DIR") .. "/Dependencies.lua")


project "Tracy"
	kind "SharedLib"
	language "C++"

    targetdir ("%{wks.location}/Binaries/" .. outputdir)
    objdir ("%{wks.location}/Intermediates/Obj/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"TRACY_EXPORTS",
		"TRACY_ALLOW_SHADOW_WARNING",
	}

	files
	{
		"public/TracyClient.cpp",
	}
	
	includedirs
	{
		"public",
	}