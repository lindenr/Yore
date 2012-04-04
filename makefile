C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(C_FILES:%=obj/%.o)
LD_FLAGS := -lm -ggdb
CC_FLAGS := -I./ -ggdb

Yore: $(OBJ_FILES)
	gcc -o $@ $(OBJ_FILES) $(LD_FLAGS)

obj/%.c.o: src/*.c
	gcc $(CC_FLAGS) -c -o $@ $<
