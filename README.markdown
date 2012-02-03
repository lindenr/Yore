#Yore

Yore is an under-developement roguelike console game.

##Build instructions:

At the moment Yore is capable of running only on Windows and Linux systems.

####To build for Linux:

1.  you must have make and gcc installed;
2.  [optional:] get rid of the -ggdb flags in makefile;
3.  [optional:] open build-linux (if you leave it, game_binary will be copied to ~/Desktop/) and comment out lines 3-4;
4.  run <pre>bash build-linux</pre>

####For Windows:

1.  you must have MinGW installed;
2.  run <pre>PATH &lt;your_mingw_install_path&gt;\bin<br />makefile.bat</pre>


###Yore runs (tested) on the following platforms:

 *  Ubuntu 11.10
 *  Windows XP
 *  Windows 7

Feel free to add to the list if you have a different operating system handy.

##Gameplay

Movement in Yore is largely similar to NetHack, so NetHack players have to relearn the m

###Movement

Movement with the hjkl set (yubn for diagonals) is supported; running using a capital letter (HJKLYUBN) is not implemented yet.
You may also use the arrow keys to move, but you can't move diagonally. Also, don't try moving through typed text with the arrow
keys - they have been mapped directly to hjkl for movement.


