package main

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
)

// parseWheeToC function parses the Whee code and generates corresponding C code
func parseWheeToC(wheeCode string, outputFile string) error {
	// Open the output file
	f, err := os.Create(outputFile)
	if err != nil {
		return fmt.Errorf("error creating output file: %v", err)
	}
	defer f.Close()

	// Write basic C program structure
	fmt.Fprintln(f, "#include <stdio.h>")
	fmt.Fprintln(f, "int main() {")

	// Process the Whee code line by line
	lines := strings.Split(wheeCode, "\n")
	for _, line := range lines {
		line = strings.TrimSpace(line)

		// Handle variable assignment in Whee
		if strings.HasPrefix(line, "var =") {
			parts := strings.SplitN(line, " = ", 2)
			if len(parts) == 2 {
				varName := strings.Trim(parts[0], "var ")
				varValue := strings.Trim(parts[1], "\"")
				fmt.Fprintf(f, "    char* %s = \"%s\";\n", varName, varValue)
			}
		}

		// Handle print statement in Whee
		if strings.HasPrefix(line, "print(") {
			parts := strings.SplitN(line, "print(", 2)
			if len(parts) > 1 {
				printStatement := strings.Trim(parts[1], "()")
				printStatement = strings.Trim(printStatement, "\"")

				// Print to C code
				fmt.Fprintf(f, "    printf(\"%s\\n\");\n", printStatement)
			}
		}
	}

	// Close the C program structure
	fmt.Fprintln(f, "    return 0;")
	fmt.Fprintln(f, "}")

	return nil
}

// compileAndExecuteCCode function compiles and executes the generated C code
func compileAndExecuteCCode(outputFile string) error {
	// Compile the generated C code using gcc
	cmd := exec.Command("gcc", "-o", "temp_program", outputFile)
	err := cmd.Run()
	if err != nil {
		return fmt.Errorf("error compiling C code: %v", err)
	}

	// Execute the compiled C program
	cmd = exec.Command("./temp_program")
	err = cmd.Run()
	if err != nil {
		return fmt.Errorf("error executing the compiled program: %v", err)
	}

	// Clean up by removing the temporary program
	os.Remove("temp_program")

	return nil
}

func main() {
	// Sample Whee code
	wheeCode := `var = "Leon";
print("Hello, " $var);
`
	// Output file for the generated C code
	outputFile := "generated_code.c"

	// Step 1: Parse Whee code and generate C code
	err := parseWheeToC(wheeCode, outputFile)
	if err != nil {
		fmt.Println("Error generating C code:", err)
		return
	}

	// Step 2: Compile and execute the generated C code
	err = compileAndExecuteCCode(outputFile)
	if err != nil {
		fmt.Println("Error during compilation or execution:", err)
		return
	}
}
