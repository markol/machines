# Machines: Wired For War
### 3D Real Time Strategy video game GL port source code
Released as a [free software](https://en.wikipedia.org/wiki/Free_software) under a permission of [Night Dive Studios](https://www.nightdivestudios.com/) who owns the rights to the game.

**Followed conventions:**
- file names use a DOS convention (8 chars + 3 for extension), not strictly followed any more,
- macros are all UPPER_CASE in code, sometimes it may be enum cases, 
- classes are named WithUpperCaseLetter on the beginning and between words,
- class private members are named with dash on the end someVar_ and with lower case on the beginning,
- pointer names start from p pPointerVar_,
- some class members are "seamleslly" declared from implClass with CB_DEPIMPL macro, 
- use _NEW, _NEW_ARRAY, _DELETE, _DELETE_ARRAY, _STATIC_CAST, _CONST_CAST macros instead,
- comments added now to cut out the code starts from the beginning of the line// (no spaces before),
- std::string and string are different classes - the second one is added to support persistence (dump to/load from binfile),
- old watcom compiler did not support namespaces,
- templates were instantiated explicitly in separate sources, this was commented out in most of the cases (not sure if it was reasonable),

**The main code is spilt into a set of libraries:**
- afx - SDL app window creation, string resources handling,
- ani - smacker animations,
- base - compiler type things, new and delete overloaded global operators, leak and stack tracker (not used), persistence and logging mechanisms,
- ctl - charybdis template library - this was created to improve STL functionality, now it is a wrapper around gnu stl - note it uses public inheritance from std containers, found no problems yet? Only ctl containers support persistence,
- device - mice, keyboards, timers, (back when they did that stuff and didn't just rely on direct x), cd audio (music track),
- envirnmt - maps, sky loading code,
- gui - base classes for gui concepts (buttons, list, scrolling dialog windows and such like),
- gxin - parser for old .agt text format meshes,
- machgui - the real machines based UI,
- machlog - the logic stuff, AI, choosing levels and such like,
- machphys - the renderable bits of machines (agressors, administrators, mines, etc),
- mathex - maths libraries,
- network - low level communication layer,
- phys - base classes of some physics types things - this is NOT real time physics - by how to fiddle it,
- profiler - not used now,
- recorder - allows to record and replay all the  in game activity, events, random seeds etc. Game works in single thread, useful when reproducing a bug (not checked if it works),
- render - base library for rendering primitives, triangles, meshes, textures, materials, surfaces, etc,
- sim - base classes for things that can be simulated,
- sound - loading and playing wave samples 2D/3D,
- stdlib - now a little has left from it (there was cout, cerr, etc ),
- system - mostly file device handling, msgbox, metafile, registry (xml config),
- utility - helpers of various types, e.g. file parsers. again a lot of this stuff you wouldn't bother with now but back then there was no source forge or even google, 
- world4d - built on top of the render libraries, turned basic rendering into multi-mesh entities (W4dEntity is a base class for lot of things),
- xin - .X mesh file loading into game engine (added now),

**Build dependencies**
Use provided Docker image for building or manually setup your environment.

`libsdl2-dev libsdl2-image-dev libopenal-dev libalure-dev libenet-dev libfreetype6-dev libglew-dev libglm-dev libaudio-dev libswscale-dev`

**Building**
`git clone && git lfs pull`

***Windows exe on linux/docker:***
+ `sudo apt-get install mingw-w64`
+ `mkdir buildMingw64 && cd buildMingw64`
+ `cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw64.cmake ..`
+ or 32-bit cpu:
+ `mkdir buildMingw32 && cd buildMingw32`
+ `cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake -DBUILD_32=1 ..`

Extra parameters for release optimizations: 
`-DCMAKE_BUILD_TYPE=RELEASE -DDEV_BUILD=OFF`

***For linux:***
+ `mkdir build && cd build`
+ `cmake ..`
+ `make -j 9`

For ARM SoC development boards (like Raspberry or Nitrogen8M) if you do not have cross compilation toolchain prepared it will be easier to boot-up one of dedicated system distribution and build it within this enviroment (takes approx one hour for 4 cores).

**Making a distribution (Collect everything for installation)**
Run `make distribution` to collect all of the data files, graphics files, and compiled binaries into a single folder. You may then copy the contents of the `distribution` folder into your game's directory.

**Class invariant checks (for debugging)**
`export CB_INVARIANT_CHECKING=on`

**Debug streams**
To turn on a stream define environment variable with correct name, eg. `set CB_HAL_TO=cout` or in linux shell `export CB_HAL_TO=cout`.
Target cout is a current shell of course, replace it with filename to write log into or leave empty to turn off.
Avaiable streams: (*base/diag.hpp*)
- BOB
- HAL
- IAIN
- JERRY
- RICHARD
- JON
- YUEAI
- LIONEL
- CERI
- NEIL
- MISC
- WHERE
- TRACE
- MEMORY
- RENDER
- PER_READ
- PER_WRITE
- NETWORK
- WAYNE
- DANIEL
- MOTSEQ
- CS2PATH         // Config space pathfinding stuff (domain and normal)
- CS2VGRA         // Visibility graph diagnostics
- OBSERVER        // trapping observer pattern problems - stream added to due volume of output
- PATH_PROFILE    // Debugging the path profile across the planet surface
- SOUND
- REC_RECORD
- REC_PLAYBACK
- NET_ANALYSIS
- A_STAR
- LINEAR_TRAVEL      // Stream for use by PhysLinearTravelPlan
- PLANET_SURFACE     // Used for reporting errors in a planet surface
- PHYS_MOTION        // Physical motion stream
- NETWORK_ERRORS     // Specifically to help us spot network errors (such as loss of synchronicity)
- MEX_GRID2D
- CONFIG_SPACE
