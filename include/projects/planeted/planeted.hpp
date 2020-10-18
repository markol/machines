/*
 * P L A N E T E D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedPlanetEditor

    A brief description of the class should go in here
*/

#ifndef _PLANETED_PLANETED_HPP
#define _PLANETED_PLANETED_HPP

#include "base/base.hpp"

#include "system/pathname.hpp"
#include "planeted/scenfile.hpp"

class W4dSceneManager;
class MachLogPlanet;
class DevButtonEvent;
class PedEditorMode;

class PedPlanetEditor
// Canonical form revoked
{
public:
    //  Singleton class
    static PedPlanetEditor& instance( void );
  
    void CLASS_INVARIANT;

	void initialise( W4dSceneManager*, MachLogPlanet* );
	// PRE( pPlanet_ != NULL );
	// PRE( pSceneManager_ != NULL );

	void processInput( const DevButtonEvent& );

	void preRenderUpdate( void );

	void readCspFile( const SysPathName& );
	void readScnFile( const SysPathName& );
	void readArfFile( const SysPathName& );

private:
	// Private as class is Singleton
    PedPlanetEditor( void );
	~PedPlanetEditor( void );

	// Operations deliberately revoked
    PedPlanetEditor( const PedPlanetEditor& );
    PedPlanetEditor& operator =( const PedPlanetEditor& );
    bool operator ==( const PedPlanetEditor& );

    friend ostream& operator <<( ostream& o, const PedPlanetEditor& t );

	void initDeviceEvents( void );
	void processSave( void );
	void displayKeyboardCtrls( void );
	void displaySavedStatus( void );
	// void writeScnFile( ofstream& );

	// Data...
	PedEditorMode* pTileMode_;
	PedEditorMode* pObstacleMode_;
	PedEditorMode* pDomainMode_;
	PedEditorMode* pPortalMode_;
	PedEditorMode* pConstructionMode_;
	PedEditorMode* pMachineMode_;
	PedEditorMode* pArtefactMode_;
	PedEditorMode* pCameraMode_;

	PedEditorMode* pCurrentMode_;
	MachLogPlanet* pPlanet_;
	W4dSceneManager* pSceneManager_;
	bool saved_;
	bool saveSuccessful_;
	bool dispKeyboardControls_;
	bool dispWarnings_;
	SysPathName cspFileName_;
	SysPathName scnFileName_;
	SysPathName arfFileName_;
	PedScenarioFile scenarioFile_;
};


#endif

/* End PLANETED.HPP *************************************************/
