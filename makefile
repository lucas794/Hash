EXEC=hash
CC=gcc
FLAGS= -Wall -Werror -pedantic -std=c99 -g -Wconversion -Wtype-limits
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
