C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(C_FILES:%=obj/%.o)
LD_FLAGS := -lm -Wall -ggdb
CC_FLAGS := -I./ -Wall -ggdb

bin/Yore: $(OBJ_FILES)
	gcc -o $@ $(OBJ_FILES) $(LD_FLAGS)

obj/%.o: %
	gcc $(CC_FLAGS) -c -o $@ $<
