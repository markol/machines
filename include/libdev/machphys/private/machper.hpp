/*
 * M A C H P E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMachinePersistence

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_MACHPER_HPP
#define _MACHPHYS_MACHPER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class MachPhysMachinePersistence
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysMachinePersistence& instance( void );
    ~MachPhysMachinePersistence( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachinePersistence& t );

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysMachinePersistence );
    
private:
    MachPhysMachinePersistence( const MachPhysMachinePersistence& );
    MachPhysMachinePersistence& operator =( const MachPhysMachinePersistence& );
    bool operator ==( const MachPhysMachinePersistence& );

    MachPhysMachinePersistence( void );
};

PER_READ_WRITE( MachPhysMachinePersistence );
PER_DECLARE_PERSISTENT( MachPhysMachinePersistence );

#endif

/* End MACHPER.HPP **************************************************/
