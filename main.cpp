#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <filesystem>

// Callback function for writing data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outFile = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;
    outFile->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Function to check if a file exists
bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// Function to download a file from a given URL
bool downloadFile(const std::string& url, const std::string& outputPath) {
    // Check if file already exists
    if (fileExists(outputPath)) {
        std::cout << "File already exists: " << outputPath << std::endl;
        return true;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
        return false;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_cleanup(curl);
    return true;
}

// Function to execute a system command and return its exit code
int executeCommand(const std::string& command) {
    return system(command.c_str());
}

// Function to build the project
bool buildProject() {
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
    manifest << "Main-Class: src.Example\n"
             << "Class-Path:";
    
    // Add all JARs to manifest Class-Path
    for (const auto& entry : std::filesystem::directory_iterator("lib")) {
        if (entry.path().extension() == ".jar") {
            manifest << " lib/" << entry.path().filename().string();
        }
    }
    manifest << "\n";
    manifest.close();

    // Create JAR file
    std::string jarCommand = "jar cvfm build/program.jar build/MANIFEST.MF -C build src";
    std::cout << "\nCreating JAR file..." << std::endl;
    std::cout << "Command: " << jarCommand << std::endl;
    
    int jarResult = executeCommand(jarCommand);
    if (jarResult != 0) {
        std::cerr << "JAR creation failed!" << std::endl;
        return false;
    }

    std::cout << "Build successful!" << std::endl;
    std::cout << "You can run the program with: java -jar build/program.jar" << std::endl;
    return true;
}

void printHelp() {
    std::cout << "Usage: program <command>\n"
              << "Commands:\n"
              << "  install    Download dependencies from Maven repository\n"
              << "  build      Build the project (not implemented)\n"
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
