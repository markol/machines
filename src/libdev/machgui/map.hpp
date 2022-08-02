/*
 * M A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiContinentMap

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MAP_HPP
#define _MACHGUI_MAP_HPP

#include "base/base.hpp"
#include "ctl/fixedvec.hpp"
#include "ctl/vector.hpp"
#include "mathex/line3d.hpp"
#include "utility/id.hpp"
#include "system/pathname.hpp"
#include "gui/displaya.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"

//Forward refs
class MachInGameScreen;
class MachGuiBmuText;
class MachCameras;
class MachGuiTerrainOnOffButton;
class MachGuiMapModeButton;

class MachContinentMap : public GuiDisplayable
{
public:
	MachContinentMap( 	GuiDisplayable* pParent, 
						const Gui::Coord& rel,
						MachCameras* pCameras,
						MachInGameScreen* pInGameScreen );
	virtual ~MachContinentMap() {};

	// Refresh the position of one of the races ( cycles through each race ).
	void refresh();

	MachGuiBmuText* bmuText();

	// Cause map to be completely refreshed, i.e. update race positions, update camera position
	// and redraw whole map.
	void forceUpdate();

    // Setup map with correct background bitmap, update map to world metrics etc.
    void loadGame( const string& planet );

    void unloadGame();

	// Ability to switch "Fog Of War" on/off
	void fogOfWarOn( bool );

	// change map mode
	enum MapMode { UNITS_ONLY, RESOURCES_ONLY, ALL };
	void mapMode( MapMode );
	MapMode mapMode() const;

	// Load/Save map info ( i.e. FOW bitmap )
	void saveGame( PerOstream& outStream );
	void loadSavedGame( const string& planet, PerIstream& inStream );

protected:
	// inherited from GuiDisplayable...
	virtual void doDisplay();
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& ); 
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

	bool findActor( const GuiMouseEvent&, MachActor** );
	
	void positionCamera( const GuiMouseEvent& rel );

	MATHEX_SCALAR mapToWorldRatio() const;
	size_t xOffset() const;
	size_t yOffset() const;

	void updateScreen();
	void updateMapBackground();
	void updateMapBackground2();
	void updateMapFrameOne( size_t index );
	void updateMapFrameTwo( size_t index );
	void updateVisibleAreas( size_t index );
	void updateRacePos();
	void drawCameraPos( GuiBitmap* pMapFrame );
	void updateBeacon( bool forceUpdate = false );
	void refreshLastFrame();
	void refreshFrame();

	void updateMapToWorldMetrics();

	static GuiBitmap* machineImage();
	static GuiBitmap* constructionImage();
	static GuiBitmap* podImage();
	static GuiBitmap* selectedMachineImage();
	static GuiBitmap* selectedConstructionImage();
	static GuiBitmap* selectedPodImage();
	static GuiBitmap& debrisImage();
	static GuiBitmap& artifactImage();
	static GuiBitmap& oreImage();
	static GuiBitmap& attackedMachineImage();
	static GuiBitmap& selectedAttackedMachineImage();
	static GuiBitmap& attackedPodImage();
	static GuiBitmap& selectedAttackedPodImage();
	
	// Returns a line that starts at the camera position and travels through a specified
	// point on the screen.
	MexLine3d fromCameraToScreenLine( const Gui::Coord& screenCoord ) const;

	// Translates from a point on the screen to a 3D point on the planet, it then translates the
	// planet coord ( x,y,0 ) into a coord for the continent map.
	Gui::Coord translate3DScreenCoordToMapCoord( const Gui::Coord& screenCoord ) const;

	void updateMapCameraRepresentation();

	enum FogOfWarLevel { NO_FOW, TEND_TO_FIFTY_PERCENT_FOW, TEND_TO_FULL_FOW, FULL_FOW };
	 
	FogOfWarLevel getFogOfWarLevel( const Gui::Coord& checkPos );
	void update3dFogOfWarLightLevel();

	void handleIntelligentCursor( const GuiMouseEvent& rel, bool buttonClicked );
	
private:
	enum ActorState { NORMAL = 0, ATTACKED = 1, ATTACKING = 2, SELECTED = 4 };
	enum Type { CONSTRUCTION, MACHINE, POD, ARTIFACT, DEBRIS, ORE, CAMOUFLAGEDMACHINE };
	
	typedef char BYTE;

	struct ActorPosInfo
	{
		Gui::Coord drawPos_; 	// Screen pos to draw actor marker
		Gui::Coord actualPos_; 	// Screen pos for centre of actor
		Gui::Coord worldPos_;	// World pos of actor
		MachPhys::Race race_;
		MachPhys::Race displayAsRace_;
		UtlId actorId_;
		Type type_;
		ActorState actorState_;
		BYTE scanner_; /* scanner size 1-5*/
	};

	ctl_vector< ActorPosInfo > actorPositions_;
	ctl_vector< ActorPosInfo > secondFrameActorPositions_;
	MATHEX_SCALAR 	mapToWorldRatio_;
	GuiBitmap		mapBackground_;
	GuiBitmap   	mapFrameOne_;
	GuiBitmap   	mapFrameTwo_;
	GuiBitmap 		cameraPosImage_;
	GuiBitmap   	mapVisibleArea_;
	GuiBitmap   	scannerRangeImage_[5];
	size_t   		scannerRange_[5];
	Gui::Coord 		cameraPos_;
	Gui::Coord 		cameraEndPos_;
	size_t 			xOffset_;
	size_t 			yOffset_;
	size_t 			mapRefreshCounter_;
	size_t 			firstFrameNumActors_;
	size_t 			secondFrameNumActors_;
	size_t 			numActorsPerFrame_;
	bool 			firstDraw_;
	MachGuiBmuText* pBmuText_;
	MachCameras* 	pCameras_;
	MachPhys::Race 	playerRace_;
	MATHEX_SCALAR 	beenHereXRatio_;
	MATHEX_SCALAR 	beenHereYRatio_;
	bool 			zenithCamera_;
    Gui::Coord 		cameraFov_[4];
	MapMode 		mapMode_;
	SysPathName 	mapDir_;
	bool 			fogOfWarOn_;
	double 			fogOfWarFrameTimer_;
	double 			fogOfWarLightLevel_;
	FogOfWarLevel 	fogOfWarLevel_;
	MachInGameScreen* pInGameScreen_;
	MachLog::BeaconType currentBeacon_;
	MachGuiTerrainOnOffButton* pTerrainOnOffButton_;
	MachGuiMapModeButton* pMapModeButton_;
	ctl_fixed_vector<BYTE> *pBeenHere_; // Stores the largest scanner type that has visited an area of the map.
	bool noFastChangeInLightLevel_;
};

#endif

/* End MAP.HPP ******************************************************/
