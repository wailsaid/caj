#include "src/utils.hpp"
#include "src/downloader.hpp"
#include "src/builder.hpp"
#include <iostream>
#include <fstream>

void printHelp() {
    std::cout << "Usage: program <command>\n"
              << "Commands:\n"
              << "  install    Download dependencies from Maven repository\n"
              << "  build      Build the project\n"
              << "  help       Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No command specified\n";
        printHelp();
        return 1;
    }

    std::string command = argv[1];

    if (command == "help") {
        printHelp();
        return 0;
    } else if (command == "build") {
        return buildProject() ? 0 : 1;
    } else if (command == "install") {
        const std::string jarListFile = "dep.txt";
        const std::string mavenRepo = "https://repo1.maven.org/maven2/";
        const std::string outputDir = "lib/";
        if (!std::filesystem::exists(outputDir)) {
            std::filesystem::create_directory(outputDir);
        }

        // Read the list of JAR files
        std::ifstream jarFile(jarListFile);
        if (!jarFile) {
            std::cerr << "Failed to open file: " << jarListFile << std::endl;
            return 1;
        }

        std::string jarPath;
        while (std::getline(jarFile, jarPath)) {
            if (jarPath.empty()) continue;

            std::string url = mavenRepo + jarPath;
            std::string outputFileName = outputDir + jarPath.substr(jarPath.find_last_of("/") + 1);

            std::cout << "Processing: " << url << std::endl;

            if (downloadFile(url, outputFileName)) {
                if (!fileExists(outputFileName)) {
                    std::cout << "Downloaded: " << outputFileName << std::endl;
                }
            } else {
                std::cerr << "Failed to download: " << url << std::endl;
            }
        }

        std::cout << "Done!" << std::endl;
        return 0;
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n";
        printHelp();
        return 1;
    }
}

