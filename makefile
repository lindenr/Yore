C_FILES := $(wildcard *.c)
OBJ_FILES := $(C_FILES:%=obj/%.o)
LD_FLAGS := -ggdb -lm
CC_FLAGS := -ggdb -lm

game_binary: $(OBJ_FILES)
	gcc -o $@ $(OBJ_FILES) $(LD_FLAGS)

obj/%.c.o: %.c
	gcc $(CC_FLAGS) -c -o $@ $<
