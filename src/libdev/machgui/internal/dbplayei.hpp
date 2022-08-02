/*
 * D B P L A Y E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbIPlayer

    Data implementation object for MachGuiDbPlayer
*/

#ifndef _MACHGUI_DBPLAYEI_HPP
#define _MACHGUI_DBPLAYEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

//forward refs
class MachGuiDbPlayerScenario;
class MachGuiDbScenario;

class MachGuiDbIPlayer
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbIPlayer );
    PER_FRIEND_READ_WRITE( MachGuiDbIPlayer );

private:
    friend class MachGuiDbPlayer;

    friend ostream& operator <<( ostream& o, const MachGuiDbIPlayer& t );

    MachGuiDbIPlayer();
    ~MachGuiDbIPlayer();
    MachGuiDbIPlayer( const MachGuiDbIPlayer& );
    MachGuiDbIPlayer& operator =( const MachGuiDbIPlayer& );

    typedef ctl_vector< MachGuiDbPlayerScenario* > PlayerScenarios;

    //data members
    uint id_; //The player's unique id
    string name_; //The name s/he's known by in the list boxes
    uint useSequenceId_; //Stores sequence number of lst use of the player.
                         //This is the field for time order sorting
    uint nextUpdateId_; //Next number to use in update sequence for the player scenarios
    PlayerScenarios playerScenarios_; //Collection of scenario's this player has attempted
	MachGuiDbScenario* pLastSelectedScenario_;	// Last scenario selected on scenario selection screen
};

PER_DECLARE_PERSISTENT( MachGuiDbIPlayer );


#endif

/* End DBPLAYEI.HPP *************************************************/
