/*
 * I G C U R S 2 D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachInGameCursors2d

    Supports the loading and switching between all the in-game screen 2d cursors
*/

#ifndef _MACHGUI_IGCURS2D_HPP
#define _MACHGUI_IGCURS2D_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "machgui/gui.hpp"

//Forward refs
class RenAnimCursor2d;
class MachInGameScreen;

//orthodox canonical (revoked)
class MachInGameCursors2d
{
public:
    //ctor.
    MachInGameCursors2d( MachInGameScreen* pInGameScreen );

    //dtor
    ~MachInGameCursors2d();

    //Get/Select the appropriate current cursor
    void cursor( MachGui::Cursor2dType type );
    MachGui::Cursor2dType type() const;

	// Same as "cursor" only there is no initial check that we are
	// selecting the same cursor again. This should be used if a
	// class other that MachInGameScreen has at some point changed the
	// cursor.
	void forceCursor( MachGui::Cursor2dType type );

	// Change between small and large cursors ( small used on continent map )
	enum CursorSize { LARGECURSORS, SMALLCURSORS };
	void cursorSize( CursorSize );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachInGameCursors2d& t );

private:
    // Operations deliberately revoked
    MachInGameCursors2d( const MachInGameCursors2d& );
    MachInGameCursors2d& operator =( const MachInGameCursors2d& );
    bool operator ==( const MachInGameCursors2d& );

    //Load up all the cursors
    void loadCursors();
    RenAnimCursor2d* loadCursor( const string& baseName, int nFrames );
   
    //Data members
    RenAnimCursor2d* pMenuCursor_; 			//The one over the menus etc
    RenAnimCursor2d* pMoveCursor_[2];		//Move to terrain location
    RenAnimCursor2d* pEnterBuildingCursor_ ;//Enter a building
    RenAnimCursor2d* pNoEntryCursor_[2];	//Invalid pick
    RenAnimCursor2d* pAttackCursor_; 		//Attack something with non-ion and non-nuke weapons
	RenAnimCursor2d* pIonAttackCursor_; 	//Attack something with an ion beam
	RenAnimCursor2d* pNukeAttackCursor_; 	//Attack something with a nuclear strike
    RenAnimCursor2d* pConstructCursor_; 	//Construct a building
    RenAnimCursor2d* pLocateToCursor_;		//Locate along path
    RenAnimCursor2d* pPatrolCursor_;		//Patrol up and down path
    RenAnimCursor2d* pDeployCursor_; 		//APC deploy contents
    RenAnimCursor2d* pPickUpCursor_; 		//Collect ore/BMUs
	RenAnimCursor2d* pScavengeCursor_; 		//Scavenge debris
    RenAnimCursor2d* pTransportCursor_;		//Deliver ore/debris to smelter
    RenAnimCursor2d* pEnterAPCCursor_;		//Enter an APC
    RenAnimCursor2d* pFollowCursor_;		//Enter an APC
    RenAnimCursor2d* pSelectCursor_[2];		//Select the entity
    RenAnimCursor2d* pDropLandMineCursor_;	//drop a land mine
    RenAnimCursor2d* pHealCursor_;			//Heal a machine cursor
	RenAnimCursor2d* pDeconstructCursor_; 	//Deconstruct a building cursor
	RenAnimCursor2d* pRecycleCursor_; 		//Recycle a machine cursor
	RenAnimCursor2d* pRepairCursor_; 		//Repair cursor
	RenAnimCursor2d* pCaptureCursor_; 		//Capture a building cursor
	RenAnimCursor2d* pJoinConstructCursor_; //Join in on a construction cursor
	RenAnimCursor2d* pAssemblePointCursor_; //Assemble point cursor
	RenAnimCursor2d* pScrollWestCursor_;	//When camera is scrolling left
	RenAnimCursor2d* pScrollEastCursor_;	//When camera is scrolling right
	RenAnimCursor2d* pScrollNorthCursor_;	//When camera is scrolling up
	RenAnimCursor2d* pScrollSouthCursor_;	//When camera is scrolling down
	RenAnimCursor2d* pScrollNorthWestCursor_;//When camera is scrolling up and left
	RenAnimCursor2d* pScrollNorthEastCursor_;//When camera is scrolling up and right
	RenAnimCursor2d* pScrollSouthWestCursor_;//When camera is scrolling down and left
	RenAnimCursor2d* pScrollSouthEastCursor_;//When camera is scrolling down and right
	RenAnimCursor2d* pChooseConstructionCursor_;//When construct has been selected but no construction
	RenAnimCursor2d* pTreacheryCursor_;		//Target an enemy machine to be treacheried
	MachGui::Cursor2dType currentType_; 	//The current cursor
    MachInGameScreen* pInGameScreen_; 		//The gui root
	CursorSize cursorSize_;					//Cursor size used ( some cursors come in two sizes )
};


#endif

/* End IGCURS2D.HPP *************************************************/
