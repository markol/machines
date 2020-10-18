/*
 * I O N B E A M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/ionbeam.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "mathex/transf3d.hpp"
#include "phys/lsclplan.hpp"
#include "system/pathname.hpp"
#include "render/texture.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/soundman.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/light.hpp"
#include "machphys/snddata.hpp"
#include "machphys/beam.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/punch.hpp"
#include "machphys/crakfire.hpp"

PER_DEFINE_PERSISTENT( MachPhysIonBeam );

static const MachPhysCrackFire::Points& ringPath();

MachPhysIonBeam::MachPhysIonBeam(W4dEntity* pParent, const MexTransform3d& localTransform)
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    TEST_INVARIANT;
}


MachPhysIonBeam::MachPhysIonBeam( PerConstructor con )
: W4dEntity( con )
{
}


MachPhysIonBeam::~MachPhysIonBeam()
{
    TEST_INVARIANT;

    //Stop any playing sound
    W4dSoundManager::instance().stop( this );
}


void MachPhysIonBeam::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysIonBeam& t )
{

    o << "MachPhysIonBeam " << (void*)&t << " start" << std::endl;
    o << "MachPhysIonBeam " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysIonBeam::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
const MATHEX_SCALAR MachPhysIonBeam::radius()
{
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData(MachPhys::ION_ORBITAL_CANNON);
	static const MATHEX_SCALAR r =  data.extras()[0];
	return r;
}

const PhysRelativeTime& MachPhysIonBeam::duration()
{
	static 	PhysRelativeTime DURATION = 6.0;

	return DURATION;
}

//static
bool MachPhysIonBeam::radiusTime(const  MATHEX_SCALAR& distance, PhysRelativeTime* pTime)
{
	PRE(distance >=0);

    PhysRelativeTime time;
	const double r = radius();	//the default radius of outer is 1.0

	const double alpha =(r-0.01)/duration(); //	 initial scale is 0.01, expands to ionRadius in duration()
	time = (distance - 0.01)/alpha;

	bool reachTarget = true;
	if(time > duration())	//beam has missed the target
		reachTarget = false;

    *pTime = time;

	return reachTarget;
}

static PhysScalarPlanPtr createIonInensityPlan
(
	PhysRelativeTime duration,
	const MachPhysWeaponData& data
)
{
	// Firstly create a low frequency basic plan: rise to a maxiumu, stay
	// there, then drop sharply to zero once the beam is switched off.
	const MATHEX_SCALAR initialIntensity = data.extras()[5];
	const MATHEX_SCALAR maxIntensity     = data.extras()[6];
    PhysLinearScalarPlan::ScalarVec bTimes;
    PhysLinearScalarPlan::ScalarVec bScales;
	bTimes.reserve(2);
	bScales.reserve(3);

    bScales.push_back(initialIntensity);

	bTimes.push_back(duration * 0.7);	 	// reach max before the end
    bScales.push_back(maxIntensity);

	bTimes.push_back(duration);				// stay at max until the end
    bScales.push_back(maxIntensity);

	IAIN_STREAM("  basic scales:\n" << bScales << "\n");
	IAIN_STREAM("  basic times:\n" << bTimes << "\n");

	// Should die at the end of this function.
	PhysScalarPlanPtr basicPlan = _NEW(PhysLinearScalarPlan(bTimes, bScales));

	const MATHEX_SCALAR flickerFrequency = data.extras()[7];
	const MATHEX_SCALAR flickerAmplitude = data.extras()[8] / 2;

	ASSERT_INFO(flickerFrequency);
	ASSERT(flickerFrequency > 0, "Ion cannon light flicker frequency must be >0.");
	const MATHEX_SCALAR halfWavelength = 0.5 / flickerFrequency;
	const size_t nCycles = _STATIC_CAST(size_t, 10 + duration / halfWavelength);

    PhysLinearScalarPlan::ScalarVec fTimes;
    PhysLinearScalarPlan::ScalarVec fScales;
	fTimes.reserve(nCycles);
	fScales.reserve(nCycles+1);

    fScales.push_back(initialIntensity);

	// Superimpose the flicker up until the drop off at the end.  The first
	// peak occurs after a quarter wavelength.
	PhysRelativeTime time = halfWavelength * 0.5;
	MATHEX_SCALAR addition = flickerAmplitude;
	while (time < duration)
	{
		const MATHEX_SCALAR basicInt = basicPlan->scalar(time);
		MATHEX_SCALAR intensity = basicInt + addition;
		if (intensity < 0)
			intensity = 0;

	    fScales.push_back(intensity);
		fTimes.push_back(time);

		// Switch signs to give alternate peaks and troughs.
		addition = -addition;
		time += halfWavelength;
	}

	fTimes.push_back(duration);				// stay at max until the end
    fScales.push_back(maxIntensity);

	fTimes.push_back(duration + 0.3);		// drop sharply to zero
    fScales.push_back(0);

	IAIN_STREAM("  flicker scales:\n" << fScales << "\n");
	IAIN_STREAM("  flicker times:\n" << fTimes << "\n");

	return _NEW(PhysLinearScalarPlan(fTimes, fScales));
}

PhysRelativeTime MachPhysIonBeam::startExplosion( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface )
{
	IAIN_STREAM("Creating Ion Cannon effect.\n");
	MachPhysBeam* pBeam = _NEW( MachPhysBeam( this, MexTransform3d() ) );
    PhysRelativeTime beamDuration = pBeam->startBeam( startTime );
	IAIN_STREAM("  beamDuration=" << beamDuration << "\n");
	IAIN_STREAM("  duration()=" << duration() << "\n");

	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData(MachPhys::ION_ORBITAL_CANNON);
	ASSERT(data.extras().size() == 11, "Not enough Ion Cannon weapon data extra parameters.");

	//create a danamic light
	W4dUniformLight* pLight = _NEW( W4dUniformLight(this, MexVec3(0, 0, -1), data.extras()[4]));

    pLight->localTransform( MexPoint3d(0, 0, data.extras()[9]) );	//high above ground
	pLight->colour(RenColour(data.extras()[1], data.extras()[2], data.extras()[3]) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->visible(false);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, beamDuration);

    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	PhysScalarPlanPtr flickerPlan = createIonInensityPlan(duration(), data);
	pLight->intensityPlan(flickerPlan, startTime);

	// Create another light giving a very brief flash at the end.
	W4dUniformLight* pLight2 = _NEW( W4dUniformLight(this, MexVec3(0, 0, -1), data.extras()[4]));

    pLight2->localTransform(MexPoint3d(0, 0, data.extras()[9]));	//high above ground
	pLight2->colour(RenColour(1.5, 0.78, 0));		// orange
	pLight2->constantAttenuation(0);
	pLight2->linearAttenuation(0.177);
	pLight2->quadraticAttenuation(0.823);
	pLight2->scope(W4dLight::DYNAMIC);
	pLight2->visible(false);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr2( _NEW( W4dVisibilityPlan( false ) ) );
	lightVisibilityPlanPtr2->add(true,  beamDuration - 0.2);
	lightVisibilityPlanPtr2->add(false, beamDuration - 0.1);

    pLight2->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr2, startTime );

	MachPhysCrackFire::createCrackFires( this,
								  MexTransform3d(),
								  ringPath(), 			//positions
								  startTime+duration(),  //startTime
								  data.extras()[10],   //duration
								  surface,
								  25.0, 30.0, 	//crack and fire scales
								  MachPhysCrackFire::DEBRIS_DISPLAY,
								  MachPhysCrackFire::GARILLA_PUNCH_CHASM
								);

    //Play the sound
	W4dSoundManager::instance().play(this, SID_IONCANNON, startTime, 1);

	W4dGarbageCollector::instance().add( pBeam,   startTime + beamDuration );
	W4dGarbageCollector::instance().add( pLight,  startTime + beamDuration );
	W4dGarbageCollector::instance().add( pLight2, startTime + beamDuration );

    return beamDuration;
}

void perWrite( PerOstream& ostr, const MachPhysIonBeam& ionBeam )
{
    const W4dEntity& base = ionBeam;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysIonBeam& ionBeam )
{
    W4dEntity& base = ionBeam;

    istr >> base;
}

//local
static const MachPhysCrackFire::Points& ringPath()
{
	static MachPhysCrackFire::Points points;
	static bool once = true;

	if( once )
	{
		once = false;
		points.reserve( 19 );

		points.push_back( MexPoint2d( 0.0,		 	2.0*15.0 ) );
		points.push_back( MexPoint2d( 2.0*5.13,		2.0*14.1 ) );
		points.push_back( MexPoint2d( 2.0*9.64,		2.0*11.49 ) );
		points.push_back( MexPoint2d( 2.0*12.99,	2.0*7.5 ) );
		points.push_back( MexPoint2d( 2.0*14.77,	2.0*2.6 ) );
		points.push_back( MexPoint2d( 2.0*14.77,	-2.0*2.6 ) );
		points.push_back( MexPoint2d( 2.0*12.99,	-2.0*7.5 ) );
		points.push_back( MexPoint2d( 2.0*9.64,		-2.0*11.49 ) );
		points.push_back( MexPoint2d( 2.0*5.13,		-2.0*14.1 ) );
		points.push_back( MexPoint2d( 0.0,			-2.0*15.0 ) );
		points.push_back( MexPoint2d( -2.0*5.13,	-2.0*14.1 ) );
		points.push_back( MexPoint2d( -2.0*9.64,	-2.0*11.49 ) );
		points.push_back( MexPoint2d( -2.0*12.99,	-2.0*7.5 ) );
		points.push_back( MexPoint2d( -2.0*14.77,	-2.0*2.6 ) );
		points.push_back( MexPoint2d( -2.0*14.77,	2.0*2.6 ) );
		points.push_back( MexPoint2d( -2.0*12.99,	2.0*7.5 ) );
		points.push_back( MexPoint2d( -2.0*9.64,	2.0*11.49 ) );
		points.push_back( MexPoint2d( -2.0*5.13,	2.0*14.49 ) );
		points.push_back( MexPoint2d( 0.0,			2.0*15.0 ) );
	}

	return points;
}

/* End IONBEAM.CPP *************************************************/

