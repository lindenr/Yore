C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(C_FILES))
PRE_FILES := $(patsubst src/%.c,pre/%.c,$(C_FILES))
LD_FLAGS := -lm -Wall -Werror -ggdb -O0
CC_FLAGS := -I$(CURDIR) -Wall -Werror -ggdb -O0

bin/Yore: $(PRE_FILES)
	gcc -L./bin -o $@ $(PRE_FILES) $(CC_FLAGS) $(LD_FLAGS) -lSDL

pre/%.c: src/%.c
	./function $@ $< 

