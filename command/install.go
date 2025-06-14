package command

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

// DownloadMavenJAR downloads a JAR file from Maven Central or other repositories
func DownloadMavenJAR(coords string, repos []string) error {
	// Parse coordinates
	parts := strings.Split(coords, ":")
	if len(parts) != 3 {
		return fmt.Errorf("invalid coordinates format, expected group:artifact:version")
	}
	groupId, artifactId, version := parts[0], parts[1], parts[2]

	// Determine filename
	versionPart := version
	if version == "latest" {
		// In a real implementation, you'd need to fetch metadata to determine latest
		return fmt.Errorf("'latest' version not implemented in this example")
	}
	fileName := fmt.Sprintf("%s-%s.jar", artifactId, versionPart)

	// Convert groupId to path
	groupIdPath := strings.ReplaceAll(groupId, ".", "/")

	// Construct paths
	paths := []string{
		groupIdPath,
		artifactId,
		version,
		fileName,
	}
	relativePath := strings.Join(paths, "/")

	// Try each repository until successful
	var lastError error
	for _, repo := range repos {
		// Ensure repo URL ends with /
		if !strings.HasSuffix(repo, "/") {
			repo += "/"
		}

		url := repo + relativePath
		fmt.Printf("Trying %s...\n", url)

		resp, err := http.Get(url)
		if err != nil {
			lastError = fmt.Errorf("failed to download from %s: %v", repo, err)
			continue
		}
		defer resp.Body.Close()

		if resp.StatusCode != http.StatusOK {
			lastError = fmt.Errorf("failed to download from %s: HTTP %d", repo, resp.StatusCode)
			continue
		}

		// Create directory if needed
		dir := filepath.Dir(fileName)
		if dir != "" {
			if err := os.MkdirAll(dir, 0755); err != nil {
				return fmt.Errorf("failed to create directory: %v", err)
			}
		}

		// Create the file
		out, err := os.Create(fileName)
		if err != nil {
			return fmt.Errorf("failed to create file: %v", err)
		}
		defer out.Close()

		// Write the body to file
		_, err = io.Copy(out, resp.Body)
		if err != nil {
			return fmt.Errorf("failed to write file: %v", err)
		}

		fmt.Printf("Successfully downloaded %s to %s\n", coords, fileName)
		return nil
	}

	return fmt.Errorf("failed to download from all repositories: %v", lastError)
}

func example() {
	// Example usage
	coordinates := []string{
		"org.apache.commons:commons-lang3:3.17.0",
		"com.fasterxml.jackson.dataformat:jackson-dataformat-xml:latest",
	}

	// List of repositories to try (in order)
	repositories := []string{
		"https://repo1.maven.org/maven2/",
		"https://maven.aliyun.com/repository/public/",
	}

	for _, coord := range coordinates {
		fmt.Printf("Downloading %s...\n", coord)
		err := DownloadMavenJAR(coord, repositories)
		if err != nil {
			fmt.Printf("Error downloading %s: %v\n", coord, err)
		}
	}
}
