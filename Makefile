build:
	gcc -Wall -Werror -Wextra -g -o rmf -std=c99 src/main.c

clear:
	rm -rf *.txt && rm rmf
