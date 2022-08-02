/*
 * D B P L Y S C I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbIPlayerScenario

    Data implementation object for MachGuiDbPlayerScenario
*/

#ifndef _MACHGUI_DBPLYSCI_HPP
#define _MACHGUI_DBPLYSCI_HPP

#include "stdlib/string.hpp"
#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"
#include "machlog/produnit.hpp"

//forward refs
class MachGuiDbScenario;
class MachGuiDbPlayer;

class MachGuiDbIPlayerScenario
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbIPlayerScenario );
    PER_FRIEND_READ_WRITE( MachGuiDbIPlayerScenario );

private:
    friend class MachGuiDbPlayerScenario;

    friend ostream& operator <<( ostream& o, const MachGuiDbIPlayerScenario& t );

    MachGuiDbIPlayerScenario();
    ~MachGuiDbIPlayerScenario();
    MachGuiDbIPlayerScenario( const MachGuiDbIPlayerScenario& );
    MachGuiDbIPlayerScenario& operator =( const MachGuiDbIPlayerScenario& );

    typedef ctl_vector< MachLogProductionUnit > Units;
    typedef ctl_vector< Units > RaceUnits;
    typedef ctl_vector< string > Strings;

    //data members
    MachGuiDbScenario* pDbScenario_; //The campaign scenario this relates to
    MachGuiDbPlayer* pDbPlayer_; //The player whose info is being stored
    bool isWon_; //True if the player has won this scenario ever
    int lastScore_; //Score attained on most recent attempt
    uint nAttempts_; //The number of attempts at playing the scenario
    uint lastUpdateId_; //Sequence number of last update. Used to indicate which scenarios
                        //tried last.
    RaceUnits raceUnits_; //Optional lists of machines surviving at the end of each scenario by race.
                          //Each machine is represented by a production unit
    Strings setFlags_;//A list of the named flags set by actions while playing the scenario
};

PER_DECLARE_PERSISTENT( MachGuiDbIPlayerScenario );

#endif

/* End DBPLYSCI.HPP *************************************************/
