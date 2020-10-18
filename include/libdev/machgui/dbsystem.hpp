/*
 * D B S Y S T E M . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbSystem

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_DBSYSTEM_HPP
#define _MACHGUI_DBSYSTEM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machgui/dbelemen.hpp"


//forward refs
class MachGuiDbISystem;
class MachGuiDbPlanet;

class MachGuiDbSystem : public MachGuiDbElement
// Canonical form revoked
{
public:
    //The system is known by identifier systemName.
    //The id of the string used in the menu list boxes is menuStringId.
    MachGuiDbSystem( const string& systemName, uint menuStringId );

    virtual ~MachGuiDbSystem();

    //set/get the flic or bmp filename displayed in the campaign menu.
    //string is empty if undefined.
    void campaignPicture( const string& filename );
    const string& campaignPicture() const;

    //The number of planets in this system
    uint nPlanets() const;

    //The index'th scenario
    MachGuiDbPlanet& planet( uint index );
    //PRE( index < nPlanets() );

    //Add a planet to the system
    void addPlanet( MachGuiDbPlanet* pPlanet );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachGuiDbSystem );
    PER_FRIEND_READ_WRITE( MachGuiDbSystem );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbSystem& t );

    MachGuiDbSystem( const MachGuiDbSystem& );
    MachGuiDbSystem& operator =( const MachGuiDbSystem& );

    //data members
    MachGuiDbISystem* pData_; //implementation data object
};

PER_DECLARE_PERSISTENT( MachGuiDbSystem );


#endif

/* End DBSYSTEM.HPP *************************************************/
