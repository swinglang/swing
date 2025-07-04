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

const std::string TEMP_CPP_FILE = "/tmp/wcc_temp.cpp";

int run_command(const std::string& cmd) {
    int ret = std::system(cmd.c_str());
    if (ret == -1) {
        std::cerr << "Failed to run command: " << cmd << "\n";
        return -1;
    }
    return WEXITSTATUS(ret);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source.wh>\n";
        return 1;
    }

    std::string source_file = argv[1];

    // Derive output executable name from source filename stem
    std::string output_path = fs::path(source_file).stem().string();
    if (output_path.empty()) output_path = "a.out";

    // Run wheec to generate C++ code
    std::string cmd = "wheec " + source_file;
    std::array<char, 256> buffer;
    std::string wheec_output;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to execute wheec\n";
        return 1;
    }
    while (fgets(buffer.data(), buffer.size(), pipe)) {
        wheec_output += buffer.data();
    }
    int status = pclose(pipe);
    if (status != 0) {
        std::cerr << "wheec failed with exit code " << status << "\n";
        return 1;
    }

    // Write output to temp cpp file
    std::ofstream ofs(TEMP_CPP_FILE);
    if (!ofs) {
        std::cerr << "Failed to create temporary file\n";
        return 1;
    }
    ofs << wheec_output;
    ofs.close();

    // Compile with g++
    std::string compile_cmd = "g++ " + TEMP_CPP_FILE + " -o " + output_path + " -pthread -std=c++17";
    if (run_command(compile_cmd) != 0) {
        std::cerr << "g++ compilation failed\n";
        fs::remove(TEMP_CPP_FILE);
        return 1;
    }

    // Cleanup temp file
    fs::remove(TEMP_CPP_FILE);

    std::cout << "Compiled to: " << output_path << "\n";
    return 0;
}
