package command

import (
	"fmt"
	"os"
)

func CreateNewProject(name string) error {
	// This function will create a new project with the given name.
	// For now, we will just print the name of the project.

	fmt.Printf("Creating new project: %s\n", name)
	if err := os.MkdirAll(name, os.ModePerm); err != nil { // Create a new directory for the project
		return fmt.Errorf("failed to create project directory: %w", err)
	}
	if err := os.MkdirAll(name+"/src", os.ModePerm); err != nil { // Create a src directory inside the project directory
		return fmt.Errorf("failed to create a src directory inside the project directory: %w", err)

	}
	os.Create(name + "/src/main.java") // Create a main Java file in the src directory
	os.Create(name + "/README.md")     // Create a README file in the project directory
	os.Create(name + "/.gitignore")    // Create a .gitignore file in the project directory
	os.Create(name + "./pom.caj")      // Create a pom.caj file in the project directory

	// Here you would add the logic to create a new project directory,
	// initialize version control, etc.
	return nil
}
