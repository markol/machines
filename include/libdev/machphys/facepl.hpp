/*
 * P R O T G E N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysFacePlate

    This IS-A W4dGeneric which is created purely for the
    purpose of being copied or having its meshes copied.
    Because of its single argument constructor it can be
    used in a factory.
*/

#ifndef _MACHPHYS_FACEPLATE_HPP
#define _MACHPHYS_FACEPLATE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "system/pathname.hpp"

#include "world4d/generic.hpp"

class MachPhysFacePlate : public W4dGeneric
{
public:
    MachPhysFacePlate( W4dEntity* pParent, size_t level );
    ~MachPhysFacePlate( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysFacePlate );
    
private:
    // Operation deliberately revoked
    MachPhysFacePlate( const MachPhysFacePlate& );

    // Operation deliberately revoked
    MachPhysFacePlate& operator =( const MachPhysFacePlate& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysFacePlate& );

    SysPathName compositeFileName( size_t brainLevel ) const;
};

PER_READ_WRITE( MachPhysFacePlate );
PER_DECLARE_PERSISTENT( MachPhysFacePlate );

#endif

/* End PROTGEN.HPP **************************************************/
