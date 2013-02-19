#Yore

Yore is an under-developement roguelike game.

##Build instructions

Yore can run on any machine which is supported by SDL.

####To build for Linux:

1.  you must have gcc and SDL installed (you probably will anyway);
2.  you must have [function](http://github.com/lindenr/function) binary in the Yore/ directory;
3.  [optional] get rid of the -ggdb flags in makefile;
4.  run <pre>./build-linux.sh</pre>

####To build for Windows:

NOTE: function has not been tested on Windows

1.  you must have MinGW and SDL installed, with SDL.dll in your bin/ folder;
2.  [optional] get rid of the -ggdb flags in makefile;
3.  run <pre>PATH &lt;your_mingw_install_path&gt;\bin<br />build-win</pre>

The executable is created as ./bin/Yore (Linux) or bin\Yore (Windows).


###Yore runs (tested) on the following platforms:

 *  Ubuntu 11.10
 *  Windows XP
 *  Windows 7
 *  Arch

Feel free to add to the list if you have a different operating system handy.

##Gameplay

Gameplay in Yore is largely similar to NetHack. 

###Movement

Movement with the hjkl set (yubn for diagonals) is supported; running using a capital letter (HJKLYUBN) is not implemented yet.
The arrow keys move the view around (the map is probably too large to fit all on to the screen at once).

###Miscellaneous

Various other commands are (in no particular order):
<pre>e      eat
,      pick up
:      look down
i      check your inventory
w      wield a weapon
d      drop an item from your inventory

S      save and quit
Q      quit permanently</pre>

##License

Yore is distributed under the GNU GPL v3.

