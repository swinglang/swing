#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <array>

namespace fs = std::filesystem;

const std::string TEMP_WHEE_FILE = "/tmp/whee_repl.wh";
const std::string TEMP_CPP_FILE = "/tmp/whee_repl.cpp";
const std::string TEMP_EXEC_FILE = "/tmp/whee_repl_exec";

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
    if (argc == 2) {
        std::string script_path = argv[1];
        // Check if file exists and is non-empty
        std::ifstream script_file(script_path);
        if (!script_file.is_open()) {
            std::cerr << "Error: cannot open file '" << script_path << "'\n";
            return 1;
        }

        script_file.seekg(0, std::ios::end);
        if (script_file.tellg() == 0) {
            // Empty file, fallback to shell
            std::cout << "Script is empty. Starting interactive shell...\n";
        } else {
            // File is non-empty: run the script directly
            script_file.close();

            // Run wheec to get C++ code
            std::string cpp_code = run_wheec_and_capture(script_path);
            if (cpp_code.empty()) {
                std::cerr << "Compilation failed or no output.\n";
                return 1;
            }

            // Write C++ code to file
            std::ofstream cpp_file(TEMP_CPP_FILE);
            if (!cpp_file.is_open()) {
                std::cerr << "Failed to write temp C++ file.\n";
                return 1;
            }
            cpp_file << cpp_code;
            cpp_file.close();

            // Compile
            std::string compile_cmd = "g++ " + TEMP_CPP_FILE + " -o " + TEMP_EXEC_FILE + " -pthread -std=c++17";
            if (run_command(compile_cmd) != 0) {
                std::cerr << "g++ compilation failed.\n";
                return 1;
            }

            // Run executable
            int ret = run_command(TEMP_EXEC_FILE);

            // Cleanup temp files
            fs::remove(TEMP_CPP_FILE);
            fs::remove(TEMP_EXEC_FILE);

            return ret;
        }
    } else if (argc > 2) {
        std::cerr << "Usage: whee [script.wh]\n";
        return 1;
    }

    // If no arguments or empty script, start interactive shell
    std::cout << "🐶 Whee 2.1.1\nType 'help' for help.\n";

    std::string input_line;
    while (true) {
        std::cout << "wh> ";
        if (!std::getline(std::cin, input_line)) break;

        if (input_line == "exit" || input_line == "quit") {
            break;
        }

        if (input_line == "help") {
            std::cout << "Commands:\n"
                      << "  help  - Show this help message\n"
                      << "  exit  - Exit the shell\n"
                      << "You can also enter Whee code lines to execute.\n";
            continue;
        }

        // Write input_line to .wh file
        std::ofstream whee_file(TEMP_WHEE_FILE);
        if (!whee_file.is_open()) {
            std::cerr << "Failed to write temp Whee file.\n";
            continue;
        }
        whee_file << input_line << "\n";
        whee_file.close();

        // Run wheec to get C++ code
        std::string cpp_code = run_wheec_and_capture(TEMP_WHEE_FILE);
        if (cpp_code.empty()) {
            std::cerr << "Compilation failed or no output.\n";
            continue;
        }

        // Write C++ code to file
        std::ofstream cpp_file(TEMP_CPP_FILE);
        if (!cpp_file.is_open()) {
            std::cerr << "Failed to write temp C++ file.\n";
            continue;
        }
        cpp_file << cpp_code;
        cpp_file.close();

        // Compile
        std::string compile_cmd = "g++ " + TEMP_CPP_FILE + " -o " + TEMP_EXEC_FILE + " -pthread -std=c++17";
        if (run_command(compile_cmd) != 0) {
            std::cerr << "g++ compilation failed.\n";
            continue;
        }

        // Run and display output
        std::string run_cmd = TEMP_EXEC_FILE;
        int ret = run_command(run_cmd);
        if (ret != 0) {
            std::cerr << "Execution failed with exit code " << ret << "\n";
        }
    }

    // Cleanup temp files on exit
    fs::remove(TEMP_WHEE_FILE);
    fs::remove(TEMP_CPP_FILE);
    fs::remove(TEMP_EXEC_FILE);

    return 0;
}
