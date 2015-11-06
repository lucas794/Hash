EXEC=hash
CC=gcc
FLAGS= -std=c99 -Wall -Wconversion -Wtype-limits -Wbad-function-cast -Wshadow -Wpointer-arith -Wunreachable-code -pedantic -Werror -MMD
OBJS=$(wildcard *.c) $(wildcard *.h)

all: $(EXEC)

%.o: %.c %.h
	$(CC) $(FLAGS) -c $<

$(EXEC): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(EXEC)


.PHONY:clean, memcheck
clean:
	rm $(wildcard *.o) $(EXEC)
memcheck:
	valgrind --leak-check=full --track-origins=yes ./$(EXEC)
