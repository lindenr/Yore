C_FILES := $(wildcard src/*.c)
H_FILES := $(wildcard include/*.h)
#OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(C_FILES))
LD_FLAGS := -Wall -Werror
CC_FLAGS := -I$(CURDIR) -Wall -Werror -funsigned-char -O2 -g
SDL_FLAGS = $$(sdl2-config --cflags --libs)

ALL: Yore
	gdb ./Yore

Yore: $(C_FILES) $(H_FILES) makefile
	@echo "[36;41m===STARTING BUILD===[0m"
	gcc $(SDL_FLAGS) -o $@ $(C_FILES) $(CC_FLAGS) $(LD_FLAGS)
