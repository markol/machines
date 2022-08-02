/*
 * D B P L Y S C N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbPlayerScenario

    Represents the track record of a single player at playing a specified campaign scenario.
*/

#ifndef _MACHGUI_DBPLYSCN_HPP
#define _MACHGUI_DBPLYSCN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/strfwd.hpp"
#include "machlog/produnit.hpp" //Fairly cheap to include - avoids clients defining ctl_vector having to

//forward ref
class MachGuiDbPlayer;
class MachGuiDbScenario;
class MachGuiDbIPlayerScenario;
template< class T > class ctl_vector;

class MachGuiDbPlayerScenario
// Canonical form revoked
{
public:
    MachGuiDbPlayerScenario( MachGuiDbPlayer* pDbPlayer, MachGuiDbScenario* pDbScenario );

    ~MachGuiDbPlayerScenario();

    //Accessors
    MachGuiDbScenario& scenario() const;
    MachGuiDbPlayer& player() const;
    bool hasWon() const;
    int lastScore() const;
    uint nAttempts() const;

    //Update for another attempt at the scenario. Player won it if didWin is true,
    //and his score is supplied.
    void upDate( bool didWin, int score );

    //Set the list of units for specified race that survive at the end of the scenario
    typedef ctl_vector< MachLogProductionUnit > Units;
    void raceSurvivingUnits( MachPhys::Race race, const Units& units );
    //PRE( hasWon() );

    //Export the list
    const Units& raceSurvivingUnits( MachPhys::Race race ) const;
    //PRE( hasWon() );

    //Add/remove the named flag to/from the list set for this player/scenario
    void setFlag( const string& flag, bool isSet );

    //true if the named flag has been set
    bool isFlagSet( const string& flag );

    //Clear all set flags
    void clearSetFlags();

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachGuiDbPlayerScenario );
    PER_FRIEND_READ_WRITE( MachGuiDbPlayerScenario );

private:
    friend class MachGuiDbPlayer;

    friend ostream& operator <<( ostream& o, const MachGuiDbPlayerScenario& t );

    MachGuiDbPlayerScenario( const MachGuiDbPlayerScenario& );
    MachGuiDbPlayerScenario& operator =( const MachGuiDbPlayerScenario& );

    //the sequence id for updates
    uint updateId() const;

    //data members
    MachGuiDbIPlayerScenario* pData_; //data implementation object
};

PER_DECLARE_PERSISTENT( MachGuiDbPlayerScenario );

#endif

/* End DBPLYSCN.HPP *************************************************/
