TARGET = rmf

DIR := ${CURDIR}
SRC_DIR = $(DIR)/src
APPDIR = $(HOME)/.rmf

C = gcc
FLAGS = -g -Wall -Wextra -Werror -Wformat

SRCS = $(SRC_DIR)/main.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=%.o)


.PHONY: all clear

all: $(TARGET)

# executable assembly
$(TARGET): $(OBJS)
	$(C) $(FLAGS) -o $@ $^

# source compilation
main.o: $(SRC_DIR)/main.c $(SRC_DIR)/errors.h
	$(C) $(FLAGS) -c $< -o $@

# install
install: $(TARGET)
	@mkdir -p $(HOME)/.rmf
	@make clear_obj

# cleanup
clear:
	@rm -rf $(TARGET) $(OBJS)

clear_obj:
	@rm -rf $(OBJS)

# uninstall
uninstall:
	@rm -rf $(APPDIR)
	@make clear