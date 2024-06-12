CC = gcc
CFLAGS_RELEASE = -std=c11 -pedantic -W -Wall -Wextra -Werror
CFLAGS_DEBUG = -g -ggdb -std=c11 -pedantic -W -Wall -Wextra

.PHONY: all clean

MODE = release

ifeq ($(MODE), release)
	CFLAGS = $(CFLAGS_RELEASE)
	TARGET = main
else
	CFLAGS = $(CFLAGS_DEBUG)
	TARGET = main_debug
endif

all: $(TARGET) generate

$(TARGET): src/main.c
	$(CC) $(CFLAGS_RELEASE) $^ -o $@ -lm

generate: src/generate.c
	$(CC) $(CFLAGS_RELEASE) $^ -o $@ 

clean:
	rm -f main main_debug generate 