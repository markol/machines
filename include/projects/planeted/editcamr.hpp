/*
 * E D I T C A M R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedCameraEditor

    Camera editor mode. Used for setting the initial camera positions for each race.
*/

#ifndef _PLANETED_EDITCAMR_HPP
#define _PLANETED_EDITCAMR_HPP

#include "base/base.hpp"
#include "device/butevent.hpp"
#include "render/colour.hpp"
#include "machphys/machphys.hpp"
#include "planeted/editmode.hpp"


//class DevButtonEvent;
class SysPathName;
class ofstream;
class PedCameraMarker;

class PedCameraEditor : public PedEditorMode
// Canonical form revoked
{
public:
    PedCameraEditor( void );
    virtual ~PedCameraEditor( void );

    void CLASS_INVARIANT;

	virtual void initialise( W4dSceneManager*, MachLogPlanet* );
	// PRE( pPlanet != NULL );
	// PRE( pSceneManager != NULL );

	virtual void processInput( const DevButtonEvent& );
	
	virtual void changingMode( void );
	
	virtual void preRenderUpdate( void );
	
	virtual void displayKeyboardCtrls( void );
	
	virtual void displayModeInfo( void );

	virtual void readScnFile( PedScenarioFile& );
	// PRE( cspFileName.existsAsFile() );

	virtual void writeScnFile( PedScenarioFile& );

	virtual void activateMode( void );


private:
	// Operations deliberately revoked
    PedCameraEditor( const PedCameraEditor& );
    PedCameraEditor& operator =( const PedCameraEditor& );
    bool operator ==( const PedCameraEditor& );

    friend ostream& operator <<( ostream& o, const PedCameraEditor& t );

	void processDrop( void );

	PedCameraMarker* processChangeRace( PedCameraMarker* );

	void setCameraToMarker( const PedCameraMarker& ); 

	void setMarkerToCamera( PedCameraMarker**, MachPhys::Race );

	PedCameraMarker* setMarkerForRace( MachPhys::Race );

	// Data
	
	PedCameraMarker* pRedMarker_;
	PedCameraMarker* pBlueMarker_;
	PedCameraMarker* pGreenMarker_;
	PedCameraMarker* pYellowMarker_;
	PedCameraMarker* pCurrentMarker_;
	MachPhys::Race 	race_;
};


#endif

/* End EDITCAMR.HPP *************************************************/
