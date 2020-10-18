/*
 * D A T A B A S I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiIDatabase

    Data implementation object for MachGuiDatabase
*/

#ifndef _MACHGUI_DATABASI_HPP
#define _MACHGUI_DATABASI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include <functional>
#include "ctl/vector.hpp"
#include "ctl/map.hpp"

//forward refs
class MachGuiDbSystem;
class MachGuiDbElement;
class MachGuiDbPlayer;
class MachGuiDbSavedGame;
class MachGuiDatabaseHandler;
class MachGuiDbScenario;

class MachGuiIDatabase
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiIDatabase );
    PER_FRIEND_READ_WRITE( MachGuiIDatabase );

private:
    friend class MachGuiDatabase;
    friend ostream& operator <<( ostream& o, const MachGuiIDatabase& t );

    MachGuiIDatabase();
    ~MachGuiIDatabase();

    MachGuiIDatabase( const MachGuiIDatabase& );
    MachGuiIDatabase& operator =( const MachGuiIDatabase& );

    //Used for sorting players into most recent use order
    typedef MachGuiDbPlayer* MachGuiDbPlayer_ptr;
    class PlayerNameComparator
    : public std::binary_function< MachGuiDbPlayer_ptr, MachGuiDbPlayer_ptr, bool >
    {
    public:
	    bool operator ()( const MachGuiDbPlayer_ptr & pLhs, const MachGuiDbPlayer_ptr & pRhs ) const;
    };

    typedef ctl_map< string, MachGuiDbElement* , std::less< string > > ElementMap;
    typedef ctl_vector< MachGuiDbPlayer* > Players;
    typedef ctl_vector< MachGuiDbSystem* > Systems;
    typedef ctl_vector< MachGuiDbSavedGame* > SavedGames;

    //data members
    Systems campaignSystems_; //the systems available from the campaign menu
    ctl_vector< MachGuiDbSystem* > skirmishSystems_; //the small, medium, large systems for skirmishes
    ctl_vector< MachGuiDbSystem* > multiPlayerSystems_; //the small, medium, large systems for multi-player
    Players players_; //the registered players
    SavedGames savedGames_; //The games saved in the load menu
    uint nextPlayerId_;
    uint nextUseSequenceId_; //generates ids for order of use of players
    ElementMap* pElementMap_;
    MachGuiDbPlayer* pDbCurrentPlayer_;
    MachGuiDatabaseHandler* pDbHandler_; //used to interface to lower libs
    MachGuiDbScenario* pCurrentDbScenario_; //Current scenario being played
};

PER_DECLARE_PERSISTENT( MachGuiIDatabase );

#endif

/* End DATABASI.HPP *************************************************/
