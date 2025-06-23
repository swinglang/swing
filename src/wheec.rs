#!/usr/bin/env rustc
use std::fs::{File, read_to_string};
use std::io::{BufRead, BufReader};
use std::path::Path;
use std::process::{self, Command};
use std::collections::HashMap;
use serde_json::Value;
use regex::Regex;

const SYNTAX_FILE: &str = "/opt/bitey/Whee/syntax.json";
const MODULES_DIR: &str = "/opt/bitey/Whee/modules";

fn load_syntax_from_file(path: &str) -> HashMap<String, (Regex, String)> {
    let file = File::open(path).unwrap_or_else(|e| {
        eprintln!("Failed to open syntax file '{}': {}", path, e);
        process::exit(1);
    });

    let json: Value = serde_json::from_reader(file).unwrap_or_else(|e| {
        eprintln!("Failed to parse JSON in '{}': {}", path, e);
        process::exit(1);
    });

    let mut rules = HashMap::new();
    for (key, entry) in json.as_object().unwrap() {
        let pattern = entry["pattern"].as_str().unwrap();
        let replacement = entry["replacement"].as_str().unwrap();
        let regex = Regex::new(pattern).unwrap();
        rules.insert(key.clone(), (regex, replacement.to_string()));
    }
    rules
}

fn merge_rules(target: &mut HashMap<String, (Regex, String)>, source: HashMap<String, (Regex, String)>) {
    for (k, v) in source {
        target.insert(k, v);
    }
}

fn load_module_main_body(module: &str) -> Option<Vec<String>> {
    let path = format!("{}/{}/main.wh", MODULES_DIR, module);
    if !Path::new(&path).exists() {
        return None;
    }

    let file = File::open(&path).ok()?;
    let reader = BufReader::new(file);
    let mut lines = vec![];
    let mut in_main = false;

    for line in reader.lines().flatten() {
        if !in_main {
            if line.trim() == "package main;" {
                in_main = true;
            }
            continue;
        }
        lines.push(line);
    }

    Some(lines)
}

fn convert_whee_to_rust(filename: &str, rules: &mut HashMap<String, (Regex, String)>) {
    let file = File::open(filename).unwrap_or_else(|e| {
        eprintln!("Error opening Whee file: {}", e);
        process::exit(1);
    });

    let reader = BufReader::new(file);
    let mut module_main_calls = vec![];
    let mut module_code_blocks = vec![];
    let mut script_lines = vec![];

    // First pass: process !import and skip !insert lines
    for line in reader.lines().flatten() {
        if line.starts_with("!import ") {
            let module_name = line.trim_start_matches("!import ").trim();
            let syntax_path = format!("{}/{}/syntax.json", MODULES_DIR, module_name);
            if Path::new(&syntax_path).exists() {
                let module_rules = load_syntax_from_file(&syntax_path);
                merge_rules(rules, module_rules);
                println!("// Imported module: {}", module_name);
            } else {
                println!("// Module not found: {}", module_name);
            }

            if let Some(code) = load_module_main_body(module_name) {
                module_code_blocks.push((module_name.to_string(), code));
                module_main_calls.push(format!("{}();", module_name.to_string() + "_module"));
            }

            continue;
        }

        if line.trim().starts_with("!insert ") {
            continue; // skip from syntax parsing
        }

        script_lines.push(line);
    }

    // Print module code first
    for (module_name, lines) in module_code_blocks {
        println!("// Module: {}", module_name);
        for line in lines {
            let mut matched = false;
            for (_name, (pattern, replacement)) in rules.iter() {
                if let Some(caps) = pattern.captures(&line) {
                    let mut output = replacement.clone();
                    for (i, cap) in caps.iter().enumerate().skip(1) {
                        if let Some(m) = cap {
                            output = output.replace(&format!("\\{}", i), m.as_str());
                        }
                    }
                    println!("{}", output);
                    matched = true;
                    break;
                }
            }
            if !matched {
                println!("// Unrecognized module line: {}", line);
            }
        }
        println!();
    }

    // Second pass: main user code
    for line in script_lines {
        if line.trim() == "package main;" {
            println!("// User main package start");
            continue;
        }

        if line.trim().starts_with("!insert ") {
            let insert_path = line.trim().trim_start_matches("!insert").trim();

            let output = Command::new("wheec")
                .arg(insert_path)
                .output()
                .expect("Failed to execute wheec for !insert");

            if !output.status.success() {
                eprintln!("✗ wheec failed for insert file: {}", insert_path);
                process::exit(1);
            }

            let stdout = String::from_utf8_lossy(&output.stdout);
            for (i, line) in stdout.lines().enumerate() {
                if i == 0 && line.trim() == "=== Converted Rust Code ===" {
                    continue;
                }
                println!("{}", line);
            }

            continue;
        }

        let mut matched = false;
        for (_name, (pattern, replacement)) in rules.iter() {
            if let Some(caps) = pattern.captures(&line) {
                let mut output = replacement.clone();
                for (i, cap) in caps.iter().enumerate().skip(1) {
                    if let Some(m) = cap {
                        output = output.replace(&format!("\\{}", i), m.as_str());
                    }
                }
                println!("{}", output);
                matched = true;
                break;
            }
        }

        if !matched {
            println!("// Unrecognized line: {}", line);
        }
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: wheec <filename>");
        process::exit(1);
    }

    println!("=== Converted Rust Code ===");
    let mut rules = load_syntax_from_file(SYNTAX_FILE);
    convert_whee_to_rust(&args[1], &mut rules);
}
