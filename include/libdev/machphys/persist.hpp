/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPersistence

    The controlling class for the MachPhys persistence
    mechanism. Allow everything in MachPhys to be persisted
    or allow different bits to be persisted independently.
*/

#ifndef _MACHPHYS_PERSIST_HPP
#define _MACHPHYS_PERSIST_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class MachPhysMachinePersistence;
class MachPhysConstructionPersistence;
class MachPhysWeaponPersistence;
class MachPhysOtherPersistence;

class MachPhysPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysPersistence& instance( void );
    ~MachPhysPersistence( void );

    //  Allow the machines to be persisted independently of everything else
    const MachPhysMachinePersistence& machines() const;
    MachPhysMachinePersistence& machines();
    
    //  Allow the constructions to be persisted independently of everything else
    const MachPhysConstructionPersistence& constructions() const;
    MachPhysConstructionPersistence& constructions();
    
    //  Allow the weapons to be persisted independently of everything else
    const MachPhysWeaponPersistence& weapons() const;
    MachPhysWeaponPersistence& weapons();
    
    //  Allow the others to be persisted independently of everything else
    const MachPhysOtherPersistence& others() const;
    MachPhysOtherPersistence& others();
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPersistence& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysPersistence );
    
private:
    MachPhysPersistence( const MachPhysPersistence& );
    MachPhysPersistence& operator =( const MachPhysPersistence& );
    bool operator ==( const MachPhysPersistence& );

    MachPhysPersistence( void );
};

PER_READ_WRITE( MachPhysPersistence );
PER_DECLARE_PERSISTENT( MachPhysPersistence );

#endif

/* End PERSIST.HPP **************************************************/
