#!/bin/bash
mkdir -p obj
mkdir -p bin
make -j4 -B
echo "Run the game??? (y) "
read ANSWER
if [ "$ANSWER" == "y" ]; then
    ./bin/Yore
fi
