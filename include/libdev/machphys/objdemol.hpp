/*
 * O B J D E M O L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysObjDemolish

    A brief description of the class should go in here
*/

#ifndef _OBJDEMOL_HPP
#define _OBJDEMOL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"

#include "machphys/machphys.hpp"

class MachPhysObjDemolish : public W4dComposite
// Canonical form revoked
{
public:

    MachPhysObjDemolish( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::DemolitionType type );

	void startDemolition( const PhysAbsoluteTime& startTime, const PhysRelativeTime& demolitionDuration, MATHEX_SCALAR ObjectToGenericRatio, MATHEX_SCALAR objectExpansion );

    ~MachPhysObjDemolish( void );

    static const MachPhysObjDemolish& exemplar( MachPhys::DemolitionType type );

	static bool useGenericDemolition( MachPhys::DemolitionType type );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysObjDemolish );
    PER_FRIEND_READ_WRITE( MachPhysObjDemolish );

private:
    friend ostream& operator <<( ostream& o, const MachPhysObjDemolish& t );
    //One-time constructor used to create the exemplar
    MachPhysObjDemolish( MachPhys::DemolitionType type );
    MachPhysObjDemolish( const MachPhysObjDemolish& copyMe, W4dEntity* pParent, const MexTransform3d& localTransform );
	// operations revoked
    MachPhysObjDemolish( const MachPhysObjDemolish& );
    MachPhysObjDemolish& operator =( const MachPhysObjDemolish& );

	SysPathName compositeFileName( MachPhys::DemolitionType type );

    friend class MachPhysWeaponPersistence;

	// Is the demolition generic or dedicated
	bool isGeneric_;
};

PER_DECLARE_PERSISTENT( MachPhysObjDemolish );

#endif

/* End OBJDEMOLISH.HPP ************************************************/
