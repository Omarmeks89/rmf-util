DIR := ${CURDIR}

build:
	gcc -Wall -Werror -Wextra -g -o rmf $(DIR)/src/main.c -L/usr/local/lib -lunistring

clear:
	rm -rf *.txt && rm rmf
