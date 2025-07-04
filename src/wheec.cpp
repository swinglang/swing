#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <sstream>
#include <cstdlib>

namespace fs = std::filesystem;

const std::string MODULES_DIR = "/opt/bitey/Whee/modules";

struct Rule {
    std::regex pattern;
    std::string replacement;
};

std::vector<Rule> get_rules() {
    return {
        {std::regex(R"(let (\w+) = "(.*?)";)"), "std::string \\1 = \\2;"},
        {std::regex(R"(let (\w+) = (\d+);)"), "int \\1 = \\2;"},
        {std::regex(R"(ref (\w+) >> (\w+);)"), "std::getline(std::cin, \\2);"},
        {std::regex(R"(^\s*int\s+(\w+)\s*\(\)\s*\{\s*$)"), "int \\1() {"},
        {std::regex(R"(^\s*$)"), ""},
        {std::regex(R"(^\s*}\s*$)"), "}"},
        {std::regex(R"(^\s*call\s+(\w+)\s*;\s*$)"), "\\1();"},
        {std::regex(R"(^\s*-(.*)$)"), "\\1"},
        {std::regex(R"(^\s*if\s+\((.*?)\)\s*\{\s*$)"), "if (\\1) {"},
        {std::regex(R"(^\s*else\s*\{\s*$)"), "else {"},
        {std::regex(R"(^\s*for\s+(\w+)\s+in\s+(\w+)\s*\{\s*$)"), "for (auto& \\1 : \\2) {"},
        {std::regex(R"(^\s*while\s+\((.*?)\)\s*\{\s*$)"), "while (\\1) {"},
        {std::regex(R"(^\s*#(.*)$)"), "// \\1"},
        {std::regex(R"(^\s*importcpp\s+([\w:]+);\s*$)"), "#include <\\1>"},
        {std::regex(R"(^\s*importcpp\s+([\w:]+)\s+as\s+(\w+);\s*$)"), "namespace \\2 = \\1;"},
        {std::regex(R"(^\s*add\s+(\d+)\s+and\s+(\d+)\s+>>\s+(\w+);$)"), "int \\3 = \\1 + \\2;"},
        {std::regex(R"(^\s*return\s+(.*);$)"), "return \\1;"},
        {std::regex(R"(^\s*thread\s+(\w+)\s*\{\s*$)"), "std::thread \\1([&]() {"},
        {std::regex(R"(^\s*endthread\s*$)"), "});"}
    };
}

std::vector<std::string> load_module(const std::string& module) {
    std::vector<std::string> lines;
    fs::path path = fs::path(MODULES_DIR) / module / "main.wh";
    if (!fs::exists(path)) return lines;

    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line != "package main;") lines.push_back(line);
    }
    return lines;
}

std::string run_insert(const std::string& filepath) {
    std::ostringstream output;
    std::string command = "./wheec " + filepath;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "// Failed to run wheec\n";

    char buffer[256];
    bool skipHeader = true;
    while (fgets(buffer, sizeof buffer, pipe)) {
        std::string line(buffer);
        // No header to skip now, but keep this if future-proof
        if (skipHeader && line.find("=== Converted Rust Code ===") != std::string::npos) continue;
        skipHeader = false;
        output << line;
    }
    pclose(pipe);
    return output.str();
}

std::string apply_rules(const std::string& line, const std::vector<Rule>& rules) {
    for (const auto& rule : rules) {
        if (std::regex_match(line, rule.pattern)) {
            return std::regex_replace(line, rule.pattern, rule.replacement);
        }
    }
    return "";
}

void process_line(const std::string& line, const std::vector<Rule>& rules) {
    std::string trimmed = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

    if (trimmed.empty()) {
        std::cout << "\n";
        return;
    }

    if (trimmed.rfind("package ", 0) == 0) {
        std::cout << "// " << trimmed << "\n";
        return;
    }

    std::smatch m;
    if (std::regex_match(trimmed, m, std::regex(R"(^print\((.+)\);$)"))) {
        std::cout << "std::cout << " << m[1] << " << std::endl;\n";
        return;
    }

    std::string transformed = apply_rules(trimmed, rules);
    if (!transformed.empty()) {
        std::cout << transformed << "\n";
        return;
    }

    if (std::regex_match(trimmed, std::regex(R"(^\w+\s*;$)"))) {
        std::cout << trimmed.substr(0, trimmed.size() - 1) << "();\n";
        return;
    }

    std::cout << "// Unrecognized line: " << line << "\n";
}

void convert_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "✗ Error opening file: " << filename << "\n";
        return;
    }

    std::vector<Rule> rules = get_rules();
    std::vector<std::string> main_lines;
    std::vector<std::pair<std::string, std::vector<std::string>>> modules;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("!import ", 0) == 0) {
            std::string module = line.substr(8);
            modules.emplace_back(module, load_module(module));
        } else {
            main_lines.push_back(line);
        }
    }

    for (const auto& [name, lines] : modules) {
        std::cout << "// Module: " << name << "\n";
        for (const auto& l : lines) process_line(l, rules);
        std::cout << "\n";
    }

    for (const auto& l : main_lines) {
        if (l.rfind("!insert ", 0) == 0) {
            std::string path = l.substr(8);
            std::cout << run_insert(path);
            continue;
        }
        process_line(l, rules);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: wheec <filename>\n";
        return 1;
    }

    convert_file(argv[1]);
    return 0;
}
