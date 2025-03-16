TARGET = rmf

DIR := ${CURDIR}
SRC_DIR = $(DIR)/src
APPDIR = $(HOME)/.rmf

C = gcc
FLAGS = -g -Wall -Wextra -Werror -Wformat

SRCS = main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clear

all: $(TARGET)

# executable assembly
$(TARGET): $(OBJS)
	$(C) $(FLAGS) -o $@ $^

# source compilation
%.o: $(SRC_DIR)/main.c $(SRC_DIR)/errors.h
	$(C) $(FLAGS) -c $<

# install
install: $(TARGET)
	@echo ==== RMF INSTALLATION ====
	@echo create working directory $(APPDIR) ...
	mkdir -p $(HOME)/.rmf
	@echo copy binary into $(APPDIR) ...
	cp $(TARGET) $(APPDIR)/
	make clear_obj
	@echo ===== RMF INSTALLED ======

# cleanup
clear:
	rm -rf $(TARGET) $(OBJS)

clear_obj:
	rm -rf $(OBJS)

# uninstall
uninstall:
	@echo ==== RMF UNINSTALL ====
	@echo remove working directory ...
	rm -rf $(APPDIR)
	@echo ===== RMF DELETED =====
