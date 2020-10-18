/*
 * P L A N S U R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlanetSurfaceImpl

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_PLANSURI_HPP
#define _MACHPHYS_PLANSURI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"

class W4dRoot;

class MachPhysPlanetSurfaceImpl
// Canonical form revoked
{
public:
    MachPhysPlanetSurfaceImpl( void );
    ~MachPhysPlanetSurfaceImpl( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysPlanetSurfaceImpl );
    PER_FRIEND_READ_WRITE( MachPhysPlanetSurfaceImpl );

private:
    friend ostream& operator <<( ostream& o, const MachPhysPlanetSurfaceImpl& t );

    MachPhysPlanetSurfaceImpl( const MachPhysPlanetSurfaceImpl& );
    MachPhysPlanetSurfaceImpl& operator =( const MachPhysPlanetSurfaceImpl& );

    friend class MachPhysPlanetSurface;
    
    //  The minumum height that any machine should have on the terrain
    MATHEX_SCALAR minMachineHeight_;
    W4dRoot* pPersistenceRoot_; //A temporary root used to keep the surface self-contained for persistence
};

PER_DECLARE_PERSISTENT( MachPhysPlanetSurfaceImpl );

#endif

/* End PLANSURI.HPP *************************************************/
