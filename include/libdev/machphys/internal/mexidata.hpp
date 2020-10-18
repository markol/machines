/*
 * M E X I D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysIMachineExplosionData

    A brief description of the class should go in here
*/

#ifndef  _MACHPHYS_MEXIDATA_HPP
#define  _MACHPHYS_MEXIDATA_HPP

#include "ctl/list.hpp"
#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/point3d.hpp"
#include "phys/phys.hpp"
#include "machphys/mexpdata.hpp"

class W4dLink;
class W4dComposite;

class MachPhysIMachineExplosionData
// Canonical form revoked
{
public:
    MachPhysIMachineExplosionData( void );
    MachPhysIMachineExplosionData( const MachPhysIMachineExplosionData& );
    ~MachPhysIMachineExplosionData( void );

    void CLASS_INVARIANT;

    //  Return the maximum size of link to be exploded
    MATHEX_SCALAR   maxSize( void ) const;

    //  The min number of links to shoot off
    size_t  minToShootOff( void ) const;
    void    minToShootOff( size_t newMin );

    //  The maximum number of links to shoot off
    size_t  maxToShootOff( void ) const;
    void    maxToShootOff( size_t newMax );

    //  The explosion center from wich all the parts are sent in the air
    const MexPoint3d&  explosionCenter( void ) const;
    void  explosionCenter( const MexPoint3d& newExplosionCenter );

    PER_MEMBER_PERSISTENT( MachPhysIMachineExplosionData );
    PER_FRIEND_READ_WRITE( MachPhysIMachineExplosionData );

private:
	friend class MachPhysMachineExplosionData;
    friend ostream& operator <<( ostream& o, const MachPhysIMachineExplosionData& t );

    MachPhysIMachineExplosionData& operator =( const MachPhysIMachineExplosionData& );

    MachPhysMachineExplosionData::LinkDatas   links_;
    
    MATHEX_SCALAR   maxSize_;
    size_t          minToShootOff_;
	size_t      	maxToShootOff_;
	MexPoint3d  	explosionCenter_;
};

#ifdef _INLINE
    #include "machphys/internal/mexidata.ipp"
#endif


PER_DECLARE_PERSISTENT( MachPhysIMachineExplosionData );

#endif

/* End MEXIDATA.HPP *************************************************/
