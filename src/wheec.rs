#!/usr/bin/env rustc
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::process;
use std::collections::HashMap;
use serde_json::Value;
use regex::Regex;

const SYNTAX_FILE: &str = "/etc/whee/syntax.json";

fn load_syntax_rules() -> HashMap<String, (Regex, String)> {
    let file = File::open(SYNTAX_FILE).expect("Failed to open syntax file");
    let json: Value = serde_json::from_reader(file).expect("Failed to parse JSON");

    let mut rules = HashMap::new();

    for (key, entry) in json.as_object().unwrap() {
        let pattern = entry["pattern"].as_str().unwrap();
        let replacement = entry["replacement"].as_str().unwrap();

        let regex = Regex::new(pattern).unwrap();
        rules.insert(key.clone(), (regex, replacement.to_string()));
    }

    rules
}

fn convert_whee_to_rust(filename: &str, rules: &HashMap<String, (Regex, String)>) {
    let file = File::open(filename).unwrap_or_else(|e| {
        eprintln!("Error opening Whee file: {}", e);
        process::exit(1);
    });

    let reader = BufReader::new(file);

    for line in reader.lines() {
        let line = line.unwrap();
        let mut matched = false;

        for (_name, (pattern, replacement)) in rules {
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
    let rules = load_syntax_rules();
    convert_whee_to_rust(&args[1], &rules);
}
