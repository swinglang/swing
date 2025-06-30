#!/usr/bin/env rustc
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::path::Path;
use std::process::{self, Command};
use regex::Regex;

const MODULES_DIR: &str = "/opt/bitey/Whee/modules";

struct Rule {
    pattern: Regex,
    replacement: &'static str,
}

fn get_hardcoded_rules() -> Vec<Rule> {
    vec![
        Rule { pattern: Regex::new(r#"let (\w+) = "(.*?)";"#).unwrap(), replacement: r#"let \1 = String::from("\2");"# },
        Rule { pattern: Regex::new(r#"let (\w+) = (\d+);"#).unwrap(), replacement: r#"let \1 = \2;"# },
        Rule { pattern: Regex::new(r#"ref (\w+) >> (\w+);"#).unwrap(), replacement: r#"std::io::stdin().read_line(&mut \2).unwrap();"# },
        Rule { pattern: Regex::new(r#"^\s*int\s+(\w+)\s*\(\)\s*\{\s*$"#).unwrap(), replacement: r#"fn \1() {"# },
        Rule { pattern: Regex::new(r#"^=== WHEE ===$"#).unwrap(), replacement: r#"// Whee"# },
        Rule { pattern: Regex::new(r#"^\s*$"#).unwrap(), replacement: r#""# },
        Rule { pattern: Regex::new(r#"^\s*}\s*$"#).unwrap(), replacement: r#"}"# },
        Rule { pattern: Regex::new(r#"^\s*call\s+(\w+)\s*;\s*$"#).unwrap(), replacement: r#"\1();"# },
        Rule { pattern: Regex::new(r#"^\s*-(.*)$"#).unwrap(), replacement: r#"\1"# },
        Rule { pattern: Regex::new(r#"^\s*if\s+\((.*?)\)\s*\{\s*$"#).unwrap(), replacement: r#"if \1 {"# },
        Rule { pattern: Regex::new(r#"^\s*else\s*\{\s*$"#).unwrap(), replacement: r#"else {"# },
        Rule { pattern: Regex::new(r#"^\s*for\s+(\w+)\s+in\s+(\w+)\s*\{\s*$"#).unwrap(), replacement: r#"for \1 in \2 {"# },
        Rule { pattern: Regex::new(r#"^\s*while\s+\((.*?)\)\s*\{\s*$"#).unwrap(), replacement: r#"while \1 {"# },
        Rule { pattern: Regex::new(r#"^\s*#(.*)$"#).unwrap(), replacement: r#"// \1"# },
        Rule { pattern: Regex::new(r#"^\s*importrs\s+(std::[\w:]+);\s*$"#).unwrap(), replacement: r#"use \1;"# },
        Rule { pattern: Regex::new(r#"^\s*importrs\s+(std::[\w:]+)\s+as\s+(\w+);\s*$"#).unwrap(), replacement: r#"use \1 as \2;"# },
        Rule { pattern: Regex::new(r#"^\s*add\s+(\d+)\s+and\s+(\d+)\s+>>\s+(\w+);$"#).unwrap(), replacement: r#"let \3 = \1 + \2;"# },
        Rule { pattern: Regex::new(r#"^\s*return\s+(.*);$"#).unwrap(), replacement: r#"return \1;"# },
        Rule { pattern: Regex::new(r#"^\s*thread\s+(\w+)\s*\{\s*$"#).unwrap(), replacement: r#"let \1 = std::thread::spawn(|| {"# },
        Rule { pattern: Regex::new(r#"^\s*endthread\s*$"#).unwrap(), replacement: r#"});"# },
    ]
}

fn load_module_main_body(module: &str) -> Option<Vec<String>> {
    let path = format!("{}/{}/main.wh", MODULES_DIR, module);
    if !Path::new(&path).exists() {
        return None;
    }

    let file = File::open(&path).ok()?;
    let reader = BufReader::new(file);
    Some(reader.lines().flatten().filter(|l| l.trim() != "package main;").collect())
}

fn convert_whee_to_rust(filename: &str, rules: &[Rule]) {
    let file = File::open(filename).unwrap_or_else(|e| {
        eprintln!("Error opening Whee file: {}", e);
        process::exit(1);
    });

    let reader = BufReader::new(file);
    let mut module_code_blocks = vec![];
    let mut script_lines = vec![];

    for line in reader.lines().flatten() {
        if line.starts_with("!import ") {
            let module_name = line.trim_start_matches("!import ").trim();
            if let Some(code) = load_module_main_body(module_name) {
                module_code_blocks.push((module_name.to_string(), code));
            } else {
                println!("// Module '{}' not found", module_name);
            }
            continue;
        }
        script_lines.push(line);
    }

    for (module_name, lines) in module_code_blocks {
        println!("// Module: {}", module_name);
        for line in lines {
            process_line(&line, rules);
        }
        println!();
    }

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

        process_line(&line, rules);
    }
}

fn process_line(line: &str, rules: &[Rule]) {
    let trimmed = line.trim();

    if trimmed.is_empty() {
        println!();
        return;
    }

    if trimmed.starts_with("package ") {
        println!("// {}", trimmed);
        return;
    }

    // Custom: print(anything);
    if let Some(caps) = Regex::new(r#"^print\((.+)\);$"#).unwrap().captures(trimmed) {
        let expr = caps.get(1).unwrap().as_str();
        println!("println!(\"{{}}\", {});", expr);
        return;
    }

    for rule in rules {
        if let Some(caps) = rule.pattern.captures(trimmed) {
            let mut output = rule.replacement.to_string();
            for (i, cap) in caps.iter().enumerate().skip(1) {
                if let Some(m) = cap {
                    output = output.replace(&format!("\\{}", i), m.as_str());
                }
            }
            println!("{}", output);
            return;
        }
    }

    // Fallback: implicit function call
    if Regex::new(r"^\w+\s*;$").unwrap().is_match(trimmed) {
        println!("{}();", trimmed.trim_end_matches(';').trim());
        return;
    }

    println!("// Unrecognized line: {}", line);
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: wheec <filename>");
        process::exit(1);
    }

    println!("=== Converted Rust Code ===");
    let rules = get_hardcoded_rules();
    convert_whee_to_rust(&args[1], &rules);
}
