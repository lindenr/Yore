C_FILES := $(wildcard *.c)
OBJ_FILES := $(C_FILES:.c=.o)
LD_FLAGS := -ggdb
CC_FLAGS := -ggdb

game_binary: $(OBJ_FILES)
	gcc $(LD_FLAGS) -o $@ $(OBJ_FILES)

%.o: %.c
	gcc $(CC_FLAGS) -c -o $@ $<
