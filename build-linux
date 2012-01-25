#!/bin/bash
make -j 4 -B
cp ./game_binary ~/Desktop/game_binary
cd ~/Desktop
sudo chmod u+x game_binary
echo "Run the game??? (y) "
read ANSWER
if [ "$ANSWER" == "y" ]; then
    ./game_binary
fi
