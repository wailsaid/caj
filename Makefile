# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -I.
LDFLAGS = -lcurl

# Target executable name
TARGET = caj

# Source files
SRCS = main.cpp src/utils.cpp src/downloader.cpp src/builder.cpp
OBJS = $(SRCS:.cpp=.o)

# Build rule
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)