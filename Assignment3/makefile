# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -g

# Source files and object files
SRCS1 = NN.cpp
SRCS2 = GP.cpp
OBJS1 = $(SRCS1:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)

# Target executables
TARGET1 = program1
TARGET2 = program2

# Default target
all: $(TARGET1) $(TARGET2)

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executables
$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) $^ -o $@

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) $^ -o $@

run1: $(TARGET1)
	./$(TARGET1)

run2: $(TARGET2)
	./$(TARGET2)

# Clean up object files and the target executables
clean:
	rm -f $(OBJS1) $(OBJS2) $(TARGET1) $(TARGET2)