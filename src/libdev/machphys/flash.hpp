/*
 * F L A S H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysFlashDisc

	defines a white flash when a MachPhysElectroChager is fired
*/

#ifndef _MACHPHYS_FLASH_HPP
#define _MACHPHYS_FLASH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysFlashDisc : public W4dEntity
// Canonical form revoked
{
public:
	enum  ColourType : unsigned char {WHITE, YELLOW};
    //ctor
    MachPhysFlashDisc( W4dEntity* pParent, const MexTransform3d& localTransform, const MATHEX_SCALAR& size, ColourType colour);

    //dtor
    virtual ~MachPhysFlashDisc();

	static const MachPhysFlashDisc& exemplar( const MATHEX_SCALAR& size, ColourType colour );

	PhysRelativeTime flash(const PhysAbsoluteTime& startTime, ColourType colour);
	virtual bool intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysFlashDisc& t );

    PER_MEMBER_PERSISTENT( MachPhysFlashDisc );
private:
    //Deliberately revoked
    MachPhysFlashDisc( const MachPhysFlashDisc& );
    MachPhysFlashDisc& operator =( const MachPhysFlashDisc& );
    bool operator ==( const MachPhysFlashDisc& );

	friend class MachPhysOtherPersistence;
	//one time constructor
	MachPhysFlashDisc( const MATHEX_SCALAR& size, ColourType colour );

};

PER_DECLARE_PERSISTENT( MachPhysFlashDisc );
PER_READ_WRITE( MachPhysFlashDisc );
PER_ENUM_PERSISTENT( MachPhysFlashDisc::ColourType );

#endif

/* End FLASH.HPP *************************************************/
