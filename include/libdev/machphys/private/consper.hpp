/*
 * C O N S P E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysConstructionPersistence

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_CONSPER_HPP
#define _MACHPHYS_CONSPER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class MachPhysConstructionPersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysConstructionPersistence& instance();
    ~MachPhysConstructionPersistence();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysConstructionPersistence& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysConstructionPersistence );
    
private:
    MachPhysConstructionPersistence( const MachPhysConstructionPersistence& );
    MachPhysConstructionPersistence& operator =( const MachPhysConstructionPersistence& );
    bool operator ==( const MachPhysConstructionPersistence& );

    MachPhysConstructionPersistence();
};

PER_READ_WRITE( MachPhysConstructionPersistence );
PER_DECLARE_PERSISTENT( MachPhysConstructionPersistence );

#endif

/* End CONSPER.HPP **************************************************/
