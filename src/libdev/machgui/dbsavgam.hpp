/*
 * D B S A V G A M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbSavedGame

    Represents a saved game, storing user and external path, player and scenario etc.
*/

#ifndef _MACHGUI_DBSAVGAM_HPP
#define _MACHGUI_DBSAVGAM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"

//forward refs
class MachGuiDbScenario;
class MachGuiDbPlayer;
class MachGuiDbISavedGame;

class MachGuiDbSavedGame
// Canonical form revoked
{
public:
    //Create with menu name userFileName, and external path fileName for the specfiied scenario
    MachGuiDbSavedGame( const string& userFileName, const string& fileName, MachGuiDbScenario* pDbScenario );

    ~MachGuiDbSavedGame();

    //Set/get the user filename
    const string& userFileName() const;
    void userFileName( const string& name );

    //Set/get the external file name
    const string& externalFileName() const;
    void externalFileName( const string& name );

    //true iff a player is associated with the saved game
    bool hasPlayer() const;

    //Set/get the player associated with the saved game
    MachGuiDbPlayer& player() const;
    //PRE( hasPlayer() );
    void player( MachGuiDbPlayer* pDbPlayer ); //Use NULL to clear

    //Set/get flag indicating whether the saved game is for a campaign scenario
    bool isCampaignGame() const;
    void isCampaignGame( bool isIt );

    //Set/get the scenario used to start the game
    MachGuiDbScenario& scenario() const;
    void scenario( MachGuiDbScenario* pDbScenario );
    //PRE( pDbScenario != NULL );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachGuiDbSavedGame );
    PER_FRIEND_READ_WRITE( MachGuiDbSavedGame );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbSavedGame& t );

    MachGuiDbSavedGame( const MachGuiDbSavedGame& );
    MachGuiDbSavedGame& operator =( const MachGuiDbSavedGame& );

    //data members
    MachGuiDbISavedGame* pData_; //data implementation object
};

PER_DECLARE_PERSISTENT( MachGuiDbSavedGame );

#endif

/* End DBSAVGAM.HPP *************************************************/
