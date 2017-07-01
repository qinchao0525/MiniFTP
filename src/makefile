.PHONY:clean
CC=gcc
CFLAGS=-Wall -g
BIN=main
OBJS=
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
