/*
 * D A M A G E I . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachPhysEntityDamageImpl

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_DAMAGEI_HPP
#define _MACHPHYS_DAMAGEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "ctl/vector.hpp"

class W4dEntity;
class MachPhysGroundSplat;
class MachPhysSmokePlume;
class MachPhysConstruction;
class MachPhysSTFFlame;
class MachPhysFlame;

class MachPhysEntityDamageImpl
{
public:

    PER_MEMBER_PERSISTENT( MachPhysEntityDamageImpl );
    PER_FRIEND_READ_WRITE( MachPhysEntityDamageImpl );		  

private:

    MachPhysEntityDamageImpl( W4dEntity* pTarget );
    MachPhysEntityDamageImpl( MachPhysConstruction* pTarget );

    ~MachPhysEntityDamageImpl( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysEntityDamageImpl& t );
	friend class MachPhysEntityDamage;

    MachPhysEntityDamageImpl( const MachPhysEntityDamageImpl& );
    MachPhysEntityDamageImpl& operator =( const MachPhysEntityDamageImpl& );

	enum TargetType{ CONSTRUCTION, NON_CONSTRUCTION};
	PER_FRIEND_ENUM_PERSISTENT( TargetType );

	W4dEntity* pTarget_;
	MachPhysConstruction* ptargetConstruction_; 
	MachPhysGroundSplat* pSplat_;
	MachPhysSmokePlume* pSmoke1_;
	MachPhysSmokePlume* pSmoke2_;
	MachPhysSTFFlame* pFlame1_;
	MachPhysSTFFlame* pFlame2_;    
	MachPhysSTFFlame* pFlame3_;
	double damageLevel_;
	//MexPoint3d refPoint_;  //defines the reference impact point (global coordinates)
	PhysAbsoluteTime lastSmoke1Time_;
	PhysAbsoluteTime lastSmoke2Time_;
	PhysRelativeTime smokeDuration_;
	MexTransform3d transform_; //preserve the localTransform of pTarget_ for smokes and splat so that they would not sink with the target when it is destroyed.
	MachPhysPuffType smoke1Type_;
	MachPhysPuffType smoke2Type_;

	TargetType targetType_;	
	MexAlignedBox3d  targetVolume_;

	ctl_vector<MexTransform3d> flameTransforms_;
	ctl_vector<MATHEX_SCALAR> flameWidths_;
	ctl_vector<MATHEX_SCALAR> flameHeights_;
	    
};

PER_DECLARE_PERSISTENT( MachPhysEntityDamageImpl );
#endif

/* End DAMAGEI.HPP **************************************************/
