#!/bin/bash
make -j4 -B
echo "Run the game??? (y) "
read ANSWER
if [ "$ANSWER" == "y" ]; then
    ./bin/Yore
fi
