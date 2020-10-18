/*
 * D B S A V G A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbISavedGame

    Data implementation object for MachGuiDbSavedGame
*/

#ifndef _MACHGUI_DBSAVGAI_HPP
#define _MACHGUI_DBSAVGAI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"

//forward refs
class MachGuiDbPlayer;
class MachGuiDbScenario;

class MachGuiDbISavedGame
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbISavedGame );
    PER_FRIEND_READ_WRITE( MachGuiDbISavedGame );

private:
    friend class MachGuiDbSavedGame;

    friend ostream& operator <<( ostream& o, const MachGuiDbISavedGame& t );

    MachGuiDbISavedGame();
    ~MachGuiDbISavedGame();
    MachGuiDbISavedGame( const MachGuiDbISavedGame& );
    MachGuiDbISavedGame& operator =( const MachGuiDbISavedGame& );

    //data members
    string userFileName_; //The filename the user sees in the menus
    string fileName_; //The actual filename
    MachGuiDbPlayer* pDbPlayer_; //The current player when game was saved (may be NULL)
    bool isCampaign_; //True if a campaign scenario
    MachGuiDbScenario* pDbScenario_; //The original scenario
};

PER_DECLARE_PERSISTENT( MachGuiDbISavedGame );

#endif

/* End DBSAVGAI.HPP *************************************************/
