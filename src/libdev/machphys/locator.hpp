/*
 * L O C A T O R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLocator

    Models geolocator locating animation
*/

#ifndef _MACHPHYS_LOCATOR_HPP
#define _MACHPHYS_LOCATOR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysLocator: public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysLocator( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysLocator();

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLocator& exemplar();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startLocate( const PhysAbsoluteTime& startTime,
	                  const PhysRelativeTime& duration,
	                  const MATHEX_SCALAR& maxSize,
	                  uint nRepetations,
	                  uint animId );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLocator& t );

    PER_MEMBER_PERSISTENT( MachPhysLocator );
	PER_FRIEND_READ_WRITE( MachPhysLocator );

private:
    //Deliberately revoked
    MachPhysLocator( const MachPhysLocator& );
    MachPhysLocator& operator =( const MachPhysLocator& );
    bool operator ==( const MachPhysLocator& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLocator();

};

PER_DECLARE_PERSISTENT( MachPhysLocator );

#endif

/* End LOCATOR.HPP *************************************************/
