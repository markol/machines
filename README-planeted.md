planeted <planet file> -f <scrren width> <screen height> -s <scenario.scn> -a <artefact.arf>
The easiest way of launching the editor with proper parameters is by creating a shell (.bat/.sh) script like in examples.
Tool is clumsy and full of errors but it gets the job done, remember about backups and frequent saves.
Copy *models.bin* as *pemodels.bin*
If got a message: *No tile lods found for this planet <directory>* create a empty file `tile00.lod` under pointed directory.

Q : Exit
ESC + Shift : Exit
Hold CTRL for multiple selection

F1 : first person view
F2 : zenith view
F4 : free camera view
F5 : tile edit mode
    I : previous tile
	O : next tile
	P : current tile
	L : rotate tile clockwise
	K : rotate tile anti-clockwise
	U : tile up (1m)
	D : tile down (1m)
	Shift + U : tile up (10m)
	Shift + D : tile down (10m)

F6 : domain edit mode - ( + Ctrl : hide domains )
    Shift + F6 : hide domain
	D : drop new domain (at cursor pos)
	V : insert vertex (at cursor pos, after highlighted vertex)
	X : delete vertex ( Shift : domain )
	I : previous vertex ( Shift : domain )
	O : next vertex ( Shift : domain )
	Y : vertex up ( Shift : domain )
	B : vertex down ( Shift : domain )
	H : vertex right ( Shift : domain )
	G : vertex left ( Shift : domain )
	Alt + vertex drag: will move individual vertex rather than box
	F : 2d/3d domains

F7 : obstacle edit mode - ( + Ctrl : hide obstacles )
Shift + F7 : hide obstacle
	D : drop new obstacle (at cursor pos)
	V : insert vertex (at cursor pos, after highlighted vertex)
	X : delete vertex ( Shift : obstacle )
	I : previous vertex ( Shift : obstacle )
	O : next vertex ( Shift : obstacle )
	Y : vertex up ( Shift : obstacle )
	B : vertex down ( Shift : obstacle )
	H : vertex right ( Shift : obstacle )
	G : vertex left ( Shift : obstacle )
	K : raise 1m ( Shift : 10m )
	L : lower 1m ( Shift : 10m )
	F : 2d/3d obstacles
	P : Display vertical polygons ( + shift hide | + ctrl recompute )
	W,E,N : Define obstacle as water, low or normal obstacle
	W,E,N + shift : Show all obstacles of that type

F8 : portal edit mode - ( + Ctrl : hide portals )
    Shift + F6 : hide portal
	D : drop new portal (at cursor pos)
	Shift + X : delete portal
	I : previous vertex ( Shift : portal )
	O : next vertex ( Shift : portal )
	Y : vertex up ( Shift : portal )
	B : vertex down ( Shift : portal )
	H : vertex right ( Shift : portal )
	G : vertex left ( Shift : portal )
	F : 2d/3d portals
	P : Auto create portals
    
F9 : warnings on/off

1 : construction edit - ( + Ctrl : hide )
    L : rotate clockwise
	K : rotate anti-clockwise
	C : race 
    
2 : machine edit - ( + Ctrl : hide )
    C : race 
    
3 : artefact edit - ( + Ctrl : hide )
    L : rotate clockwise
	K : rotate anti-clockwise
	J ( + shift ): increase height 1m ( 10m )
	N ( + shift ): decrease height 1m ( 10m )
    
4 : camera edit - ( + ctrl : hide )
    D : mark camera position for this race
	C: show next race's camera position
    
F11 : create planet map
Ctrl + S : save
HOME : Enable profiling
END : Disable profiling 
