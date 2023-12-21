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
    targetdir ("build/bin/") -- output directory for the binaries
    objdir ("build/bin-int/")  -- output directory for intermediate files
    
    files
    {
    	"Wolfenstein_TG/src/main.c" 
    }
        
    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }


