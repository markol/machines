/*
 * C A M E R A S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachCameras

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CAMERAS_HPP
#define _MACHGUI_CAMERAS_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "device/timer.hpp"
#include "device/butevent.hpp"
#include "gui/gui.hpp"

#include "world4d/observer.hpp"

//class DevButtonEvent;
class DevKeyToCommandTranslator;
class PhysFlyControl;
class PhysZenithFlyControl;
class PhysGroundFlyControl;
class MachLogZenithCameraMotionConstraint;
class MachLogGroundCameraMotionConstraint;
class MachLogCamera;
class MachActor;
class W4dSceneManager;
class W4dRoot;
class GuiMouseEvent;

class MachCameras : public W4dObserver
// Canonical form revoked
{
public:
    MachCameras( W4dSceneManager*, W4dRoot* );
	// PRE( pSceneManager );
	// PRE( pRoot );

    virtual ~MachCameras();

    void CLASS_INVARIANT;

    //Create the cameras and controls for a game
    void loadGame();

    //Delete the game's cameras and controls
    void unloadGame();

	// Called every time there is a buttonEvent. Returns true if the buttonEvent was
	// processed (e.g. buttonEvent caused camera rotation).
	bool processButtonEvent( const DevButtonEvent& buttonEvent );

	// Called every frame before rendering to ensure that cameras are in the correct
	// domain etc.
	void updateCameras();

	MachLogCamera* currentCamera();
	// PRE( pCurrentCamera_ != NULL );

	// Make camera move to new position ( assuming camera can move without
	// ending up inside an obstacle etc ).
	void moveTo( const MexPoint2d& );

	// Make camera move into a position where it can look at the MachActor.
	// Note that the camera can automatically switch between ground,
	// zenith and 3rd person depending upon the actor type.
	void setFollowTarget( MachActor* pActor);
	void resetFollowTarget();

	// Make camera move into a position where it can look at the MachActor.
	// Note that the camera can automatically switch between ground,
	// zenith and 3rd person depending upon the actor type.
	void lookAt( const MachActor& );

	// Make camera move into a position where the centre of the screen is the
	// passed in point.
	void lookAt( const MexPoint2d& );

	// Called when mouse is at edge of screen.
	enum ScrollDir { LEFT, RIGHT, UP, DOWN };
	void scroll( ScrollDir, const GuiMouseEvent& event );

    void scrollWithWheel(const Gui::ScrollState wheelDir, const double step);

	bool isZenithCameraActive() const;
	bool isGroundCameraActive() const;
	bool is1stPersonCameraActive() const;

	void freezeMotion();
	void resumeMotion();
	bool motionFrozen() const;

	MATHEX_SCALAR zenithMinimumHeight() const;
	MATHEX_SCALAR zenithMaximumHeight() const;
	MATHEX_SCALAR zenithMinimumEarHeight() const;
	MATHEX_SCALAR zenithMaximumEarHeight() const;

	// Switch to first person camera. Use currentCamera to get at the 1st person camera object.
	void use1stPersonCamera();

	// Switch to ground camera. Use currentCamera to get at the ground camera object.
	void useGroundCamera();

	// Switch to zenith camera. Use currentCamera to get at the zenith camera object.
	void useZenithCamera();

	// Load/Save camera info
	void saveGame( PerOstream& outStream );
	void loadSavedGame( PerIstream& inStream );

	Gui::Coord positionOnTerrainThatZenithCameraIsLookingAt() const;
	// PRE( isZenithCameraActive() );

	// Configure pitch up/down keys for ground camera
	void reversePitchUpDownKeys( bool );

	bool beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int clientData);
	void domainDeleted( W4dDomain* pDomain );

private:
	// Operations revoked
    MachCameras( const MachCameras& );
    MachCameras& operator =( const MachCameras& );
    bool operator ==( const MachCameras& );

    friend ostream& operator <<( ostream& o, const MachCameras& t );

	// Keeps currentCamera_ consistent with the W4dSceneManager's version.
	void useCamera(MachLogCamera*);

	// Reduce fog for zenith view to enable planet surface to been seen from high up.
	void reduceFog();

	// Restore fog to default setting when switching from zenith view.
	void restoreFog();

	void internalLookAt( const MachActor& );

	// Return the position that the zenith camera would have to be in in order for
	// "lookAt" to be the centre of the sceen.
	MexPoint3d zenithLookAt( const MexPoint3d& lookAt );

	void switchToZenith( const MexPoint3d& lookAt );

	enum Command { 	ZENITHVIEW, GROUNDVIEW, FIRSTPERSONVIEW,
					SAVEVIEW1, SAVEVIEW2, SAVEVIEW3, SAVEVIEW4,
					RESTOREVIEW1, RESTOREVIEW2, RESTOREVIEW3, RESTOREVIEW4,
					RESTOREDEFAULTCAMERAPOS, RESTORELASTCAMERAPOS };

	// Nested structure for storing camera positions.
	struct CameraSave
	{
		enum Saved { NOTHINGSAVED, ZENITHVIEW, GROUNDVIEW, FIRSTPERSONVIEW };

		CameraSave()
		: saved_( NOTHINGSAVED )
		{}

		Saved saved_;
		MexTransform3d position_;
	};

	// Update CameraSave with the position etc of the currently selected camera.
	void saveCamera( CameraSave* );

	// Switch to camera and position as described in CameraSave.
	void restoreCamera( const CameraSave& );

	// Data members...
	DevKeyToCommandTranslator* 				pKeyTranslator_;
	PhysGroundFlyControl* 					pGroundControl_;
	PhysFlyControl* 	 					pFreeControl_;
	PhysFlyControl* 	 					pFirstPersonControl_;
    PhysZenithFlyControl* 					pZenithControl_;
    MachLogZenithCameraMotionConstraint* 	pZenithConstraint_;
	MachLogGroundCameraMotionConstraint*	pGroundConstraint_;
 	MachLogCamera*							pFirstPersonCamera_;
 	MachLogCamera*							pFreeCamera_;
 	MachLogCamera*							pGroundCamera_;
 	MachLogCamera*							pZenithCamera_;
 	MachLogCamera*							pCurrentCamera_;
	MachActor*								pFollowTarget_ = nullptr;
	W4dSceneManager*						pSceneManager_;
	W4dRoot*								pRoot_;
	CameraSave								save1_;
	CameraSave								save2_;
	CameraSave								save3_;
	CameraSave								save4_;
	CameraSave								startCameraPos_;
	CameraSave								lastCameraPos_;
	CameraSave								lastZenithPos_;
	DevTimer								lastThirdPersonChange_;
	MATHEX_SCALAR							zenithMinimumHeight_;
	MATHEX_SCALAR							zenithMaximumHeight_;
	MATHEX_SCALAR							zenithMinimumEarHeight_;
	MATHEX_SCALAR							zenithMaximumEarHeight_;
	bool									cameraMoved_;
	bool									groundCameraMoved_;
};


#endif

/* End CAMERAS.HPP **************************************************/
