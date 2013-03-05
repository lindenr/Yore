#Yore

Yore is an under-developement roguelike game.

##Build instructions

Yore can run on any machine which is supported by SDL.

####To build for Linux:

1.  you must have gcc and SDL installed (you probably will anyway);
2.  you must have the [function](http://github.com/lindenr/function) binary in the top directory;
3.  run
<pre>$ ./build-linux.sh</pre>

####To build for Windows:

1.  you must have MinGW and SDL installed, with SDL.dll in your bin/ folder;
2.  you must have the [function](http://github.com/lindenr/function) executable in the top folder;
3.  run
<pre>&gt; PATH &lt;your_mingw_install_path&gt;\bin
&gt; build-win</pre>
If that doesn't work then try
<pre>&gt; mingw32-make</pre>

The binary "Yore" (or "Yore.exe") is in the bin/ directory.

###Yore runs (tested) on the following platforms:

 *  Arch Linux
 *  Ubuntu 11.10
 *  Windows XP, 7, 8

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

