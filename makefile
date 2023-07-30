# Compiler settings
CXX = g++
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS =

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES)) \
            $(patsubst $(MODULES_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(MODULES_FILES))

# Executable program
EXEC_NAME = miru
EXEC = $(BIN_DIR)/$(EXEC_NAME)

# Program command line arguments
CLA = 

# OS
OS := LIN
ifeq ($(OS), WIN)
	CXX = x86_64-w64-mingw32-$(CXX)
	EXEC = $(BIN_DIR)/$(EXEC_NAME).exe
endif

# Targets
.PHONY:
	all clear help run final

all: $(EXEC)

# Create the executable program
$(EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ -o $@

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

# Run the program
run:
	./$(EXEC) $(CLA)

# Clear files used by the program
clear:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Use valgrind - Output is saved at bin/valgrind.log
help:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=./bin/valgrind.log ./$(EXEC) $(CLA)

# Final build // use optimization flags such as:
# -O1: Enables level 1 optimization, which includes basic optimizations such as loop optimization, function inlining, and common subexpression elimination.
# -O2: Enables level 2 optimization, which includes more aggressive optimizations such as loop unrolling, function cloning, and instruction scheduling.
# -O3: Enables level 3 optimization, which includes even more aggressive optimizations such as loop vectorization, function inlining across file boundaries, 
#      and profile-guided optimization.
# -Ofast: Enables all the optimizations from -O3 and adds more aggressive optimizations that may violate strict compliance with the C standard.
# -Os: Optimizes for code size by enabling all the optimizations that do not increase code size. This can be useful for embedded systems or programs that need to be small in size.
final: CFLAGS = -Ofast
final: LDFLAGS = 
final: all