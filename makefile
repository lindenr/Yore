C_FILES := $(wildcard src/*.c)
H_FILES := $(wildcard include/*.h)
#OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(C_FILES))
LD_FLAGS := -lm -Wall -Werror -ggdb -O0
CC_FLAGS := -I$(CURDIR) -Wall -Werror -ggdb -O0
SDL_FLAGS = $$(sdl-config --cflags --libs)

ALL: bin/Yore
	gdb ./bin/Yore

g: bin/Yore
	./bin/Yore gen

bin/Yore: $(C_FILES) $(H_FILES) makefile
	@echo "[36;41m===STARTING BUILD===[0m"
	gcc $(SDL_FLAGS) -o $@ $(C_FILES) $(CC_FLAGS) $(LD_FLAGS)
