# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra
LDFLAGS = -lcurl

# Target executable name
TARGET = caj

# Source files
SRCS = main.cpp

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)