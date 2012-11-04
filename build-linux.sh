#!/bin/bash
echo "[36;41m===STARTING BUILD===[0m"
make -B -j4
echo "Run the game??? (y) "
read ANSWER
if [ "$ANSWER" == "y" ]; then
    gdb ./bin/Yore
fi
