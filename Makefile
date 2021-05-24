.PHONY: all

all: barry

barry: barry.c
	gcc barry.c -o barry
