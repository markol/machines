/*
 * B E E B O M B E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/beebombe.hpp"

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"

#include "phys/lsclplan.hpp"

#include "world4d/entity.hpp"
#include "world4d/light.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/generic.hpp"
#include "world4d/soundman.hpp"

#include "machphys/beewave.hpp"
#include "machphys/punch.hpp"
#include "machphys/crakfire.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"
#include "sim/manager.hpp"

//locals
static const MachPhysWeaponData& beeWeaponData();
static void createLight( W4dEntity* pParent, const MexTransform3d& position, const PhysAbsoluteTime& startTime );
static const MachPhysCrackFire::Points createRingPath( const MATHEX_SCALAR& time );

static const MachPhysCrackFire::Points& ringPath1();
static const MachPhysCrackFire::Points& ringPath2();
static const MachPhysCrackFire::Points& ringPath3();
static const MachPhysCrackFire::Points& ringPath4();
static const MachPhysCrackFire::Points& ringPath5();
static const MachPhysCrackFire::Points& ringPath6();

//members

MachPhysBeeBombExplosion::MachPhysBeeBombExplosion( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID )
{
	// We do not want to light up the esplosion
	doNotLight(true);

    TEST_INVARIANT;
}

MachPhysBeeBombExplosion::~MachPhysBeeBombExplosion()
{
    TEST_INVARIANT;
}

void MachPhysBeeBombExplosion::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBeeBombExplosion& t )
{

    o << "MachPhysBeeBombExplosion " << (void*)&t << " start" << std::endl;
    o << "MachPhysBeeBombExplosion " << (void*)&t << " end" << std::endl;

    return o;
}

const PhysRelativeTime MachPhysBeeBombExplosion::startExplosion( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface)
{
	const PhysRelativeTime fireDuration = createFireRings(this, MexTransform3d(), startTime + 0.25, surface);

	MachPhysBeeBombWave* pWave = _NEW( MachPhysBeeBombWave( this, MexTransform3d() ) );

	const PhysRelativeTime waveDuration = pWave->startBeeWave( startTime );

    W4dSoundManager::instance().play( this, SID_BEE_DETONATE,
                          startTime, 1 );

    W4dGarbageCollector::instance().add( pWave, startTime + waveDuration );

	return std::max( fireDuration, waveDuration );
}

//virtual
bool MachPhysBeeBombExplosion::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
const PhysRelativeTime MachPhysBeeBombExplosion::createFireRings(W4dEntity* pParent, const MexTransform3d& position, const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface)
{
	PhysAbsoluteTime duration, ringDuration;
  	//first ring
	PhysAbsoluteTime ringStartTime = startTime + beeWeaponData().extras()[15];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath1(), ringStartTime, beeWeaponData().extras()[21], surface, 15, beeWeaponData().extras()[9], MachPhysCrackFire::DEBRIS_NOT_DISPLAY );

	ringDuration = beeWeaponData().extras()[15] + beeWeaponData().extras()[21];
	duration = ringDuration;

	//second ring
	ringStartTime = startTime + beeWeaponData().extras()[16];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath2(), ringStartTime, beeWeaponData().extras()[22], surface, 15, beeWeaponData().extras()[10] );

	ringDuration = beeWeaponData().extras()[16] + beeWeaponData().extras()[22];
	if ( ringDuration > duration )
		duration = ringDuration;


	//third ring
	ringStartTime = startTime + beeWeaponData().extras()[17];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath3(), ringStartTime, beeWeaponData().extras()[23], surface, 15, beeWeaponData().extras()[11] );

	ringDuration = beeWeaponData().extras()[17] + beeWeaponData().extras()[23];
	if ( ringDuration > duration )
		duration = ringDuration;

	//fourth ring
	ringStartTime = startTime + beeWeaponData().extras()[18];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath4(), ringStartTime, beeWeaponData().extras()[24], surface, 15, beeWeaponData().extras()[12] );

	ringDuration = beeWeaponData().extras()[18] + beeWeaponData().extras()[24];
	if ( ringDuration > duration )
		duration = ringDuration;

	//fifth ring
	ringStartTime = startTime + beeWeaponData().extras()[19];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath5(), ringStartTime, beeWeaponData().extras()[25], surface, 15, beeWeaponData().extras()[13] );

	ringDuration = beeWeaponData().extras()[19] + beeWeaponData().extras()[25];
	if ( ringDuration > duration )
		duration = ringDuration;

	//sixth ring
	ringStartTime = startTime + beeWeaponData().extras()[20];
	createLight( pParent, position, ringStartTime );
	MachPhysCrackFire::createCrackFires( pParent, position, ringPath6(), ringStartTime, beeWeaponData().extras()[26], surface, 15, beeWeaponData().extras()[14] );

	ringDuration = beeWeaponData().extras()[20] + beeWeaponData().extras()[26];
	if ( ringDuration > duration )
		duration = ringDuration;


	//create a blue light which becomes visible when each ring shows up
	W4dUniformLight* pLight1 = _NEW(W4dUniformLight(pParent, MexVec3(0, 0, 1), 100.0));
	pLight1->constantAttenuation(0);
	pLight1->linearAttenuation(0.177);
	pLight1->quadraticAttenuation(0.823);
	pLight1->scope(W4dLight::DYNAMIC);
	pLight1->visible(false);
	pLight1->localTransform( position );

	pLight1->colour(RenColour( beeWeaponData().extras()[0], beeWeaponData().extras()[1], beeWeaponData().extras()[2] )  );

    W4dVisibilityPlanPtr light1VisibilityPlanPtr(_NEW(W4dVisibilityPlan(true)));
	light1VisibilityPlanPtr->add(false, 0.12);
	light1VisibilityPlanPtr->add(true, beeWeaponData().extras()[16]);
	light1VisibilityPlanPtr->add(false, 0.12);
	light1VisibilityPlanPtr->add(true, beeWeaponData().extras()[17]);
	light1VisibilityPlanPtr->add(false, 0.12);
	light1VisibilityPlanPtr->add(true, beeWeaponData().extras()[18]);
	light1VisibilityPlanPtr->add(false, 0.12);
	light1VisibilityPlanPtr->add(true, beeWeaponData().extras()[19]);
	light1VisibilityPlanPtr->add(false, 0.12);
	light1VisibilityPlanPtr->add(true, beeWeaponData().extras()[20]);
	light1VisibilityPlanPtr->add(false, 0.12);

    pLight1->entityPlanForEdit().visibilityPlan( light1VisibilityPlanPtr, startTime );

    //Ensure it gets garbage collected
    W4dGarbageCollector::instance().add(pLight1, duration + startTime);

	//and 1 more lights
	const PhysRelativeTime lightDuration = 6;
	if ( lightDuration > duration )
		duration = lightDuration;

	W4dUniformLight* pLight = _NEW(W4dUniformLight(pParent, MexVec3(0, 0, 1), 100.0));
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->visible(false);
	pLight->localTransform( position );
	pLight->colour(RenColour::white()  );

    W4dVisibilityPlanPtr lightVisibilityPlanPtr(_NEW(W4dVisibilityPlan(false)));
	lightVisibilityPlanPtr->add(true, 1);
	lightVisibilityPlanPtr->add(false, lightDuration);
    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	PhysLinearScalarPlan::ScalarVec times;
	times.reserve(4);
	times.push_back( 1 );
	times.push_back( 1.5 );
	times.push_back( 4 );
	times.push_back( 6 );

	PhysLinearScalarPlan::ScalarVec ints;
	ints.reserve( 5 );
	ints.push_back( 2 );
	ints.push_back( 2 );
	ints.push_back( 0.02 );
	ints.push_back( 4 );
	ints.push_back( 0 );

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, ints) );
	PhysScalarPlanPtr intensityPlanPtr = pPlan;
	pLight->intensityPlan(intensityPlanPtr, startTime);

    //Ensure it gets garbage collected
    W4dGarbageCollector::instance().add(pLight, lightDuration + startTime);

	return 	duration;
}

//non member functions
static const MachPhysWeaponData& beeWeaponData()
{
	static const MachPhysWeaponData& weaponData = MachPhysData::instance().weaponData(MachPhys::BEE_BOMB);
	return weaponData;
}

static const MachPhysCrackFire::Points createRingPath( const MATHEX_SCALAR& time)
{
	MachPhysCrackFire::Points points;
	points.reserve( 19 );

	points.push_back( MexPoint2d( 0.0,		 	time*15.0 ) );
	points.push_back( MexPoint2d( time*5.13,		time*14.1 ) );
	points.push_back( MexPoint2d( time*9.64,		time*11.49 ) );
	points.push_back( MexPoint2d( time*12.99,	time*7.5 ) );
	points.push_back( MexPoint2d( time*14.77,	time*2.6 ) );
	points.push_back( MexPoint2d( time*14.77,	-time*2.6 ) );
	points.push_back( MexPoint2d( time*12.99,	-time*7.5 ) );
	points.push_back( MexPoint2d( time*9.64,		-time*11.49 ) );
	points.push_back( MexPoint2d( time*5.13,		-time*14.1 ) );
	points.push_back( MexPoint2d( 0.0,			-time*15.0 ) );
	points.push_back( MexPoint2d( -time*5.13,	-time*14.1 ) );
	points.push_back( MexPoint2d( -time*9.64,	-time*11.49 ) );
	points.push_back( MexPoint2d( -time*12.99,	-time*7.5 ) );
	points.push_back( MexPoint2d( -time*14.77,	-time*2.6 ) );
	points.push_back( MexPoint2d( -time*14.77,	time*2.6 ) );
	points.push_back( MexPoint2d( -time*12.99,	time*7.5 ) );
	points.push_back( MexPoint2d( -time*9.64,	time*11.49 ) );
	points.push_back( MexPoint2d( -time*5.13,	time*14.49 ) );
	points.push_back( MexPoint2d( 0.0,			time*15.0 ) );

	return points;
}

static const MachPhysCrackFire::Points& ringPath1()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[3];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static const MachPhysCrackFire::Points& ringPath2()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[4];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static const MachPhysCrackFire::Points& ringPath3()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[5];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static const MachPhysCrackFire::Points& ringPath4()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[6];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static const MachPhysCrackFire::Points& ringPath5()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[7];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static const MachPhysCrackFire::Points& ringPath6()
{
	static const MATHEX_SCALAR times = beeWeaponData().extras()[8];
	static MachPhysCrackFire::Points points = createRingPath( times );;

	return points;
}

static void createLight( W4dEntity* pParent, const MexTransform3d& position, const PhysAbsoluteTime& startTime )
{
	W4dUniformLight* pLight = _NEW(W4dUniformLight(pParent, MexVec3(0, 0, 1), 100.0));
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->visible(false);
	pLight->localTransform( position );

	pLight->colour(RenColour( beeWeaponData().extras()[0], beeWeaponData().extras()[1], beeWeaponData().extras()[2] )  );

	const PhysRelativeTime lightDuration = 0.12;
    W4dVisibilityPlanPtr lightVisibilityPlanPtr(_NEW(W4dVisibilityPlan(true)));
	lightVisibilityPlanPtr->add(false, lightDuration);
    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

    //Ensure it gets garbage collected
    W4dGarbageCollector::instance().add(pLight, lightDuration + startTime);
}

/* End BEEBOMBE.CPP ***************************************************/
