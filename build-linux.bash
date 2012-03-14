#!/bin/bash
make -j4 -B
echo "Run the game??? (y) "
read ANSWER
if [ "$ANSWER" == "y" ]; then
    ./game_binary
fi
