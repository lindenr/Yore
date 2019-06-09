# Yore

Yore is an under-developement roguelike game.

## Build instructions

Yore should run on any machine which is supported by SDL. It has only been tested on those mentioned below, however.

#### To build for Linux:

1.  you must have gcc and SDL installed (you probably will anyway);
2.  run

        $ make

#### To build for Windows:

1.  you must have MinGW and SDL installed, with SDL.dll in your bin/ folder;
2.  run

        > PATH <your mingw install path>\bin
        > build-win

    If that doesn't work then try

        > mingw32-make

The binary "Yore" (or "Yore.exe") is in the bin/ directory.

### Yore runs (tested) on the following platforms:

 *  Arch Linux

Feel free to add to the list if you have a different operating system handy.

## Gameplay

Gameplay in Yore is largely similar to NetHack. 

### Movement

Movement with the hjkl set (yubn for diagonals) is supported.
The arrow keys move the view around (the map is probably too large to fit all on to the screen at once).

### Miscellaneous

Various other commands are (in no particular order):
```
,      pick up
:      look down
;      look around
i      view inventory
w      wield a weapon
d      drop an item from your inventory

F      force attack in a direction
m      force movement in a direction
<Esc>  view status screen
s      view skills menu
f      enter/exit focus mode
p      shield (protect) in a direction

S      save and quit
Q      quit permanently
```

## License

Yore is distributed under the GNU GPL v3, a copy of which may be found [here](http://www.gnu.org/licenses/gpl.html).

