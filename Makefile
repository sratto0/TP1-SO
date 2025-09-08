# This is a personal academic project. Dear PVS-Studio, please check it.
# PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

CC = gcc
CFLAGS = -std=gnu99 -Wall -Werror -g -Wextra -Iinclude
LDFLAGS = -lm -lncurses -lrt -pthread
# CFLAGS = -std=gnu99 -Wall 
# CFLAGS = -std=gnu99 -Wall -Werror -g -Wextra -fsanitize=address -Iinclude


OBJS_PLAYER = player.o utils.o
OBJS_VIEW = view.o utils.o
OBJS_MASTER = master.o master_lib.o utils.o

PVS_ANALYZER = pvs-studio-analyzer
PVS_REPORT = plog-converter

VALGRIND = valgrind --leak-check=full


all: clean deps player master view

# all: deps view player

player: $(OBJS_PLAYER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

view: $(OBJS_VIEW)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

master: $(OBJS_MASTER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

format:
	clang-format -style=file --sort-includes --Werror -i *.c *.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o player master view PVS-Studio.html *log strace_out

deps:
	apt-get install -y libncurses5-dev libncursesw5-dev

analyze: clean 
	$(PVS_ANALYZER) trace -- make all
	$(PVS_ANALYZER) analyze -o PVS-Studio.log
	$(PVS_REPORT) -a GA:1,2,3,4 -t fullhtml PVS-Studio.log -o PVS-Studio.html

valgrind_player: player
	$(VALGRIND) ./player

valgrind_view: view
	$(VALGRIND) ./view

valgrind-master: master
	$(VALGRIND) ./master

.PHONY: all player view format clean deps analyse valgrind-view valgrind-player

# all player view format clean deps analyse valgrind-view valgrind-player valgrind-master
