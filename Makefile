# This is a personal academic project. Dear PVS-Studio, please check it.
# PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

CC = gcc
CFLAGS = -std=gnu99 -Wall -Werror -g -Wextra -Iinclude
LDFLAGS = -lm -lncurses -lrt -pthread


OBJS_PLAYER = player.o player_lib.o utils.o
OBJS_VIEW = view.o view_lib.o utils.o
OBJS_MASTER = master.o master_lib.o utils.o

PVS_ANALYZER = pvs-studio-analyzer
PVS_REPORT = plog-converter

VALGRIND = valgrind --leak-check=full --trace-children=yes --suppressions=ncurses.supp --tool=memcheck


all: deps player master view

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
	$(PVS_ANALYZER) trace -o strace_out -- make -B all
	$(PVS_ANALYZER) analyze -o PVS-Studio.log
	$(PVS_REPORT) -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t fullhtml PVS-Studio.log -o PVS-Studio.html

valgrind: deps master player view
	$(VALGRIND) ./master -p ./player -v ./view

.PHONY: all player view format clean deps analyze valgrind

