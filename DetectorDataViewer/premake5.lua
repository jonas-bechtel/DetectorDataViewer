project "DetectorDataViewer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	--pchheader "pch.h"
	--pchsource "src/pch.cpp"

	files	
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/DetectorDataViewer/src",
		"%{wks.location}/DetectorDataViewer/vendor/imgui",
		"%{wks.location}/DetectorDataViewer/vendor/imgui/backends",
		"%{wks.location}/DetectorDataViewer/vendor/implot",
		"%{wks.location}/DetectorDataViewer/vendor/tinyfiledialogs"
	}

	libdirs 
	{
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
		
		postbuildcommands
		{
			--'cmd /c if exist vendor\\JSPEC\\lib\\*.dll xcopy /Q /Y /I vendor\\JSPEC\\lib\\*.dll "%{cfg.targetdir}" > nul',
			--'{COPY} "%{cfg.buildtarget.relpath}" "..\\..\\Energy Distribution Model Release"',
			--'{COPY} "imgui.ini" "..\\..\\Energy Distribution Model Release"'
		}
    

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
