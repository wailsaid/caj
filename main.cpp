#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

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

int main() {
    const std::string jarListFile = "dep.txt"; // File containing JAR names
    const std::string mavenRepo = "https://repo1.maven.org/maven2/";
    const std::string outputDir = "lib/";

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
}
