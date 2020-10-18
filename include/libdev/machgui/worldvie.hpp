/*
 * W O R L D V I E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachWorldViewWindow

    Implements the Gui responsibilities of the world view window in the in-game screen.
    Does not control rendering.
*/

#ifndef _MACHGUI_WORLDVIE_HPP
#define _MACHGUI_WORLDVIE_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "gui/gui.hpp"
#include "gui/displaya.hpp"
#include "utility/id.hpp"
#include "world4d/world4d.hpp"
#include "world4d/bbxselec.hpp"

//Forward refs
class MachInGameScreen;
class GuiEvent;
class MexLine3d;
class W4dGeneric;
class MachCameras;
class DevKeyToCommandTranslator;

//Orthodox canonical (revoked)
class MachWorldViewWindow : public GuiDisplayable
{
public:

	MachWorldViewWindow( MachInGameScreen * pParent, 
						 const Gui::Boundary& relativeBox,
						 MachCameras* pCameras );

	virtual ~MachWorldViewWindow();

    //Do start/end game processing
    void loadGame();
    void unloadGame();

	void update();

	// If true then the world view window is currently involved in a rubber band selection
	bool rubberBandSelectionHappening() const;

	// Pass button event to worldview window for "hot key" stuff. True returned indicates
	// that worldview window used button event.
	bool processButtonEvent( const DevButtonEvent& buttonEvent );

 	//Id of any highlighted actor
	UtlId highlightedActorId() const;
    bool haveHighlightedActor() const;

protected:
    //A line in world space from the camera origin passing through the cursor of length length
    MexLine3d cameraThroughCursorLine( const Gui::Coord& cursorScreenPoint,	MATHEX_SCALAR length ) const;

	// inherited from GuiDisplayable...
	virtual void doDisplay();
    virtual void doHandleMouseClickEvent( const GuiMouseEvent& );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& );

	void doMouseRightClickCameraMotion( const GuiMouseEvent& );

private:
    //Revoked
    MachWorldViewWindow( const MachWorldViewWindow& );
    MachWorldViewWindow& operator =( const MachWorldViewWindow& );
    bool operator ==( const MachWorldViewWindow& );

    //Deal with a cursor event, after translating cursor position to a 3d line in
    //world space. 
    void dispatchCursor( const MexLine3d& cursorLine, bool click, bool ctrlPressed,
                         bool shiftPressed, bool altPressed );

	void updateActorsSelectedViaRubberBand( bool highlightOnly, bool keepSelection, bool inclMachines = true, bool inclConstructions = true );
	void unhighlightActorsSelectedViaRubberBand();
		 
	enum RubberBandCamera { ZENITH, GROUND, INVALID };
	enum CommandId {SELECT_ONSCREEN_MACHINES, SELECT_ONSCREEN_CONSTRUCTIONS, 
					SELECT_ONSCREEN_MACHINES_KEEP_SEL, SELECT_ONSCREEN_CONSTRUCTIONS_KEEP_SEL }; 

    //Data members
    MachInGameScreen* pInGameScreen_; 		//The owning in-game displayable
    UtlId 			highLightedActorId_; 	//Id of any highlighted actor
    bool 			haveHighlightedActor_; 	//True if there is a highlighted actor
	bool 			mouseButtonPressed_;
	MachCameras* 	pCameras_;
	Gui::Coord 		startRubberBand_;
	Gui::Coord 		endRubberBand_;
	MexPoint3d 		startRubberBand3DPos_;
	RubberBandCamera rubberBandCamera_;		//Camera used when rubber banding started
	bool 			rubberBanding_;			//is rubber banding operation happening
	ctl_vector< W4dId > selectedEntities_;
	DevKeyToCommandTranslator* pKeyTranslator_;	
};


#endif

/* End WORLDVIE.HPP *************************************************/
