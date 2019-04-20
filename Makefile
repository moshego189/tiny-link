SRC_DIR := ./src
OBJ_DIR := ./build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
CC := gcc-8
CFLAGS := -Wall -Werror
LDFLAGS := -static-pie
INC := -I./include 

all: bin/ld.so bin/dummy.elf 

bin/ld.so: $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

bin/dummy.elf: dummy/dummy.c
	$(CC) -s -nostdlib -static -o $@ $^ 

clean:
	rm bin/* build/*
