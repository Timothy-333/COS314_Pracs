# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -g

# Source files and object files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = myprogram

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

# Clean up object files and the target executable
clean:
	rm -f $(OBJS) $(TARGET)