/*
 * D B L F L A M E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysDoublesidedFlame

    A double-sided non TTF flame
*/

#ifndef _MACHPHYS_DBLFLAME_HPP
#define _MACHPHYS_DBLFLAME_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"
#include "render/render.hpp"
#include "render/matvec.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

class MachPhysDoublesidedFlame : public W4dEntity
// Canonical form revoked
{
public:
    MachPhysDoublesidedFlame( W4dEntity* pParent, const MexTransform3d& localTransform);
    ~MachPhysDoublesidedFlame( void );

	static const MachPhysDoublesidedFlame& exemplar();


	void startFlame(const MATHEX_SCALAR& w, const MATHEX_SCALAR& h,
    					      const PhysAbsoluteTime& startTime,
    					      const PhysRelativeTime& duration );

    //Inherited from W4dEntity. Returns false.
	virtual	bool intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysDoublesidedFlame );
	PER_FRIEND_READ_WRITE( MachPhysDoublesidedFlame );

private:
    friend ostream& operator <<( ostream& o, const MachPhysDoublesidedFlame& t );

    MachPhysDoublesidedFlame( const MachPhysDoublesidedFlame& );
    MachPhysDoublesidedFlame& operator =( const MachPhysDoublesidedFlame& );

	friend class MachPhysOtherPersistence;

	//one time constructor
    MachPhysDoublesidedFlame();

    typedef ctl_vector< Ren::MaterialVecPtr > Materials;
    typedef CtlCountedPtr< Materials > MaterialsPtr;

    static  const MaterialsPtr&  materialsPtr();
	static void addMaterial( Materials* pMaterials, const RenTexture& texture );

	PhysAbsoluteTime endTime_;
};

PER_DECLARE_PERSISTENT( MachPhysDoublesidedFlame );

#endif

/* End DBLFLAME.HPP *************************************************/
