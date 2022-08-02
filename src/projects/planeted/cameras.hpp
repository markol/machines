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
#include "device/butevent.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "device/timer.hpp"
#include "machlog/camera.hpp"

#include "gui/gui.hpp"

//class DevButtonEvent;
class DevKeyToCommandTranslator;
class PhysFlyControl;
class PhysZenithFlyControl;
class PhysGroundFlyControl;
class MachLogCamera;
class MachLogMachine;
class MachLogMachineThirdPerson;
class MachActor;
class W4dSceneManager;
class W4dRoot;
class GuiMouseEvent;

class MachCameras
// Canonical form revoked
{
public:
    //  Singleton class
    static MachCameras& instance();

	void initialise( W4dSceneManager*, W4dRoot* );

    void CLASS_INVARIANT;

	// Called every time there is a buttonEvent. Returns true if the buttonEvent was
	// processed (e.g. buttonEvent caused camera rotation).
	bool processButtonEvent( const DevButtonEvent& buttonEvent );

	// Called every frame before rendering to ensure that cameras are in the correct
	// domain etc.
	void updateCameras();

	MachLogCamera* currentCamera();
	// PRE( pCurrentCamera_ != NULL );

	MachLogCamera::Type currentCameraType();
	// PRE( pCurrentCamera_ != NULL );

	// Make camera move to new position ( assuming camera can move without
	// ending up inside an obstacle etc ).
	void moveTo( const MexPoint2d& );

	// Make camera move into a position where it can look at the MachActor.
	void lookAt( const MachActor& );

	// Change camera to appropriate view
	void switchToZenithView();

	void switchToSuperHighZenithView();

	void switchToGroundView();

	void switchToThirdPersonView();

	void switchToFirstPersonView();

	// Called when mouse is at edge of screen.
	enum ScrollDir { LEFT, RIGHT, UP, DOWN };
	void scroll( ScrollDir, const GuiMouseEvent& event );

    // FIXME: Not impl for planeted
    void scrollWithWheel(const Gui::ScrollState wheelDir, const double step) { /* ... */ }

private:
	// Private as class is Singleton
    MachCameras();
	// PRE( pSceneManager );
	// PRE( pRoot );

    ~MachCameras();

	// Operations revoked
    MachCameras( const MachCameras& );
    MachCameras& operator =( const MachCameras& );
    bool operator ==( const MachCameras& );

    friend ostream& operator <<( ostream& o, const MachCameras& t );

	// Change camera to look at the pNewMachine.
	void switchThirdPerson( MachLogMachine* pNewMachine );
	MachLogMachine& currentMachine();

	// Keeps currentCamera_ consistent with the W4dSceneManager's version.
	void useCamera(MachLogCamera*);

	// Updates 3rd person camera to be in same domain as machine being followed.
	void check3rdCameraDomain();

	// Reduce fog for zenith view to enable planet surface to been seen from high up.
	void reduceFog();

	// Restore fog to default setting when switching from zenith view.
	void restoreFog();

	// Return the position that the zenith camera would have to be in in order for
	// "lookAt" to be the centre of the sceen.
	MexPoint3d zenithLookAt( const MexPoint3d& lookAt );

	void switchToZenith( const MexPoint3d& lookAt );
	void switchToSuperHighZenith( const MexPoint3d& lookAt );

	enum Command { 	ZENITHVIEW, SUPERHIGHZENITHVIEW, GROUNDVIEW, FIRSTPERSONVIEW, THIRDPERSONVIEW,
					THIRDPERSONZOOMOUT, THIRDPERSONZOOMIN,
					SAVEVIEW1, SAVEVIEW2, SAVEVIEW3, SAVEVIEW4,
					RESTOREVIEW1, RESTOREVIEW2, RESTOREVIEW3, RESTOREVIEW4 };

	// Nested structure for storing camera positions.
	struct CameraSave
	{
		enum Saved { NOTHINGSAVED, ZENITHVIEW, SUPERHIGHZENITHVIEW, GROUNDVIEW, FIRSTPERSONVIEW, THIRDPERSONVIEW };

		CameraSave()
		: saved_( NOTHINGSAVED ),
		  pMachine_( NULL )
		{}

		Saved saved_;
		MexTransform3d position_;
		const MachLogMachine* pMachine_;
	};

	// Update CameraSave with the position etc of the currently selected camera.
	void saveCamera( CameraSave* );

	// Switch to camera and position as described in CameraSave.
	void restoreCamera( const CameraSave& );

	// Data members...
	DevKeyToCommandTranslator* pKeyTranslator_;
	PhysGroundFlyControl* 	pGroundControl_;
	PhysFlyControl* 	 	pFreeControl_;
	PhysFlyControl* 	 	pEyeControl_;
    PhysZenithFlyControl* 	pZenithControl_;
    PhysZenithFlyControl* 	pSuperHighZenithControl_;
 	MachLogCamera*			pEyeCamera_;
 	MachLogCamera*			pFreeCamera_;
 	MachLogCamera*			pGroundCamera_;
 	MachLogCamera*			pZenithCamera_;
 	MachLogCamera*			pSuperHighZenithCamera_;
 	MachLogCamera*			pCurrentCamera_;
	W4dSceneManager*		pSceneManager_;
	W4dRoot*				pRoot_;
	size_t 					machineIndex_;
	MachLogMachineThirdPerson* pThirdPerson_;
	CameraSave				save1_;
	CameraSave				save2_;
	CameraSave				save3_;
	CameraSave				save4_;
	DevTimer				lastThirdPersonChange_;
};


#endif

/* End CAMERAS.HPP **************************************************/
