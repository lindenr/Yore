C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(C_FILES))
LD_FLAGS := -lm -Wall -Werror -ggdb -O0
CC_FLAGS := -I$(CURDIR) -Wall -Werror -ggdb -O0

bin/Yore: $(OBJ_FILES)
	gcc -L./bin -lSDL -o $@ $(OBJ_FILES) $(LD_FLAGS)

obj/%.o: pre/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

pre/%.c: src/%.c
	./function $@ $< 

