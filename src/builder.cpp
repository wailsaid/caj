#include "builder.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

bool buildProject() {
    // ... (existing build code) ...
    // Create build directory if it doesn't exist
    if (!std::filesystem::exists("build")) {
        std::filesystem::create_directory("build");
    }

    // Build the javac command with classpath including all JARs in lib/
    #ifdef _WIN32
    std::string classpath = "\"";
    std::string pathSeparator = ";";
    #else
    std::string classpath = "";
    std::string pathSeparator = ":";
    #endif

    // Add all JARs from lib directory to classpath
    bool first = true;
    for (const auto& entry : std::filesystem::directory_iterator("lib")) {
        if (entry.path().extension() == ".jar") {
            if (!first) {
                classpath += pathSeparator;
            }
            classpath += "lib/" + entry.path().filename().string();
            first = false;
        }
    }

    #ifdef _WIN32
    classpath += "\"";
    #endif

    // Compile .java files
    std::string compileCommand = "javac -d build -cp " + classpath + " src/*.java";
    std::cout << "Compiling Java files..." << std::endl;
    std::cout << "Command: " << compileCommand << std::endl;
    
    int compileResult = executeCommand(compileCommand);
    if (compileResult != 0) {
        std::cerr << "Compilation failed!" << std::endl;
        return false;
    }

    // Create manifest file
    std::ofstream manifest("build/MANIFEST.MF");
    manifest << "Manifest-Version: 1.0\n"
             << "Main-Class: src.Example\n\n";
    manifest.close();

    // First, compile the Java files
    // ... existing compilation code ...

    // Create a temporary directory for all contents
    if (std::filesystem::exists("build/temp")) {
        std::filesystem::remove_all("build/temp");
    }
    std::filesystem::create_directory("build/temp");

    // Copy compiled classes
    std::string copyCmd = 
    #ifdef _WIN32
        "xcopy /E /I build\\src build\\temp\\src";
    #else
        "cp -r build/src build/temp/";
    #endif
    executeCommand(copyCmd);

    // Extract all dependency JARs to temp directory
    for (const auto& entry : std::filesystem::directory_iterator("lib")) {
        if (entry.path().extension() == ".jar") {
            std::string extractCmd = "cd build/temp && jar xf ../../" + entry.path().string();
            executeCommand(extractCmd);
        }
    }

    // Remove any existing META-INF
    if (std::filesystem::exists("build/temp/META-INF")) {
        std::filesystem::remove_all("build/temp/META-INF");
    }

    // Create the final JAR with all contents
    std::string jarCommand = "cd build/temp && jar cvfm ../program.jar ../MANIFEST.MF .";
    std::cout << "\nCreating JAR file..." << std::endl;
    std::cout << "Command: " << jarCommand << std::endl;
    
    int jarResult = executeCommand(jarCommand);
    if (jarResult != 0) {
        std::cerr << "JAR creation failed!" << std::endl;
        return false;
    }

    // Clean up temp directory
    std::filesystem::remove_all("build/temp");

    std::cout << "Build successful!" << std::endl;
    std::cout << "\nYou can run the program with:\n\n\t\t java -jar build/program.jar \n\n" << std::endl;
    return true;
} 