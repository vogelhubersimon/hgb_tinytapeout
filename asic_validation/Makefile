CC = gcc
CFLAGS = -Wall -Wextra -std=c99  -g3
INCLUDE = -I./src -lm
SRC = ./src/*.c
PROG_NAME = shader_sim

game: $(SRC)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(SRC) $(INCLUDE)

run: game
	 ./$(PROG_NAME) 

clean:
	rm -f ./$(PROG_NAME)