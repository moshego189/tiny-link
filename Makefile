SRC_DIR := ./src
OBJ_DIR := ./build
BIN_DIR := ./bin
INCLUDE_DIR := ./include

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

CC := gcc-8
CFLAGS := -Wall -Werror -Wextra
LDFLAGS := -static-pie

all: $(BIN_DIR)/ld.so $(BIN_DIR)/dummy.elf 

$(BIN_DIR)/dummy.elf: dummy/dummy.c
	$(CC) -s -nostdlib -static -o $@ $^ 

$(BIN_DIR)/ld.so: $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

clean:
	rm $(BIN_DIR)/* $(OBJ_DIR)/*
