CC = gcc
CFLAGS = -std=gnu99 -Wall

OBJS_PLAYER = player.o utils.o
OBJS_VIEW = view.o utils.o
OBJS_MASTER = master.o utils.o

#all: player master view 

all: view player

player: $(OBJS_PLAYER)
	$(CC) $(CFLAGS) -o $@ $^

view: $(OBJS_VIEW)
	$(CC) $(CFLAGS) -o $@ $^

#master: $(OBJS_MASTER)
#	$(CC) $(CFLAGS) -o $@ $^

format:
	clang-format -style=file --sort-includes --Werror -i *.c *.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#clean:
#	rm -f *.o player master view

clean:
	rm -f *.o player view