solution "Wolfenstein_TG"
    configurations 
    {
        "Debug",
        "Release"
    }

location "build" -- location for the generated project files

project "Wolfenstein_TG"
    kind "ConsoleApp"
    language "C"
    targetdir ("build/bin/" .. os.get("cfg") .. "/") -- output directory for the binaries
    objdir ("build/bin-int/" .. os.get("cfg") .. "/")  -- output directory for intermediate files

    files { "src/*.c" } -- Specify the correct path to the source files
