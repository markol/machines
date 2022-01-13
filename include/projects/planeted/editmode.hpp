/*
 * E D I T M O D E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedEditorMode

    A brief description of the class should go in here
*/

#ifndef _PLANETED_EDITMODE_HPP
#define _PLANETED_EDITMODE_HPP

#include "base/base.hpp"
#include "device/butevent.hpp"
#include "mathex/line3d.hpp"
#include "ctl/vector.hpp"
#include <fstream>
#include "stdlib/string.hpp"

class W4dSceneManager;
class MachLogPlanet;
//class DevButtonEvent;
class SysPathName;
class MexPoint2d;
class PedScenarioFile;

class PedEditorMode
// Canonical form revoked
{
public:
    PedEditorMode( void );
    virtual ~PedEditorMode( void );

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& ) = 0;

	virtual void changingMode( void ) = 0;

	virtual void activateMode( void );

	virtual void preRenderUpdate( void ) = 0;

	virtual void displayKeyboardCtrls( void ) = 0;

	virtual void displayModeInfo( void ) = 0;

	virtual void readCspFile( const SysPathName& ) {};
	// PRE( fileName.existsAsFile() );

	virtual void writeCspFile( std::ofstream& ) {};

	virtual void readScnFile( PedScenarioFile& ) {};
	// PRE( fileName.existsAsFile() );

	virtual void writeScnFile( PedScenarioFile& ) {};

	virtual void readArfFile( const SysPathName& ) {};
	// PRE( cspFileName.existsAsFile() );

	virtual void initialise( W4dSceneManager*, MachLogPlanet* );
	// PRE( pPlanet != NULL );
	// PRE( pSceneManager != NULL );

	virtual void validate();

	virtual void displayWarningMsgs();

protected:
	typedef ctl_vector< string > WarningMsgs;

	// Data...
	MachLogPlanet* pPlanet_;
	W4dSceneManager* pSceneManager_;
	bool active_;
	WarningMsgs warnings_;

	MexLine3d cameraThroughCursorLine( const MexPoint2d& cursorScreenPoint );
	// PRE( pSceneManager_ != NULL );

private:
	// Operations deliberately revoked
    PedEditorMode( const PedEditorMode& );
    PedEditorMode& operator =( const PedEditorMode& );
    bool operator ==( const PedEditorMode& );

    friend ostream& operator <<( ostream& o, const PedEditorMode& t );
};


#endif

/* End EDITMODE.HPP *************************************************/
