project "DetectorDataViewer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files	
	{
		"src/DataHandler.h",
		"src/DataHandler.cpp"
	}

	includedirs
	{
		"%{wks.location}/DetectorDataViewer/src",
		"%{wks.location}/DetectorDataViewer/vendor/imgui",
		"%{wks.location}/DetectorDataViewer/vendor/imgui/backends",
		"%{wks.location}/DetectorDataViewer/vendor/implot",
		"%{wks.location}/DetectorDataViewer/vendor/tinyfiledialogs"
	}

	links
	{
		"imgui",
		"implot",
		"tinyfiledialogs"
	}

	filter "system:windows"
		systemversion "latest"

		links
		{
			"d3d12",
			"d3dcompiler",
			"dxgi"
		}

		files
		{
			"src/mainWindows.cpp"
		}
		
    
	filter "system:linux"
		systemversion "latest"
		pic "On"

		includedirs
		{
			"/usr/include/SDL2"
		}

		links 
		{
			"SDL2",
			"GL"
    	}

		files
		{
			"src/mainLinux.cpp"
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
