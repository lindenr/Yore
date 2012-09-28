C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(C_FILES))
LD_FLAGS := -lm -Wall -ggdb
CC_FLAGS := -I./ -Wall -ggdb

bin/Yore: $(OBJ_FILES)
	gcc -o $@ $(OBJ_FILES) $(LD_FLAGS)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<
