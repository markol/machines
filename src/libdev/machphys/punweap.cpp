/*
 * P U N W E A P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/punweap.hpp"
#include "machphys/private/weapper.hpp"

#include "stdlib/string.hpp"

#include "machphys/machphys.hpp"
#include "machphys/punch.hpp"
#include "machphys/random.hpp"
#include "machphys/machine.hpp"
#include "machphys/snddata.hpp"

#include "world4d/soundman.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"

#include "render/colour.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysPunchWeapon );

MachPhysPunchWeapon::MachPhysPunchWeapon( W4dEntity* pParent, const MexTransform3d& localTransform)
: MachPhysWeapon( exemplar(), MachPhys::TOP, pParent, localTransform )
{
	visible( false );
    TEST_INVARIANT;
}

//One-time ctor
MachPhysPunchWeapon::MachPhysPunchWeapon()
:MachPhysWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( compositeFilePath() ),
                          MachPhys::GORILLA_PUNCH, MachPhys::TOP )
{
    TEST_INVARIANT;
}

MachPhysPunchWeapon::~MachPhysPunchWeapon()
{
    TEST_INVARIANT;

}

//static
const MachPhysPunchWeapon& MachPhysPunchWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().punchExemplar();
}

void MachPhysPunchWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPunchWeapon& t )
{

    o << "MachPhysPunchWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysPunchWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysPunchWeapon::compositeFilePath()
{
	return "models/weapons/nmissile/point.cdf";
}

//virtual
PhysRelativeTime MachPhysPunchWeapon::fire( const PhysAbsoluteTime&, int  )
{

	//PhysRelativeTime duration = 5;
	//lighting(RenColour(0.784, 0.784, 0.784), startTime, 5);

    return 0;
}

MachPhysPunchWeapon::MachPhysPunchWeapon( PerConstructor con )
: MachPhysWeapon( con )
{
}


void perWrite( PerOstream& ostr, const MachPhysPunchWeapon& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPunchWeapon& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
}


MachPhysPunchBlast* MachPhysPunchWeapon::createPunchBlast(const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface)
{

	W4dCompositePlanPtr punchPlanPtr;

	uint AorB = MachPhysRandom::randomInt( 0, 2 );
	bool findPlan;

	if( AorB )
	{
		findPlan = machine().findCompositePlan( "PunchA", &punchPlanPtr );
		ASSERT( findPlan, "PunchA plan not found");
	}
	else
	{
		findPlan = machine().findCompositePlan( "PunchB", &punchPlanPtr );
		ASSERT( findPlan, "PunchB plan not found");
	}

    machine().plan( *punchPlanPtr, startTime);

	RICHARD_STREAM("Punch play" << std::endl);
	W4dSoundManager::instance().play(this, SID_PUNCH, startTime, 1);

	PhysAbsoluteTime animEndTime = startTime + 42.0/MachPhysPunchBlast::punchFrameRate();

	//the blast is at MexPoint3d(2.9, 0.2, 0.0) with respect to the gorilla
	MexTransform3d blastXform = machine().localTransform();
	blastXform.translate(MexPoint3d(2.9, 0.2, 0.0));

	MachPhysPunchBlast* pPunchBlast = _NEW( MachPhysPunchBlast( machine().pParent(), blastXform) );
	PhysRelativeTime blastDuration = pPunchBlast->startPunchBlast(animEndTime, surface);

	W4dUniformLight* pLight = _NEW( W4dUniformLight(pPunchBlast, MexVec3(1, 0, 0), 20.0));
    pLight->localTransform( MexTransform3d() );
	pLight->colour(RenColour(3, 3, 3)); //::white());
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.17);
	pLight->quadraticAttenuation(0.83);
	pLight->scope(W4dLight::LOCAL);
	pLight->illuminate( this );
	pLight->visible( false );
	pLight->maxRange( 25 );
	//pLight->localTransform( spherePosition );

	//attach the entity to the light
	if(hasMachine())
		pLight->illuminate(&machine());

	//visibility plan
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add( false, 0.8 );

	//apply the visibility plan
    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, animEndTime );

    W4dGarbageCollector::instance().add( pLight, animEndTime + 0.8 );

	return 	pPunchBlast;
}

/* End PUNWEAP.CPP ****************************************************/
