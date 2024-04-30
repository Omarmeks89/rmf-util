DIR := ${CURDIR}

build:
	gcc -Wall -Werror -Wextra -g -o rmf -std=c99 $(DIR)/src/main.c

clear:
	rm -rf *.txt && rm rmf
