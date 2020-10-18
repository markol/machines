/*
 * M A C H D A T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysIMachineData

    Internal data used by MachPhysMachine
*/

#ifndef _MACHPHYS_MACHDATI_HPP
#define _MACHPHYS_MACHDATI_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

class MachPhysIMachineData
// Canonical form revoked
{
public:
    MachPhysIMachineData( void );
    ~MachPhysIMachineData( void );

    MachPhys::LocomotionType    locomotionType() const;
    void locomotionType( MachPhys::LocomotionType );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysIMachineData& t );

    MachPhysIMachineData( const MachPhysIMachineData& );
    MachPhysIMachineData& operator =( const MachPhysIMachineData& );

    MachPhys::LocomotionType    locomotionType_;
};

#ifdef _INLINE
    #include "machphys/internal/machdati.ipp"
#endif


#endif

/* End MACHDATI.HPP *************************************************/
