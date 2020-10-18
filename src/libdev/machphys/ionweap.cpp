/*
 * B O L T E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ionweap.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"

#include "world4d/soundman.hpp"
#include "world4d/root.hpp"
#include "world4d/generic.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/composit.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"

#include "phys/lsclplan.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/scale.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysIonWeapon );

MachPhysIonWeapon::MachPhysIonWeapon
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   MachPhysWeapon( exemplar(type), mounting, pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysIonWeapon::MachPhysIonWeapon( MachPhys::WeaponType type )
:   MachPhysWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath( type ) ), type, MachPhys::LEFT )
{
    TEST_INVARIANT;
}

MachPhysIonWeapon::~MachPhysIonWeapon()
{
    TEST_INVARIANT;

}

//static
const MachPhysIonWeapon& MachPhysIonWeapon::exemplar( MachPhys::WeaponType type )
{
    return MachPhysWeaponPersistence::instance().ionExemplar(type);
}

void MachPhysIonWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysIonWeapon& t )
{

    o << "MachPhysIonWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysIonWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysIonWeapon::compositeFilePath(MachPhys::WeaponType)
{
	return  "models/weapons/nmissile/point.cdf";
}

//virtual
PhysRelativeTime MachPhysIonWeapon::fire( const PhysAbsoluteTime&, int )
{
    return 0;
}


//static
PhysRelativeTime MachPhysIonWeapon::destroy(W4dEntity* pVictim, const PhysAbsoluteTime& startTime)
{
	//create a copy of the object being destroyed  through a  generic
    W4dEntity* pCopyTarget;

    if( pVictim->isComposite() )
        pCopyTarget = _NEW( W4dGenericComposite( pVictim->asComposite(),
                                                 pVictim->pParent(), pVictim->localTransform() ) );
    else
        pCopyTarget = _NEW( W4dGeneric( *pVictim, pVictim->pParent(), pVictim->localTransform() ) );

	PhysRelativeTime duration = 1.5;

//	make plans for the original

    //Make a simple scale plan
    PhysLinearScalarPlan::ScalarVec intTimes;
    	intTimes.reserve(1);
		intTimes.push_back(duration);

    PhysLinearScalarPlan::ScalarVec intensities;
    	intensities.reserve(2);
        intensities.push_back(1);
        intensities.push_back(3);

    PhysLinearScalarPlan* pScalePlan = _NEW( PhysLinearScalarPlan(intTimes, intensities) );
    W4dScalePlanPtr planPtr( _NEW( W4dGeneralUniformScalePlan( pScalePlan ) ) );

    //Propogate thru the current model
	pVictim->propogateScalePlan( planPtr, startTime, 1);

	//make a visibility plan

    W4dVisibilityPlanPtr wVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	wVisibilityPlanPtr->add(false, duration);

    pVictim->entityPlanForEdit().visibilityPlan( wVisibilityPlanPtr, startTime );


	//alpha plan. Ensure construct material only once, since any change to a shared material involves
    //searching the entire set of materials for a match.
	static RenMaterial glowingWhite;
    static bool firstTime = true;
    if( firstTime )
    {
    	glowingWhite.diffuse(RenColour::black());
    	glowingWhite.emissive(RenColour::white());
        firstTime = false;
    }

    const int reasonableSize = 100;
    RenMaterialVec* pMaterialVec = _NEW( RenMaterialVec( reasonableSize ) );
    for( int i = reasonableSize; i--; )
        pMaterialVec->push_back( glowingWhite );

    PhysLinearScalarPlan::ScalarVec linearTimes;
    	linearTimes.reserve(1);
		linearTimes.push_back(duration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(2);
        scales.push_back(1);
        scales.push_back(0);

    PhysLinearScalarPlan* pAlphaPlan = _NEW( PhysLinearScalarPlan(linearTimes, scales) );
    PhysScalarPlanPtr alphaPlanPtr( pAlphaPlan );

    W4dSimpleAlphaPlan* pPlan =
                    _NEW( W4dSimpleAlphaPlan( glowingWhite, reasonableSize, alphaPlanPtr, 5 ) );

    W4dMaterialPlanPtr pMaterialPlanPtr( pPlan );

    for( W4dEntityIter it( pVictim ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( pMaterialPlanPtr, startTime );
        (*it).entityPlanForEdit().visibilityPlan( wVisibilityPlanPtr, startTime );
	}

//	make plans for the copy

	//apply the same material and visibility plan  to the copy
    for( W4dEntityIter it( pCopyTarget ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( pMaterialPlanPtr, startTime );
        (*it).entityPlanForEdit().visibilityPlan( wVisibilityPlanPtr, startTime );
	}

	//but a different scale plan

    PhysLinearScalarPlan::ScalarVec copyScales;
    	copyScales.reserve(2);
        copyScales.push_back(1);
        copyScales.push_back(0.001);

    PhysLinearScalarPlan* pCopyScalePlan = _NEW( PhysLinearScalarPlan(intTimes, copyScales) );
    W4dScalePlanPtr copyPlanPtr( _NEW( W4dGeneralUniformScalePlan( pCopyScalePlan ) ) );

    //Propogate thru the current model
	pCopyTarget->propogateScalePlan( copyPlanPtr, startTime, 1);

	//destroy the copytarget when it becomes invisible
	W4dGarbageCollector::instance().add(pCopyTarget, startTime+ duration);

	return duration;
}

MachPhysIonWeapon::MachPhysIonWeapon( PerConstructor con )
: MachPhysWeapon( con )
{
}


void perWrite( PerOstream& ostr, const MachPhysIonWeapon& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysIonWeapon& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
}


/* End ionbeam.CPP ****************************************************/

