#include "utils.hpp"
#include <fstream>
#include <cstdlib>

bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

int executeCommand(const std::string& command) {
    return system(command.c_str());
} 