/*
 * B O L T E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/vortweap.hpp"
#include "machphys/snddata.hpp"

#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/generic.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/composit.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"

#include "phys/lsclplan.hpp"
#include "phys/timeangl.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/scale.hpp"

#include "machphys/private/weapper.hpp"
#include "machphys/wepdata.hpp"
#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysVortexWeapon );

MachPhysVortexWeapon::MachPhysVortexWeapon
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::Mounting mounting
)
:   MachPhysWeapon( exemplar(), mounting, pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysVortexWeapon::MachPhysVortexWeapon()
:   MachPhysWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::VORTEX, MachPhys::TOP )
{
    TEST_INVARIANT;
}

MachPhysVortexWeapon::~MachPhysVortexWeapon()
{
    TEST_INVARIANT;

}

//static
const MachPhysVortexWeapon& MachPhysVortexWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().vortexExemplar();
}

void MachPhysVortexWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVortexWeapon& t )
{

    o << "MachPhysVortexWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysVortexWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysVortexWeapon::compositeFilePath()
{
	return "models/weapons/vortex/vrtx.cdf";
}

//virtual
PhysRelativeTime MachPhysVortexWeapon::fire( const PhysAbsoluteTime&, int  )
{

    return 0;
}

PhysRelativeTime MachPhysVortexWeapon::destroy(W4dEntity* pVictim, const PhysAbsoluteTime& startTime)
{
	PhysRelativeTime duration = 0.5;
    //Make a simple scale plan
    RenNonUniformScale a(1,1,1);
    RenNonUniformScale b(0.125,0.125,20);
    W4dScalePlanPtr planPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

    //Propogate thru the current model
	pVictim->propogateScalePlan( planPtr, startTime, 1);

	//make a visibility plan

    W4dVisibilityPlanPtr wVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	wVisibilityPlanPtr->add(false, duration);

    pVictim->entityPlanForEdit().visibilityPlan( wVisibilityPlanPtr, startTime );

	// Add a brief light so that he victim is lit up
	const MATHEX_SCALAR lightRange = 20.0;
	W4dUniformLight* pLight = _NEW(W4dUniformLight(pVictim, MexVec3(0, 0, 1), lightRange));
	pLight->colour(RenColour(2, 2, 2.3));
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->visible( false );

	PhysAbsoluteTime lightStartTime = startTime-0.5;
	PhysRelativeTime lightDuration = duration + 0.5;
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);
    pLight->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, lightStartTime);


    PhysLinearScalarPlan::ScalarVec lightTimes;
    	lightTimes.reserve(2);
		lightTimes.push_back(0.3);
		lightTimes.push_back(lightDuration);

    PhysLinearScalarPlan::ScalarVec intensities;
    	intensities.reserve(3);
        intensities.push_back(1);
        intensities.push_back(1);
        intensities.push_back(0);

    PhysLinearScalarPlan* plightIntensityPlan = _NEW( PhysLinearScalarPlan(lightTimes, intensities) );

	PhysScalarPlanPtr intensityPlanPtr = plightIntensityPlan;
	pLight->intensityPlan(intensityPlanPtr, lightStartTime);

    W4dGarbageCollector::instance().add( pLight, lightStartTime + lightDuration );

	return duration;
}

MachPhysVortexWeapon::MachPhysVortexWeapon( PerConstructor con )
: MachPhysWeapon( con )
{
}


void perWrite( PerOstream& ostr, const MachPhysVortexWeapon& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysVortexWeapon& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
}

/* End BOLTER.CPP ****************************************************/
