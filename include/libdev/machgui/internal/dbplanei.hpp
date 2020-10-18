/*
 * D B P L A N E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbIPlanet

    Data implementation object for MachGuiDbPlanet
*/

#ifndef _MACHGUI_DBPLANEI_HPP
#define _MACHGUI_DBPLANEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

//forward refs
class MachGuiDbScenario;
class MachGuiDbSystem;

class MachGuiDbIPlanet
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbIPlanet );
    PER_FRIEND_READ_WRITE( MachGuiDbIPlanet );

private:
    friend class MachGuiDbPlanet;

    friend ostream& operator <<( ostream& o, const MachGuiDbIPlanet& t );

    MachGuiDbIPlanet();
    ~MachGuiDbIPlanet();
    MachGuiDbIPlanet( const MachGuiDbIPlanet& );
    MachGuiDbIPlanet& operator =( const MachGuiDbIPlanet& );

    //data members
    MachGuiDbSystem* pSystem_; //Owning system/terrain type
    string campaignPicture_; //Flic or bmp displayed in campaign menu
    ctl_vector< MachGuiDbScenario* > scenarios_; //Collection of scenarios
};

PER_DECLARE_PERSISTENT( MachGuiDbIPlanet );


#endif

/* End DBPLANEI.HPP *************************************************/
