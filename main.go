package main

import (
	"bufio"
	"caj/command"
	"fmt"
	"log"
	"net/http"
	"os"
)

// Version returns the version of the application.
const version = "0.1.0"

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		log.Fatalf("command expected, %d args provided", len(args))
	}
	if args[0] == "help" {
		fmt.Println("Usage: caj <command> [options]")
		fmt.Println("Available commands:")
		fmt.Println("  help     Show this help message")
		fmt.Println("  version  Show the version of the application")
		fmt.Println("  new      create a new project ")
		fmt.Println("  install  Install dependences ")
		fmt.Println("  compile  Compile source code")
		fmt.Println("  package  create packaged jar executable")
		fmt.Println("  run      Run the application with the provided arguments")
		return
	}

	if args[0] == "version" {
		fmt.Printf("Caj version %s\n", version)
		return
	}
	if args[0] == "new" {
		var projectName string
		if len(args) < 2 {
			reader := bufio.NewReader(os.Stdin)
			fmt.Print("Enter project name: ")
			name, err := reader.ReadString('\n')
			if err != nil {
				log.Fatalf("Error reading project name: %v", err)
			}
			if len(name) < 2 || name == "" {
				log.Fatalln("project name cannot be empty")
			}
			projectName = name[:len(name)-1] // Remove newline character

		} else {
			projectName = args[1]
		}

		if err := command.CreateNewProject(projectName); err != nil {
			log.Fatalf("Error creating new project: %v", err)
		}
		return
	}

	if args[0] == "install" {
		fmt.Println("Installing dependencies...")
		content, err := os.ReadFile("pom.caj")
		if err != nil {
			log.Fatalf("Error reading pom.caj file: %v", err)

		}
		fmt.Println("Content of pom.caj:")
		fmt.Println(string(content))
		res, err := http.Get("https://repo1.maven.org/maven2/com/fasterxml/jackson/dataformat/jackson-dataformat-xml")
		if err != nil {
			log.Fatal(err)
		}
		var body []byte = make([]byte, 0)
		res.Body.Read(body)
		fmt.Println(string(body))
		// Placeholder for install logic
		return
	}
	fmt.Println(os.Args[1:])
}
