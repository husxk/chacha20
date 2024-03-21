

all: chacha20

chacha20:
	gcc chacha20.c -o chacha -g 

gdb:
	gdb chacha

.PHONY: gdb
