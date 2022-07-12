/*
 * D B S C E N A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbIScenario

    Data object class for MachGuiDbScenario
*/

#ifndef _MACHGUI_DBSCENAI_HPP
#define _MACHGUI_DBSCENAI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/nbvector.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MachGuiDbPlanet;

class MachGuiDbIScenario
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbIScenario );
    PER_FRIEND_READ_WRITE( MachGuiDbIScenario );

private:
    friend class MachGuiDbScenario;

    friend ostream& operator <<( ostream& o, const MachGuiDbIScenario& t );

    MachGuiDbIScenario();
    ~MachGuiDbIScenario();
    MachGuiDbIScenario( const MachGuiDbIScenario& );
    MachGuiDbIScenario& operator =( const MachGuiDbIScenario& );

    //data members
    MachGuiDbPlanet* pPlanet_; //The planet on which it occurs
    string planetFile_; //relative path of the planet files (psf, pob etc)
    string campaignPicture_; //Flic or bmp displayed in campaign menu
    string briefingPicture_; //Flic or bmp displayed in briefing menu
    string debriefingPicture_; //Flic or bmp displayed in debriefing menu (when you win)
	string debriefingLosePicture_; //Flic or bmp displayed in debriefing menu (when you lose)
    string entryFlic_; //Fullscreen flic displayed before strating the scenario
    string winFlic_; //Fullscreen flic displayed upon winning the scenario
    string loseFlic_; //Fullscreen flic displayed upon losing the scenario
	uint   maxPlayers_; // Maximun number of players
    uint musicTrack_; //CD music Track to be played
    ctl_nb_vector< MachPhys::Race > saveMachineRaces_; //List of races needing surviving machines to be saved
	bool hasBeenWon_; // Indicates if any player has won this scenario
};

PER_DECLARE_PERSISTENT( MachGuiDbIScenario );


#endif

/* End DBSCENAI.HPP *************************************************/
