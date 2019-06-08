TARGET = Yore
CLFLAGS = -Wall -Werror -Ofast #-fsanitize=address
LIBS = -lm $(shell sdl2-config --libs) $(CLFLAGS) -lGL -lGLEW
CC = gcc
CFLAGS = -I$(CURDIR) -funsigned-char $(shell sdl2-config --cflags) $(CLFLAGS)

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard include/*.h)

mark-gen: codegen/gen.py $(wildcard gen/*)
	python codegen/gen.py
	touch mark-gen

obj/%.o: src/%.c $(HEADERS) mark-gen
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@
	./Yore

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f mark-gen
	-rm -f auto/*
