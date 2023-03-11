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

# executable program name
EXEC = miru1

# compiler
CC = g++

ifeq ($(OS), win)
	CC = x84_64-w64-mingw32-g++
	EXEC = miru1.exe
endif

# flags
flags = -Ofast -fomit-frame-pointer

# create the executable program
engine: $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(flags)

# object files
%.o: %.c
	@$(CC) $(flags) -c -o $@ $<

# execute the engine
run: engine
	./$(EXEC)

# delete excess object files
clear:
	rm -f $(OBJ)
	clear
