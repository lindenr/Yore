TARGET = Yore
CLFLAGS = -g -O0 #-fsanitize=address
LIBS = -lm $(shell sdl2-config --libs) $(CLFLAGS)
CC = gcc
CFLAGS = -I$(CURDIR) -funsigned-char -Wall -Werror $(shell sdl2-config --cflags) $(CLFLAGS)

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard include/*.h)

#$(wildcard auto/*): gen
#auto/%.h: gen-target

gen-target: codegen/gen.py $(wildcard gen/*)
	python codegen/gen.py
	touch gen-target

obj/%.o: src/%.c $(HEADERS) gen-target
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@
	./Yore

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f gen-target
