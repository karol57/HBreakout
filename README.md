README
======
**HBreakout v0.0.1**

This is simple Breakout clone written in C++.
Project should compile without errors on GCC 5.4.0. If you are using different compiler then please change *CMAKE_CXX_FLAGS* in **CMakeList.txt**
Currently only available for Windows.

Before compiling update library paths *FREETYPE_INCLUDE_DIRS* and *FREETYPE_LIBRARIES* in **CMakeList.txt**
SDL2_image should be in the same directory as SDL2

Known bugs
----------
* Physics module: Collision rarely can't be detected (when in one frame ball will collide with the same object, for example level bounds, twice in a row).
* Physics module: Interlaced collision bounds can't hang up or crash application (stack overflow). 
* Physics module: Collision between ball and paddle is checked poorly (paddle is treated as static object).
* FPS widget: Doesn't show part of graph
* Me: My English skills aren't good either :)

TODOs
-----
* Fix bugs
* Lives
* Power-ups
* Multiple balls
* Smarter asset loading
* Level file format
* Level editor
* Menu
* Not random levels


Collision testing
-----------------
Currently implemented collisions are:
* Between two line segments
* Between line segment and circle

Collision between line segment and circle will show an normal vector.

Keybindings
-----------
* Left arrow - move paddle left
* Right arrow - move paddle right
* Space - shoot ball

### Temporary, for debugging
* Up arrow - move paddle up
* Down arrow - move paddle down
* Mouse wheel - Speed up/slow down ball
* ~ - Disable/Enable background clearing (for tracing ball path)
* 1/2 - Previous/Next background
* 3 - Show/Hide font cache
* 4 - Show/Hide FPS widget
* 5 - Show/Hide collision shapes
* 6 - Disable/Enable safe game area

#### Collision testing
* MouseLeft - draw line segment
* MouseRight - draw circle

Used libraries
--------------
* [SDL 2.0](https://www.libsdl.org/index.php)
* [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
* [FreeType2](https://www.freetype.org/)