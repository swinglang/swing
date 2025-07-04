#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <array>

namespace fs = std::filesystem;

const std::string TEMP_CPP_FILE = "/tmp/whee_temp.cpp";
const std::string TEMP_EXEC_FILE = "/tmp/whee_exec";

int run_command(const std::string& cmd) {
    int ret = std::system(cmd.c_str());
    if (ret == -1) {
        std::cerr << "Failed to run command: " << cmd << "\n";
        return -1;
    }
    return WEXITSTATUS(ret);
}

std::string run_wheec_and_capture(const std::string& whee_file) {
    std::string cmd = "wheec " + whee_file;
    std::array<char, 128> buffer;
    std::string result;

    // Open pipe to read output
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run wheec command.\n";
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <whee_script>\n";
        return 1;
    }

    std::string whee_file = argv[1];
    std::string wheec_output = run_wheec_and_capture(whee_file);

    if (wheec_output.empty()) {
        std::cerr << "Error: wheec produced no output or failed.\n";
        return 1;
    }

    // Write wheec output to temp cpp file
    // No header line to skip now, just write all lines
    std::ofstream cpp_file(TEMP_CPP_FILE);
    if (!cpp_file.is_open()) {
        std::cerr << "Failed to create temp C++ file.\n";
        return 1;
    }
    cpp_file << wheec_output;
    cpp_file.close();

    // Compile with g++
    std::string compile_cmd = "g++ " + TEMP_CPP_FILE + " -o " + TEMP_EXEC_FILE + " -pthread -std=c++17";
    if (run_command(compile_cmd) != 0) {
        std::cerr << "Error: g++ compilation failed.\n";
        fs::remove(TEMP_CPP_FILE);
        return 1;
    }

    // Run the executable
    int exec_ret = run_command(TEMP_EXEC_FILE);

    // Cleanup
    fs::remove(TEMP_CPP_FILE);
    fs::remove(TEMP_EXEC_FILE);

    return exec_ret;
}
