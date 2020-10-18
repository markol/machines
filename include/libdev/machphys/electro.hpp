/*
 * E L E C T R O . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysElectro

    Models the various types of electro weapon.
*/

#ifndef _MACHPHYS_ELECTRO_HPP
#define _MACHPHYS_ELECTRO_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/lineproj.hpp"

class MachPhysElectro : public MachPhysLinearProjectile
// Canonical form revoked
{
public:
	enum FlashType : unsigned int {VIRUS, ELECTRIC};
    //Constructs appropriate.  width X hight defines the size of the stf
    MachPhysElectro( W4dEntity* pParent, const MexTransform3d& localTransform, FlashType type);

    //dtor
    virtual ~MachPhysElectro();

    //Return an exemplar electro - ensures the mesh is loaded
    static const MachPhysElectro& exemplar(FlashType type);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysElectro& t );

	//nonUniformScale (scale, 2 ,2) after applying Xform and spin a duration time.
	void scaleAndSpin(const PhysAbsoluteTime& burstTime,
							const PhysRelativeTime& duration,
							  const MATHEX_SCALAR scale,
							  const MexTransform3d xForm );

    PER_MEMBER_PERSISTENT( MachPhysElectro );

private:
    MachPhysElectro( const MachPhysElectro& );
    MachPhysElectro& operator =( const MachPhysElectro& );
    bool operator ==( const MachPhysElectro& );

	friend class MachPhysOtherPersistence;
    //One-time constructor (per type) used to create the exemplars
    MachPhysElectro(FlashType type);

};

PER_READ_WRITE( MachPhysElectro );
PER_DECLARE_PERSISTENT( MachPhysElectro );
PER_ENUM_PERSISTENT( MachPhysElectro::FlashType );
#endif

/* End electro.HPP ****************************************************/
