workspace "DetectorDataViewer"
	architecture "x86_64"
	startproject "DetectorDataViewer"

	configurations
	{
		"Debug",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "DetectorDataViewer/vendor/tinyfiledialogs"
	include "DetectorDataViewer/vendor/imgui"
	include "DetectorDataViewer/vendor/implot"
group ""

group "Core"
	include "DetectorDataViewer"
group ""
