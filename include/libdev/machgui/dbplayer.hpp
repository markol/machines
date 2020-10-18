/*
 * D B P L A Y E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbPlayer

    Represents a registered player
*/

#ifndef _MACHGUI_DBPLAYER_HPP
#define _MACHGUI_DBPLAYER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"

//forward refs
class MachGuiDbScenario;
class MachGuiDbPlayerScenario;
class MachGuiDbIPlayer;

class MachGuiDbPlayer
// Canonical form revoked
{
public:
    //Every player has a unique id and name
    MachGuiDbPlayer( uint id, const string& name );

    ~MachGuiDbPlayer();

    //Set/get id/name
    uint id() const;
    const string& name() const;
    void name( const string& newName );

    //true iff this player has played scenario. In this case returns the corresponding record
    //in ppPlayerScenario.
    bool hasPlayed( const MachGuiDbScenario& scenario, MachGuiDbPlayerScenario** ppPlayerScenario ) const;

    //Ensures a player scenario object has been added for pDbScenario, creating if necessary,
    //and returns it.
    MachGuiDbPlayerScenario& playerScenario( MachGuiDbScenario* pDbScenario );

    //the number of scenaris this player has played
    uint nPlayerScenarios() const;

    //The index'th player scenario
    MachGuiDbPlayerScenario& playerScenario( uint index ) const;
    //PRE( index < nPlayerScenarios() );

    MachGuiDbPlayerScenario& mostRecentPlayerScenario();
    //PRE( nPlayerScenarios() != 0 );

	// Last scenario to be selected on scenario selection screen
	void lastSelectedScenario( MachGuiDbScenario* );
	MachGuiDbScenario* lastSelectedScenario() const;					

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachGuiDbPlayer );
    PER_FRIEND_READ_WRITE( MachGuiDbPlayer );

private:
    friend class MachGuiDbPlayerScenario;
    friend class MachGuiDatabase;
    
    //The next id to use for player scenario update
    uint nextUpdateId();

    //Set/get the id indicating order of usage
    uint useSequenceId() const;
    void useSequenceId( uint id );

    friend ostream& operator <<( ostream& o, const MachGuiDbPlayer& t );

    MachGuiDbPlayer( const MachGuiDbPlayer& );
    MachGuiDbPlayer& operator =( const MachGuiDbPlayer& );

    //data members
    MachGuiDbIPlayer* pData_; //data implementation object
};

PER_DECLARE_PERSISTENT( MachGuiDbPlayer );

#endif

/* End DBPLAYER.HPP *************************************************/
