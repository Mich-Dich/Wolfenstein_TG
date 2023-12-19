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
    targetdir ("build/bin_" .. os.get("cfg") .. "/") -- output directory for the binaries
    objdir ("build/bin-int_" .. os.get("cfg") .. "/")  -- output directory for intermediate files
    
    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        
    files { "src/*.c" } -- Specify the correct path to the source files
    
