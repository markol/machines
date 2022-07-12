/*
 * L O A D I N G . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysResourceLoading

    the loading/dropping animation of resource carriers
*/

#ifndef _MACHPHYS_LOADING_HPP
#define _MACHPHYS_LOADING_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysResourceLoading : public W4dEntity
// Canonical form revoked
{
public:
	//public constructor
    MachPhysResourceLoading( W4dEntity* pParent, const MexTransform3d& localTransform );
    virtual ~MachPhysResourceLoading();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	//start the loading animation, returns the duration
	const PhysRelativeTime startLoading( const PhysAbsoluteTime& startTime, const MATHEX_SCALAR& size );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysResourceLoading );

private:
    friend ostream& operator <<( ostream& o, const MachPhysResourceLoading& t );

    MachPhysResourceLoading( const MachPhysResourceLoading& );
    MachPhysResourceLoading& operator =( const MachPhysResourceLoading& );

	friend class MachPhysOtherPersistence;

    MachPhysResourceLoading();
};

PER_DECLARE_PERSISTENT( MachPhysResourceLoading );
PER_READ_WRITE( MachPhysResourceLoading );

#endif

/* End LOADING.HPP *************************************************/
