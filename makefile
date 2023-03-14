# source directory
SRC = ./src

include = ./include

# object files
OBJ = $(SRC)/main.o \
	  $(SRC)/utilities.o \
	  $(SRC)/board.o \
	  $(SRC)/tests.o \
	  $(SRC)/uci_commands.o \
	  $(SRC)/search.o \
	  $(SRC)/evaluation.o \
	  $(SRC)/attacks.o \
	  $(SRC)/transposition_table.o \
	  $(SRC)/zobrist_hashing.o

# the OS the engine is running at
OS := LINUX

# executable program name
EXEC := miru1

# compiler
CXX := g++

ifeq ($(OS), WIN)
	CXX = x86_64-w64-mingw32-g++
	EXEC = miru1.exe
endif

# flags
flags = -Ofast -g

# create the executable program
engine: $(OBJ)
	$(CXX) -o $(EXEC) $(OBJ) $(flags)

# object files
%.o: %.c
	@$(CXX) $(flags) -c $@ $<

# execute the engine
run: engine
	./$(EXEC)

# delete excess object files
clear:
	rm -f $(OBJ)
	clear
