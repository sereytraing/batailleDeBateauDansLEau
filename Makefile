CC=gcc
SRC=src/multi_server.c
CFLAGS=-Wall -Wextra -isystem -Werror -pedantic -std=c99
OBJS=$(SRC:.c=.o)
DEBUG=-Wall -Wextra -Werror -pedantic -std=c99 -g
EXE=MServer

all:$(EXE)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

debug: $(OBJS)
	$(CC) $(DEBUG) -o $(EXE) $(OBJS)

clean:
	rm -f $(OBJS) $(EXE)
