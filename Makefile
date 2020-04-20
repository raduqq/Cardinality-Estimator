#Copyright 2020 Radu-Stefan Minea 314CA

.PHONY: build clean

CC = gcc
FLAGS = -Wall -Wextra -std=c11

build: freq hash

freq: freq.c
	$(CC) $(FLAGS) -o $@ $<

#hll: hll.c
#	$(CC) $(FLAGS) -o $@ $(LIBS) $<

hash: hash.c
	$(CC) $(FLAGS) -o $@  $<

clean:
	rm freq
	rm hash
#	rm hll
