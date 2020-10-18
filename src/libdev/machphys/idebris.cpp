/*
 * D E B R I S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


///////////////////////////////////

#include "machphys/debris.hpp"
#include "machphys/random.hpp"
#include "machphys/flames.hpp"
#include "machphys/snddata.hpp"

#include "world4d/root.hpp"
#include "world4d/soundman.hpp"

#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"

#include "sim/manager.hpp"

//#include <time.h>
//removed HA // RP 3/12/97  I'm the only person using this
//removed HA // just now so these have gone in here.
//removed HA
//removed HA void seed()
//removed HA {
//removed HA 	time_t timeOfDay = time( NULL );
//removed HA 	srand( timeOfDay );
//removed HA }
//removed HA
//removed HA int randomInt( int minInt, int maxInt )
//removed HA {
//removed HA 	return ( rand() % ( maxInt - minInt ) ) + minInt;
//removed HA }
//removed HA
//removed HA double randomDouble( double minValue, double maxValue )
//removed HA {
//removed HA 	// value between 0 and 1
//removed HA 	double	randDouble = (double) rand() / (double) RAND_MAX;
//removed HA
//removed HA 	double	result = randDouble * ( maxValue - minValue ) + minValue;
//removed HA
//removed HA 	return result;
//removed HA }

PER_DEFINE_PERSISTENT( MachPhysDebris );

MachPhysDebris::MachPhysDebris
(
	W4dEntity* pParent,
	const W4dTransform3d& localTransform,
	const MexAlignedBox2d& boundary,
    const PhysRelativeTime& burningDuration,
	const MachPhysDebrisType& debrisType
)
: W4dEntity( factory(), pParent, localTransform ),
burningDuration_(burningDuration),
debrisType_(debrisType)
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

	MATHEX_SCALAR boundXMin = boundary.minCorner().x();
	MATHEX_SCALAR boundYMin = boundary.minCorner().y();
	MATHEX_SCALAR boundXMax = boundary.maxCorner().x();
	MATHEX_SCALAR boundYMax = boundary.maxCorner().y();
	MATHEX_SCALAR halfXSize = ( boundary.maxCorner().x() - boundary.minCorner().x() ) / 2;
	MATHEX_SCALAR halfYSize = ( boundary.maxCorner().y() - boundary.minCorner().y() ) / 2;

    MexPoint2d minCorner(0, 0);
    MexPoint2d maxCorner(5, 5);
	MexAlignedBox2d localDebrisBoundary( -1 * halfXSize, -1 * halfYSize, halfXSize , halfYSize );

//	seed();

	int nDebrisUnits = 1;	// for smallest (default) case, SMALL

	switch(debrisType_)
    {
		// !!! NOTE - This is a deliberate DROP-THROUGH switch !!!

		case LARGE:
			nDebrisUnits += 1;
		case LARGISH:
			nDebrisUnits += 1;
		case MEDIUM:
			nDebrisUnits += 1;
		case SMALLISH:
			nDebrisUnits += 1;
		default:
			;
	}

	for (int debrisLoop = 0; debrisLoop < nDebrisUnits; ++debrisLoop)
	{
		double xVal = MachPhysRandom::randomDouble(-1 * halfXSize, halfXSize);
    	double yVal = MachPhysRandom::randomDouble(-1 * halfYSize, halfYSize);
        size_t rotation = MachPhysRandom::randomInt(0, 360);
	    MexPoint3d unitPosition(xVal, yVal, 0 );
        MexEulerAngles rotationAngle(MexDegrees( rotation ), 0, 0);
        W4dTransform3d tx;
        tx.translate(unitPosition);
    	tx.rotate( rotationAngle );
		debrisUnits_.push_back( _NEW(MachPhysDebrisUnit(this,
        								 tx,
        								 localDebrisBoundary,
                                         burningDuration) ) );
	}

	TEST_INVARIANT;
}

MachPhysDebris::MachPhysDebris( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysDebris::~MachPhysDebris()
{
    TEST_INVARIANT;
    W4dSoundManager::instance().stop( this );
}

const MachPhysDebris& MachPhysDebris::factory()
{
    static W4dRoot root( 1 );
    //static MachPhysDebris debris( &root, MexTransform3d::MexTransform3d() );
    static MachPhysDebris debris( &root, MexTransform3d() );

    return debris;
}

MachPhysDebris::MachPhysDebris
(
	W4dEntity* pParent,
	const MexTransform3d& transform
)
: W4dEntity(  pParent, transform, SOLID )
{
	  visible(true);
//    TEST_INVARIANT;
//    SysPathName lodFile( "models/debris/debris.lod" );
//    ASSERT_FILE_EXISTS( lodFile.pathname().c_str() );
//    readLODFile( lodFile );
}

void MachPhysDebris::startBurning()
{
   	for ( DebrisUnits::iterator i = debrisUnits_.begin(); i != debrisUnits_.end(); ++i )
   	{
   		(*i)->startBurning();
   	}

//	string burningSound = "sounds/fire.wav";
	PhysAbsoluteTime currentTime = SimManager::instance().currentTime();
	W4dSoundManager::instance().playForDuration( this, SID_FIRE, currentTime, burningDuration_ );
}

bool MachPhysDebris::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysDebris::DebrisUnits& MachPhysDebris::debrisUnits()
{
	return debrisUnits_;
}

void MachPhysDebris::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const MachPhysDebris& debris )
{
    const W4dEntity& base = debris;

    ostr << base;

    ostr << debris.debrisType_;
    ostr << debris.burningDuration_;
    ostr << debris.debrisUnits_;
}

void perRead( PerIstream& istr, MachPhysDebris& debris )
{
    W4dEntity& base = debris;

    istr >> base;

    istr >> debris.debrisType_;
    istr >> debris.burningDuration_;
    istr >> debris.debrisUnits_;
}


/* End DEBRIS.CPP ***************************************************/
