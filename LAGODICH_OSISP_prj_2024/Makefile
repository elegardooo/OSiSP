CC = gcc
CFLAGS = -W -Wall -Wextra -std=c11 -pedantic
LDFLAGS = -lncurses

.SUFFIXES:
.SUFFIXES: .c .o

DEBUG = ./build/debug
RELEASE = ./build/release
OUT_DIR = $(DEBUG)
MODE = release
vpath %.c src
vpath %.h src
vpath %.o build/debug

ifeq ($(MODE), release)
    CFLAGS = -std=c11 -pedantic -W -Wall -Wextra #-Werror
    LDFLAGS += -lncurses
    OUT_DIR = $(RELEASE)
    vpath %.o build/linux/release
endif

objects = $(OUT_DIR)/bdeditor.o $(OUT_DIR)/display.o $(OUT_DIR)/sector_operations.o
#objects = bdeditor.o

prog = bdeditor

all: $(prog)

$(prog) : $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@ $(LDFLAGS)

$(OUT_DIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	@rm -rf $(DEBUG)/* $(RELEASE)/* bdeditor