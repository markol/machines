/*
 * P U N C H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"
#include "machphys/punch.hpp"

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "mathex/line2d.hpp"
#include "phys/lsclplan.hpp"

#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/camshake.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"

#include "world4d/entity.hpp"
//#include "world4d/entyplan.hpp"
//#include "world4d/visplan.hpp"

#include "machphys/blast.hpp"
#include "machphys/crakfire.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/snddata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"

PER_DEFINE_PERSISTENT( MachPhysPunchBlast );

static const MachPhysPunchBlast::Points& allPoints();
static const MachPhysPunchBlast::Points& path1();
static const MachPhysPunchBlast::Points& path2();
static const MachPhysPunchBlast::Points& path3();
static const MachPhysPunchBlast::Points& path4();
static const MachPhysPunchBlast::Points& path5();
static const MachPhysPunchBlast::Points& path6();

//create a crackfire streched from startPoint to EndPoint atop of the planetSurface of type
static MachPhysCrackFire& crackFire(W4dEntity* pParent,
                                   const MexPoint3d& startPoint,
                                   const MexPoint3d& endPoint,
								   PhysRelativeTime* timeOffset,
								   const MachPhysPlanetSurface& surface );

MachPhysPunchBlast::MachPhysPunchBlast(W4dEntity* pParent, const MexTransform3d& localTransform)
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID ),
	blastBeginTime_( 0 ),
	blastEndTime_( 0 ),
	startPunchBlastCalled_( false )
{
    TEST_INVARIANT;
}

MachPhysPunchBlast::MachPhysPunchBlast( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysPunchBlast::~MachPhysPunchBlast()
{
    TEST_INVARIANT;

    //Stop any playing sound
    //W4dSoundManager::instance().stop( this );

}

PhysAbsoluteTime MachPhysPunchBlast::blastBeginTime() const
{
	PRE( startPunchBlastCalled() );

	return blastBeginTime_;
}

PhysAbsoluteTime MachPhysPunchBlast::blastEndTime() const
{
	PRE( startPunchBlastCalled() );

	return blastEndTime_;
}

bool MachPhysPunchBlast::startPunchBlastCalled() const
{
	return startPunchBlastCalled_;
}

void MachPhysPunchBlast::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	INVARIANT( implies( startPunchBlastCalled() , blastBeginTime_ != 0 or blastEndTime_ != 0 ) );
}

ostream& operator <<( ostream& o, const MachPhysPunchBlast& t )
{

    o << "MachPhysPunchBlast " << (void*)&t << " start" << std::endl;
    o << "MachPhysPunchBlast " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysPunchBlast::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}


PhysRelativeTime MachPhysPunchBlast::startPunchBlast( const PhysAbsoluteTime& startTime, const MachPhysPlanetSurface& surface, const Crack_Type& type)
{
	startPunchBlastCalled_ = true;

	PhysRelativeTime duration = 71.0/punchFrameRate();
	MATHEX_SCALAR crackScale = 7.5;
	MATHEX_SCALAR fireScale = 9.0;


	if( type == NUKE )
	{
		duration *= 1.0;
	}
	else
	{
		//earth quake
		W4dCameraShake& cameraShake = W4dManager::instance().sceneManager()->cameraShakeForEdit();
		cameraShake.startTime( startTime );
		cameraShake.epicCentre( this->globalTransform().position() );
		cameraShake.duration( duration/2.0 );
		cameraShake.scale( W4dCameraShake::FIRST_DEGREE);

		blastBeginTime_ = startTime;
		blastEndTime_ = startTime + duration;

		MachPhysBlast* pBlast = _NEW( MachPhysBlast( this, MexTransform3d() ) );
	   	pBlast->startBlast(blastBeginTime_);

	    W4dGarbageCollector::instance().add( pBlast, blastEndTime_ );
	}

	MachPhysCrackFire* pCrackFire[30];	//nCrackFire()
	PhysRelativeTime timeOffset[30];
	uint i = 0, j;

	uint npath = path1().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path1()[j], path1()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

	npath = path2().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path2()[j], path2()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

	npath = path3().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path3()[j], path3()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

	npath = path4().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path4()[j], path4()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

	npath = path5().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path5()[j], path5()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

	npath = path6().size() - 1;
	for(j = 0; j<npath; ++j)
	{
		pCrackFire[i] = &crackFire(this, path6()[j], path6()[j+1], &timeOffset[i], surface);
		pCrackFire[i]->startCrackAndFire( startTime+timeOffset[i], -duration, crackScale, fireScale);
		++i;
    }

    ASSERT( i == nCrackFire(), " only nCrackFire() crackFires." );

	for(i=0; i<nCrackFire(); ++i)
	    W4dGarbageCollector::instance().add( pCrackFire[i], startTime + duration );

    return duration;
}

void perWrite( PerOstream& ostr, const MachPhysPunchBlast& bomb )
{
    const W4dEntity& base = bomb;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPunchBlast& bomb )
{
    W4dEntity& base = bomb;

    istr >> base;
}

//static
const uint MachPhysPunchBlast::nCrackFire()
{
	static const uint nCrackFire = 30;
	return nCrackFire;
}

//static
const MATHEX_SCALAR& MachPhysPunchBlast::punchFrameRate()
{
	static const MATHEX_SCALAR frameRate = 25.0;
	return frameRate;
}

//static
PhysRelativeTime MachPhysPunchBlast::duration()
{
	PhysRelativeTime durationTime = 26.0/punchFrameRate();
	return durationTime;
}

//static
bool MachPhysPunchBlast::reachDistance(const MATHEX_SCALAR& distance, PhysRelativeTime* pTime)
{
	//suppose the blast travels from allPoints()[0] to allPoints()[30] with a const speed in 29 - 3 frames
	MexLine2d line(MexPoint2d(allPoints()[0].x(), allPoints()[0].y()),MexPoint2d(allPoints()[30].x(), allPoints()[30].y()));
	PhysRelativeTime time = duration();
	MATHEX_SCALAR speed = line.length()/time;

	*pTime = distance/speed;

	return *pTime <= time;
}

//static
MATHEX_SCALAR MachPhysPunchBlast::range()
{
	static MATHEX_SCALAR maxRange = MexLine2d(MexPoint2d(allPoints()[0].x(), allPoints()[0].y()),MexPoint2d(allPoints()[30].x(), allPoints()[30].y())).length();
	return maxRange;
}

//static
MexRadians MachPhysPunchBlast::absMaxDestructionAngle()
{
	static MexRadians destructionAngle = 0.53;		//!! Currently hard-coded. This may change.
	return destructionAngle;
}

//static
const MachPhysPunchBlast::Points& allPoints()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 30 );
		points.push_back( MexPoint3d(  2.9,   0.2, 3) );
		points.push_back( MexPoint3d(  8.6,   0.9, 3) );
		points.push_back( MexPoint3d(  8.0,  -1.0, 3) );
		points.push_back( MexPoint3d(  8.1,  -3.1, 3) );
		points.push_back( MexPoint3d( 12.7,   3.7, 6) );
		points.push_back( MexPoint3d( 12.9,   0.5, 6) );
		points.push_back( MexPoint3d( 13.1,  -3.5, 6) );
		points.push_back( MexPoint3d( 18.2,   4.2, 9) );
		points.push_back( MexPoint3d( 17.7,  -0.9, 9) );
		points.push_back( MexPoint3d( 17.1,  -6.3, 9) );
		points.push_back( MexPoint3d( 20.5,   9.1, 12) );
		points.push_back( MexPoint3d( 22.6,   0.4, 12) );
		points.push_back( MexPoint3d( 22.6,  -6.8, 12) );
		points.push_back( MexPoint3d( 25.0,  12.4, 16) );
		points.push_back( MexPoint3d( 27.7,  -0.9, 16) );
		points.push_back( MexPoint3d( 28.4,  -6.8, 16) );
		points.push_back( MexPoint3d( 30.3,  15.5, 19) );
		points.push_back( MexPoint3d( 29.4,   8.0, 19) );
		points.push_back( MexPoint3d( 32.8,   0.4, 19) );
		points.push_back( MexPoint3d( 33.9,  -6.8, 19) );
		points.push_back( MexPoint3d( 32.6, -10.7, 19) );
		points.push_back( MexPoint3d( 36.4,   4.9, 22) );
		points.push_back( MexPoint3d( 38.0,  -1.0, 22) );
		points.push_back( MexPoint3d( 38.9,  -9.6, 22) );
		points.push_back( MexPoint3d( 34.7, -15.9, 22) );
		points.push_back( MexPoint3d( 36.0,  15.0, 26) );
		points.push_back( MexPoint3d( 37.9,  10.5, 26) );
		points.push_back( MexPoint3d( 43.6,   0.5, 26) );
		points.push_back( MexPoint3d( 44.8,  -9.6, 26) );
		points.push_back( MexPoint3d( 39.0, -19.5, 26) );
		points.push_back( MexPoint3d( 42.8,  13.4, 29) );

	}
	POST(points.size() == 31);
	return points;
}

//static
const MachPhysPunchBlast::Points& path1()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;

	if( once )
	{
		once = false;
		points.reserve( 8 );

		points.push_back( allPoints()[0] );
		points.push_back( allPoints()[1] );
		points.push_back( allPoints()[4] );
		points.push_back( allPoints()[7] );
		points.push_back( allPoints()[10] );
		points.push_back( allPoints()[13] );
		points.push_back( allPoints()[16] );
		points.push_back( allPoints()[25] );
	}

	return points;
}

//static
const MachPhysPunchBlast::Points& path2()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 2 );
		points.push_back( allPoints()[13] );
		points.push_back( allPoints()[17] );
	}

	return points;
}

//static
const MachPhysPunchBlast::Points& path3()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 4 );
		points.push_back( allPoints()[18] );
		points.push_back( allPoints()[21] );
		points.push_back( allPoints()[26] );
		points.push_back( allPoints()[30] );
	}

	return points;
}

//static
const MachPhysPunchBlast::Points& path4()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 9 );
		points.push_back( allPoints()[0] );
		points.push_back( allPoints()[2] );
		points.push_back( allPoints()[5] );
		points.push_back( allPoints()[8] );
		points.push_back( allPoints()[11] );
		points.push_back( allPoints()[14] );
		points.push_back( allPoints()[18] );
		points.push_back( allPoints()[22] );
		points.push_back( allPoints()[27] );
	}
	return points;
}

//static
const MachPhysPunchBlast::Points& path5()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 9 );
		points.push_back( allPoints()[0] );
		points.push_back( allPoints()[3] );
		points.push_back( allPoints()[6] );
		points.push_back( allPoints()[9] );
		points.push_back( allPoints()[12] );
		points.push_back( allPoints()[15] );
		points.push_back( allPoints()[19] );
		points.push_back( allPoints()[23] );
		points.push_back( allPoints()[28] );
	}
	return points;
}

//static
const MachPhysPunchBlast::Points& path6()
{
	static MachPhysPunchBlast::Points points;
	static bool once = true;
	if( once )
	{
		once = false;
		points.reserve( 4 );
		points.push_back( allPoints()[15] );
		points.push_back( allPoints()[20] );
		points.push_back( allPoints()[24] );
		points.push_back( allPoints()[29] );
	}
	return points;
}


static MachPhysCrackFire& crackFire(W4dEntity* pParent,
                                   const MexPoint3d& startPoint,
                                   const MexPoint3d& endPoint,
								   PhysRelativeTime* timeOffset,
								   const MachPhysPlanetSurface& surface )
{
	MexPoint3d end1 = startPoint;
	MexPoint3d end2 = endPoint;

	*timeOffset = end2.z()/MachPhysPunchBlast::punchFrameRate();

	end1.z(0);
	end2.z(0);

	pParent->globalTransform().transform(&end1);
	pParent->globalTransform().transform(&end2);

	MATHEX_SCALAR z1 = surface.terrainHeight(end1.x(), end1.y());
	MATHEX_SCALAR z2 = surface.terrainHeight(end2.x(), end2.y());

	end1.z( z1 );
	end2.z( z2 );

	pParent->globalTransform().transformInverse(&end1);
	pParent->globalTransform().transformInverse(&end2);

	MexVec3 xAxis(end1, end2);
	MATHEX_SCALAR length = xAxis.modulus();

	xAxis.makeUnitVector();

	MexVec3 zAxis(0, 0, 1); //use the global z axis as the X_XZ plane defining vector
	pParent->globalTransform().transformInverse(&zAxis);

	MexVec3 yAxis = zAxis;
	yAxis.crossProduct(xAxis);
	yAxis.makeUnitVector();

    zAxis = xAxis;
    zAxis.crossProduct(yAxis);
	zAxis.makeUnitVector();

	MexTransform3d localTransform(xAxis, yAxis, zAxis, end1);

  	return *_NEW( MachPhysCrackFire( pParent, localTransform, length, MachPhysCrackFire::GARILLA_PUNCH_CHASM ) );
}

//static
MATHEX_SCALAR MachPhysPunchBlast::radius( const PhysRelativeTime& timeOffset )
{
	static const MATHEX_SCALAR speed = range()/duration();

	return speed*timeOffset;
}



/* End PUNCH.CPP *************************************************/

