use std::env;
use std::fs::{self, File};
use std::io::{Write, BufWriter};
use std::path::Path;
use std::process::Command;

const TEMP_RUST_FILE: &str = "/tmp/wcc_temp.rs";

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <source.wh>", args[0]);
        std::process::exit(1);
    }

    let source_file = &args[1];
    let output_path = Path::new(source_file)
        .file_stem()
        .map(|s| s.to_string_lossy().to_string())
        .unwrap_or_else(|| "a.out".to_string());

    // Run wheec and capture output
    let output = Command::new("wheec")
        .arg(source_file)
        .output()
        .expect("Failed to execute wheec");

    if !output.status.success() {
        eprintln!("Error: wheec failed:\n{}", String::from_utf8_lossy(&output.stderr));
        std::process::exit(1);
    }

    // Write generated Rust code to temp file
    let stdout = String::from_utf8_lossy(&output.stdout);
    let mut lines = stdout.lines();
    lines.next(); // Skip header

    let mut file = BufWriter::new(File::create(TEMP_RUST_FILE).expect("Failed to create temp file"));
    for line in lines {
        writeln!(file, "{}", line).unwrap();
    }
    file.flush().unwrap();

    // Compile it into the final binary
    let status = Command::new("rustc")
        .arg(TEMP_RUST_FILE)
        .arg("-o")
        .arg(&output_path)
        .status()
        .expect("Failed to run rustc");

    cleanup();

    if !status.success() {
        eprintln!("rustc failed to compile the program");
        std::process::exit(1);
    }

    println!("Compiled to: {}", output_path);
}

fn cleanup() {
    if Path::new(TEMP_RUST_FILE).exists() {
        let _ = fs::remove_file(TEMP_RUST_FILE);
    }
}
