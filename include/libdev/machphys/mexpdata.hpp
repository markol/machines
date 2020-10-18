/*
 * M E X P D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMachineExplosionData

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_MEXPDATA_HPP
#define _MACHPHYS_MEXPDATA_HPP


#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

template< class T > class ctl_list;
class W4dComposite;
class W4dLink;
class MachPhysIMachineExplosionData;
class MexPoint3d;

class MachPhysMachineExplosionData
{
public:
    MachPhysMachineExplosionData( void );

    //  Copy the data "as is"
    MachPhysMachineExplosionData( const MachPhysMachineExplosionData& copyMe );
    
    //  Copy the data "as is"
    MachPhysMachineExplosionData& operator =( const MachPhysMachineExplosionData& copyMe );

    ~MachPhysMachineExplosionData( void );

    //  Return the maximum size of link to be exploded
    MATHEX_SCALAR   maxSize( void ) const;

    enum { ALL_LINKS = -1 };
    
    //  The maximum number of links to shoot off
    size_t  maxToShootOff( void ) const;
    void    maxToShootOff( size_t newMax );
            
    //  The minimum number of links to shoot off
    size_t  minToShootOff( void ) const;
    void    minToShootOff( size_t newMin );

    //  The explosion center from wich all the parts are sent in the air
    const MexPoint3d&  explosionCenter( void ) const;
    void  explosionCenter( const MexPoint3d& newExplosionCenter );

    #include "machphys/linkdata.hpp"

    void addData( const LinkData& data );
        
    typedef ctl_list< LinkData >  LinkDatas;
    
    const LinkDatas& links( void ) const;
    
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachPhysMachineExplosionData );
    PER_FRIEND_READ_WRITE( MachPhysMachineExplosionData );

private:

    // Operation deliberately revoked
    bool operator ==( const MachPhysMachineExplosionData& );

    MachPhysIMachineExplosionData* pImpl_;
};

typedef CtlCountedPtr< MachPhysMachineExplosionData > MachPhysMachineExplosionDataPtr;

PER_DECLARE_PERSISTENT( MachPhysMachineExplosionData );

#endif

/* End MEXPDATA.HPP *************************************************/
