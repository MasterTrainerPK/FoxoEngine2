workspace "FoxoEngine2"
	architecture "x86_64"
	startproject "FoxoEngine2"
	configurations { "Debug", "Release", "Dist" }

	flags
	{
		"MultiProcessorCompile"
	}

outputbindir = "bin/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/%{prj.name}"
outputobjdir = "bin-int/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/%{prj.name}"

project "FoxoEngine2"
	location "FoxoEngine2"
	language "C++"
	cppdialect "C++17"

	targetdir (outputbindir)
	objdir (outputobjdir)

	files
	{
		"%{prj.location}/src/**.cpp",
		"%{prj.location}/src/**.hpp",
		"%{prj.location}/src/**.c",
		"%{prj.location}/src/**.h"
	}

	includedirs
	{
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/glad2/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/spdlog/include"
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
	}

	links
	{
		"glfw",
		"glad2",
		"imgui"
	}

	filter "system:windows"
		defines "FE_PLAT_WINDOWS"
		systemversion "latest"
		
	filter "system:linux"
		defines "FE_PLAT_LINUX"

		links
		{
			"dl",
			"pthread"
		}

	filter "system:macosx"
		defines "FE_PLAT_MAC"

		links
		{
			"CoreFoundation.framework",
			"Cocoa.framework",
			"IOKit.framework",
			"CoreVideo.framework"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines "FE_CONF_DEBUG"
		kind "ConsoleApp"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines "FE_CONF_RELEASE"
		kind "ConsoleApp"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		defines "FE_CONF_DIST"
		kind "WindowedApp"

group "Dependencies"

project "glm"
	location "vendor/glm"
	kind "None"
	language "C++"
	cppdialect "C++17"

	files
	{
		"%{prj.location}/glm/**.hpp"
	}

project "spdlog"
	location "vendor/spdlog"
	kind "None"
	language "C++"
	cppdialect "C++17"

project "glfw"
	location "vendor/glfw"
	kind "StaticLib"
	language "C"

	targetdir (outputbindir)
	objdir (outputobjdir)

	files
	{
		"%{prj.location}/src/context.c",
		"%{prj.location}/src/egl_context.c",
		"%{prj.location}/src/egl_context.h",
		"%{prj.location}/src/init.c",
		"%{prj.location}/src/input.c",
		"%{prj.location}/src/internal.h",
		"%{prj.location}/src/monitor.c",
		"%{prj.location}/src/osmesa_context.c",
		"%{prj.location}/src/osmesa_context.h",
		"%{prj.location}/src/vulkan.c",
		"%{prj.location}/src/window.c"
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

	filter "system:windows"
		systemversion "latest"

		files
		{
			"%{prj.location}/src/wgl_context.c",
			"%{prj.location}/src/wgl_context.h",
			"%{prj.location}/src/win32_init.c",
			"%{prj.location}/src/win32_joystick.c",
			"%{prj.location}/src/win32_joystick.h",
			"%{prj.location}/src/win32_monitor.c",
			"%{prj.location}/src/win32_platform.h",
			"%{prj.location}/src/win32_thread.c",
			"%{prj.location}/src/win32_time.c",
			"%{prj.location}/src/win32_window.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
	
	filter "system:linux"
		files
		{
			"%{prj.location}/src/glx_context.c",
			"%{prj.location}/src/glx_context.h",
			"%{prj.location}/src/linux_joystick.c",
			"%{prj.location}/src/linux_joystick.h",
			"%{prj.location}/src/posix_time.c",
			"%{prj.location}/src/posix_time.h",
			"%{prj.location}/src/posix_thread.c",
			"%{prj.location}/src/posix_thread.h",
			"%{prj.location}/src/x11_init.c",
			"%{prj.location}/src/x11_monitor.c",
			"%{prj.location}/src/x11_platform.h",
			"%{prj.location}/src/x11_window.c",
			"%{prj.location}/src/xkb_unicode.c",
			"%{prj.location}/src/xkb_unicode.h"
		}

		defines 
		{ 
			"_GLFW_X11"
		}
		
	filter "system:macosx"
		files
		{
			"%{prj.location}/src/cocoa_init.m",
			"%{prj.location}/src/cocoa_joystick.m",
			"%{prj.location}/src/cocoa_joystick.h",
			"%{prj.location}/src/cocoa_monitor.m",
			"%{prj.location}/src/cocoa_platform.h",
			"%{prj.location}/src/cocoa_time.c",
			"%{prj.location}/src/cocoa_window.m",
			"%{prj.location}/src/nsgl_context.m",
			"%{prj.location}/src/nsgl_context.h",
			"%{prj.location}/src/posix_thread.c",
			"%{prj.location}/src/posix_thread.h"
		}

		defines
		{ 
			"_GLFW_COCOA"
		}

project "glad2"
	location "vendor/glad2"
	kind "StaticLib"
	language "C"

	targetdir (outputbindir)
	objdir (outputobjdir)

	files
	{
		"%{prj.location}/src/gl.c"
	}

	includedirs
	{
		"%{prj.location}/include"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"