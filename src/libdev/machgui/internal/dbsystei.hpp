/*
 * D B S Y S T E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbISystem

    Implementation data object for MachGuiDbSystem
*/

#ifndef _MACHGUI_DBSYSTEI_HPP
#define _MACHGUI_DBSYSTEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

//forward refs
class MachGuiDbPlanet;

class MachGuiDbISystem
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbISystem );
    PER_FRIEND_READ_WRITE( MachGuiDbISystem );

private:
    friend class MachGuiDbSystem;

    friend ostream& operator <<( ostream& o, const MachGuiDbISystem& t );

    MachGuiDbISystem();
    ~MachGuiDbISystem();
    MachGuiDbISystem( const MachGuiDbISystem& );
    MachGuiDbISystem& operator =( const MachGuiDbISystem& );

    //data members
    string campaignPicture_; //Flic or bmp displayed in campaign menu
    ctl_vector< MachGuiDbPlanet* > planets_; //Collection of planets in the system or size group
};

PER_DECLARE_PERSISTENT( MachGuiDbISystem );


#endif

/* End DBSYSTEI.HPP *************************************************/
