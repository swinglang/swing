#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

const std::string MODULES_DIR = "/opt/bitey/Whee/modules";

bool starts_with(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool is_integer(const std::string& s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return !s.empty();
}

void process_line(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty()) {
        std::cout << "\n";
        return;
    }

    // package main; --> comment
    if (starts_with(trimmed, "package ")) {
        std::cout << "// " << trimmed << "\n";
        return;
    }

    // print(...) --> std::cout <<
    if (starts_with(trimmed, "print(") && trimmed.size() >= 8 && trimmed.back() == ';') {
        size_t start = 6;
        size_t end = trimmed.find(");");
        if (end != std::string::npos) {
            std::string inside = trimmed.substr(start, end - start);
            std::cout << "std::cout << " << inside << " << std::endl;\n";
            return;
        }
    }

    // let x = "text";
    if (starts_with(trimmed, "let ")) {
        size_t eq_pos = trimmed.find('=');
        if (eq_pos != std::string::npos) {
            std::string var = trim(trimmed.substr(4, eq_pos - 4));
            std::string val = trim(trimmed.substr(eq_pos + 1));
            if (!val.empty() && val.back() == ';') val.pop_back();

            // String assignment
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                std::string val_str = val.substr(1, val.size() - 2);
                std::cout << "std::string " << var << " = \"" << val_str << "\";\n";
                return;
            }

            // Integer assignment
            if (is_integer(val)) {
                std::cout << "int " << var << " = " << val << ";\n";
                return;
            }
        }
    }

    // ref x >> y;
    if (starts_with(trimmed, "ref ")) {
        size_t arrow_pos = trimmed.find(">>");
        if (arrow_pos != std::string::npos && trimmed.back() == ';') {
            std::string var1 = trim(trimmed.substr(4, arrow_pos - 4));
            std::string var2 = trim(trimmed.substr(arrow_pos + 2));
            if (!var2.empty() && var2.back() == ';') var2.pop_back();
            std::cout << "std::getline(std::cin, " << var2 << ");\n";
            return;
        }
    }

    // int foo() {
    if (starts_with(trimmed, "int ") && trimmed.size() > 6 && trimmed.find("()") != std::string::npos && trimmed.back() == '{') {
        size_t start = 4;
        size_t paren_pos = trimmed.find("()");
        std::string func_name = trimmed.substr(start, paren_pos - start);
        func_name = trim(func_name);
        std::cout << "int " << func_name << "() {\n";
        return;
    }

    // }
    if (trimmed == "}") {
        std::cout << "}\n";
        return;
    }

    // call foo;
    if (starts_with(trimmed, "call ") && trimmed.back() == ';') {
        std::string func = trim(trimmed.substr(5));
        if (!func.empty() && func.back() == ';') func.pop_back();
        std::cout << func << "();\n";
        return;
    }

    // -some line (just strip leading dash)
    if (!trimmed.empty() && trimmed[0] == '-') {
        std::cout << trimmed.substr(1) << "\n";
        return;
    }

    // if (cond) {
    if (starts_with(trimmed, "if ") && trimmed.find('(') != std::string::npos && trimmed.back() == '{') {
        size_t start = trimmed.find('(');
        size_t end = trimmed.rfind(')');
        if (start != std::string::npos && end != std::string::npos && end > start) {
            std::string cond = trimmed.substr(start + 1, end - start - 1);
            std::cout << "if (" << cond << ") {\n";
            return;
        }
    }

    // else {
    if (trimmed == "else {") {
        std::cout << "else {\n";
        return;
    }

    // for x in y {
    if (starts_with(trimmed, "for ") && trimmed.find(" in ") != std::string::npos && trimmed.back() == '{') {
        size_t for_start = 4;
        size_t in_pos = trimmed.find(" in ");
        size_t brace_pos = trimmed.rfind('{');
        std::string var = trim(trimmed.substr(for_start, in_pos - for_start));
        std::string container = trim(trimmed.substr(in_pos + 4, brace_pos - in_pos - 4));
        std::cout << "for (auto& " << var << " : " << container << ") {\n";
        return;
    }

    // while (cond) {
    if (starts_with(trimmed, "while ") && trimmed.find('(') != std::string::npos && trimmed.back() == '{') {
        size_t start = trimmed.find('(');
        size_t end = trimmed.rfind(')');
        if (start != std::string::npos && end != std::string::npos && end > start) {
            std::string cond = trimmed.substr(start + 1, end - start - 1);
            std::cout << "while (" << cond << ") {\n";
            return;
        }
    }

    // # comment
    if (!trimmed.empty() && trimmed[0] == '#') {
        std::cout << "// " << trimmed.substr(1) << "\n";
        return;
    }

    // importcpp Foo;
    if (starts_with(trimmed, "importcpp ")) {
        std::string rest = trimmed.substr(10);
        size_t as_pos = rest.find(" as ");
        if (as_pos != std::string::npos && rest.back() == ';') {
            // importcpp Foo as Bar;
            std::string lib = trim(rest.substr(0, as_pos));
            std::string alias = trim(rest.substr(as_pos + 4));
            if (!alias.empty() && alias.back() == ';') alias.pop_back();
            std::cout << "namespace " << alias << " = " << lib << ";\n";
            return;
        }
        else if (!rest.empty() && rest.back() == ';') {
            // importcpp Foo;
            std::string lib = trim(rest);
            if (!lib.empty() && lib.back() == ';') lib.pop_back();
            std::cout << "#include <" << lib << ">\n";
            return;
        }
    }

    // add 3 and 4 >> x;
    if (starts_with(trimmed, "add ") && trimmed.find(" and ") != std::string::npos && trimmed.find(" >> ") != std::string::npos && trimmed.back() == ';') {
        size_t and_pos = trimmed.find(" and ");
        size_t arrow_pos = trimmed.find(" >> ");
        std::string a_str = trim(trimmed.substr(4, and_pos - 4));
        std::string b_str = trim(trimmed.substr(and_pos + 5, arrow_pos - (and_pos + 5)));
        std::string var = trim(trimmed.substr(arrow_pos + 4));
        if (!var.empty() && var.back() == ';') var.pop_back();
        std::cout << "int " << var << " = " << a_str << " + " << b_str << ";\n";
        return;
    }

    // return x;
    if (starts_with(trimmed, "return ") && trimmed.back() == ';') {
        std::string ret_val = trim(trimmed.substr(7));
        if (!ret_val.empty() && ret_val.back() == ';') ret_val.pop_back();
        std::cout << "return " << ret_val << ";\n";
        return;
    }

    // thread name {
    if (starts_with(trimmed, "thread ") && trimmed.back() == '{') {
        std::string name = trim(trimmed.substr(7, trimmed.size() - 8));
        std::cout << "std::thread " << name << "([&]() {\n";
        return;
    }

    // endthread
    if (trimmed == "endthread") {
        std::cout << "});\n";
        return;
    }

    // If line is just a single word + semicolon, treat as function call:
    if (!trimmed.empty() && trimmed.find(' ') == std::string::npos && trimmed.back() == ';') {
        std::string func = trimmed.substr(0, trimmed.size() - 1);
        std::cout << func << "();\n";
        return;
    }

    std::cout << "// Unrecognized line: " << line << "\n";
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

void convert_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "✗ Error opening file: " << filename << "\n";
        return;
    }

    std::vector<std::string> main_lines;
    std::vector<std::pair<std::string, std::vector<std::string>>> modules;

    std::string line;
    while (std::getline(file, line)) {
        if (starts_with(line, "!import ")) {
            std::string module = trim(line.substr(8));
            modules.emplace_back(module, load_module(module));
        } else {
            main_lines.push_back(line);
        }
    }

    for (const auto& [name, lines] : modules) {
        std::cout << "// Module: " << name << "\n";
        for (const auto& l : lines) process_line(l);
        std::cout << "\n";
    }

    for (const auto& l : main_lines) {
        if (starts_with(l, "!insert ")) {
            std::string path = trim(l.substr(8));
            std::cout << run_insert(path);
            continue;
        }
        process_line(l);
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
