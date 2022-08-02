/*
 * D B P L A N E T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbPlanet

    Represents a planet as presented in the Campaign menu, or a terrain type
    in the skirmish and multi-player setup menus.
*/

#ifndef _MACHGUI_DBPLANET_HPP
#define _MACHGUI_DBPLANET_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machgui/dbelemen.hpp"

//forward refs
class MachGuiDbScenario;
class MachGuiDbIPlanet;
class MachGuiDbSystem;

class MachGuiDbPlanet : public MachGuiDbElement
// Canonical form revoked
{
public:
    //The planet is known by identifier planetName which is used to construct the planet surface
    //filename.
    //The id of the string used in the menu list boxes is menuStringId.
    MachGuiDbPlanet( const string& planetName, uint menuStringId );

    virtual ~MachGuiDbPlanet();

    //set/get the flic or bmp filename displayed in the campaign menu.
    //string is empty if undefined.
    void campaignPicture( const string& filename );
    const string& campaignPicture() const;

    //set/get the system/terrain type of which the planet is a member
    void system( MachGuiDbSystem* pSystem );
    MachGuiDbSystem& system() const;

    //The number of scenarios for this planet
    uint nScenarios() const;

    //The index'th scenario
    MachGuiDbScenario& scenario( uint index );
    //PRE( index < nScenarios() );

    //Add another scenario to the planet
    void addScenario( MachGuiDbScenario* pScenario );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachGuiDbPlanet );
    PER_FRIEND_READ_WRITE( MachGuiDbPlanet );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbPlanet& t );

    MachGuiDbPlanet( const MachGuiDbPlanet& );
    MachGuiDbPlanet& operator =( const MachGuiDbPlanet& );

    //data members
    MachGuiDbIPlanet* pData_; //the implementation data object
};

PER_DECLARE_PERSISTENT( MachGuiDbPlanet );


#endif

/* End DBPLANET.HPP *************************************************/
