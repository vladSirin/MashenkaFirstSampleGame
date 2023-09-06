project "Glad"
kind "StaticLib"
language "C"
staticruntime "on"

targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

files
{
    "include/Glad/glad.h",
    "include/KHR/khrplatform.h",
    "src/glad.c"
}

includedirs
{
    "include"
}

filter "system:linux"
pic "On"
systemversion "latest"

filter "system:macosx"
pic "On"

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
optimize "speed"
symbols "off"