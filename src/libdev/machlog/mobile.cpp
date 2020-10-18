/*
 * M O B I L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>

#include "base/base.hpp"
#include "stdlib/string.hpp"

#include "mathex/mathex.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/poly2d.hpp"
#include "phys/motchunk.hpp"
#include "world4d/entity.hpp"
#include "world4d/composit.hpp"
#include "world4d/domain.hpp"
#include "world4d/garbcoll.hpp"
#include "sim/manager.hpp"

#include "machphys/machine.hpp"
#include "machphys/machdata.hpp"

#include "machlog/apc.hpp"
#include "machlog/mobile.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/machine.hpp"
#include "machlog/actmaker.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

/* //////////////////////////////////////////////////////////////// */

const double FPS = 15.0;
const double FRAME_TIME = 1.0 / FPS;

/* //////////////////////////////////////////////////////////////// */

static 
bool withinEpsilonOf( MATHEX_SCALAR a, MATHEX_SCALAR b )
{
	return Mathex::abs( a - b ) <= MexEpsilon::instance();
}

/* //////////////////////////////////////////////////////////////// */

static
MATHEX_SCALAR
transformToOrientationInRadians( const MexTransform3d& xform )
{
	// Compute the heading from the xbasis vector.
	MexVec3 xBasis;
	xform.xBasis( &xBasis );
	xBasis[ 2 ] = 0;

	MATHEX_SCALAR eps = MexEpsilon::instance();
	bool pointingDown = Mathex::abs( xBasis[ 0 ] ) < eps
		and Mathex::abs( xBasis[ 1 ] ) < eps;
	
	ASSERT( not pointingDown, logic_error() );


	MexEulerAngles e;
	xform.rotation( &e );

	// By convention, north is along the -ve y-axis.
	MATHEX_SCALAR result = MexRadians( e.azimuth() + MexRadians( Mathex::PI_DIV_2 ) ).asScalar();

	if( result < 0 )
	{
		if( withinEpsilonOf( result, 0 ) )
			result = 0;
		else
			result += 2 * Mathex::PI;
	}
	else if( result > 2 * Mathex::PI )
	{
		if( withinEpsilonOf( result, 2 * Mathex::PI ) )
			result = 2 * Mathex::PI;
		else
			result -= 2 * Mathex::PI;
	}

	POST( result >= 0 and result <= ( 2 * Mathex::PI ) );
	return result;
}

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT_ABSTRACT( MachLogMobile );

MachLogMobile::MachLogMobile( MachLogRace * pRace, 
								MachPhysMobile * pPhysMobile,
								MachLog::ObjectType ot,
								MATHEX_SCALAR highClearence,
								MATHEX_SCALAR lowClearence )
: MachActor( pRace, pPhysMobile, ot ),
  pMotionSeq_( _NEW( MachLogMachineMotionSequencer( this, pPhysMobile, 
  														MexPoint2d( position().x(), position().y() ), 
  														highClearence, lowClearence ) ) )
{
    PRE( pRace != NULL );
}

MachLogMobile::MachLogMobile( MachLogRace * pRace, 
								MachPhysMobile * pPhysMobile,
								MachLog::ObjectType ot,
								UtlId withId,
								MATHEX_SCALAR highClearence,
								MATHEX_SCALAR lowClearence )
: MachActor( pRace, pPhysMobile, ot, withId ),
  pMotionSeq_( _NEW( MachLogMachineMotionSequencer( this, pPhysMobile, 
  														MexPoint2d( position().x(), position().y() ), 
  														highClearence, lowClearence ) ) )
{
    PRE( pRace != NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////

MachLogMobile::~MachLogMobile()
{
    TEST_INVARIANT;

	_DELETE( pMotionSeq_ );
}

/* //////////////////////////////////////////////////////////////// */

MachLogMachineMotionSequencer& MachLogMobile::motionSeq()
{
	ASSERT( pMotionSeq_ != NULL, logic_error() );
	return *pMotionSeq_;
}

/* //////////////////////////////////////////////////////////////// */

const MachLogMachineMotionSequencer& MachLogMobile::motionSeq() const
{
	ASSERT( pMotionSeq_ != NULL, logic_error() );
	return *pMotionSeq_;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogMobile::doEndExplodingAnimation()
{
}
	
PhysRelativeTime MachLogMobile::beDestroyed()
{
	//HAL_STREAM(" (" << id() << ") MLMobile::beDestroyed\n" );
	PhysRelativeTime result = physMobile().beDestroyed();

    //Ensure the physical model sticks around for long enough to do the exploding animation
    preservePhysicalModel( result );

    //Add some debris
    dropDebris( SimManager::instance().currentTime() );

	return result;
}

/* //////////////////////////////////////////////////////////////// */

MachPhysMobile& MachLogMobile::physMobile()
{
	return _STATIC_CAST( MachPhysMobile&, physObject() );
}

const MachPhysMobile& MachLogMobile::physMobile() const
{
	return _STATIC_CAST( const MachPhysMobile&, physObject() );
}

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogMobile::turn( const MexRadians& radians )
{
	if( MachLogNetwork::instance().isNetworkGame() )
		MachLogNetwork::instance().messageBroker().sendMachineTurnMessage( physMobile().id(), radians );

	return physMobile().turn( radians );
}

PhysRelativeTime MachLogMobile::turnWithoutEcho( const MexRadians& radians )
{
	return physMobile().turn( radians );
}


/*
PhysRelativeTime MachLogMobile::move( const MexPoint3d& dest )
{
	return physMobile().move( dest );
}
*/
/* //////////////////////////////////////////////////////////////// */

void MachLogMobile::checkLeaderDestination()
{
    //Check we are following someone
    if( pMotionSeq_->isFollowing() )
    {
        //Tell the motion sequencer
        pMotionSeq_->leaderChangedDestination();

        //Ensure we get updated soon
        nextUpdateTime( SimManager::instance().currentTime() );
    }
}

void MachLogMobile::preservePhysicalModel( const PhysRelativeTime& forTime )
{
	MachLogMachine& mlm = asMachine();

    MachLogDyingEntityEvent::InsideBuilding insideBuilding;
    MachLogConstruction* pConstruction;
    
	if( mlm.insideBuilding() )
	{
        insideBuilding = MachLogDyingEntityEvent::INSIDE_BUILDING;
        pConstruction = &mlm.insideWhichBuilding();
	}
	else
	{
        insideBuilding = MachLogDyingEntityEvent::NOT_INSIDE_BUILDING;
        pConstruction = NULL;
	}

	MachLogDyingEntityEvent* pEvent =
	    _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, forTime, insideBuilding, pConstruction ) );
	SimManager::instance().add( pEvent );
}

void MachLogMobile::dropDebris( const PhysAbsoluteTime& )
{
	if( not MachLogNetwork::instance().isNetworkGame()   or
        MachLogNetwork::instance().remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		MexPoint3d debrisPosition;

		if( objectIsMachine() and asMachine().insideAPC() )
			debrisPosition = asMachine().APCImInside().position();
		else
			debrisPosition = position();

		MexAlignedBox3d machineBox;

		if( physObject().isComposite() )
			machineBox = physObject().asComposite().compositeBoundingVolume();

		else
			machineBox = physObject().boundingVolume();

	    const MexPoint2d minCorner( machineBox.minCorner().x(), machineBox.minCorner().y());
	    const MexPoint2d maxCorner( machineBox.maxCorner().x(), machineBox.maxCorner().y());

		// flying machines don't drop debris		
		if( not mobileIsGlider() )
		{
			MachLogActorMaker::newLogDebris( race(), asMachine().machineData().cost() / 2, position(),
		                                 MexAlignedBox2d( minCorner, maxCorner ) ); //MexAlignedBox2d( debrisPosition, 5 ) );			
		}
	}
}


void perWrite( PerOstream& ostr, const MachLogMobile& actor )
{
	const MachActor& base1 = actor;
	const MachLogCanMove& base2 = actor;
	const MachLogCanTurn& base3 = actor;

	ostr << base1;
	ostr << base2;
	ostr << base3;
}

void perRead( PerIstream& istr, MachLogMobile& actor )
{
	MachActor& base1 = actor;
	MachLogCanMove& base2 = actor;
	MachLogCanTurn& base3 = actor;

	istr >> base1;
	istr >> base2;
	istr >> base3;

	actor.pMotionSeq_ = NULL;

}

MachLogMobile::MachLogMobile( PerConstructor con )
:	MachActor( con )
{
}

void MachLogMobile::createNewMachineMotionSequencer( MachPhysMobile* pPhysMobile )
{
	PRE( not pMotionSeq_ );
	HAL_STREAM("(" << id() << ") MachLogMobile::createNewMachMotSeq at " << pPhysMobile->globalTransform() << std::endl );
	pMotionSeq_ = _NEW( MachLogMachineMotionSequencer( this, pPhysMobile, 
  														MexPoint2d( pPhysMobile->globalTransform().position().x(), pPhysMobile->globalTransform().position().y() ), 
  														asMachine().highClearence(), asMachine().lowClearence() ) );
}

bool MachLogMobile::machineMotionSequencerDefined() const
{
	return pMotionSeq_ != NULL;
}

MachLogMobile::ObstacleFlags   MachLogMobile::obstacleFlags() const
{
    ObstacleFlags   result = 0;
    
    //  Relies on the fact that MachPhysMobile is a typedef for MachPhysMachine.
    if( mobileIsGlider() )
    {
        result = MachLog::OBSTACLE_WATER | MachLog::OBSTACLE_LOW;
    }
    else if( physMobile().locomotionType() == MachPhys::HOVER )
    {
        result = MachLog::OBSTACLE_WATER;
    }
    
    return result;
}

bool MachLogMobile::mobileIsGlider() const
{
	return physMobile().locomotionType() == MachPhys::GLIDER;
}	

/* End MOBILE.CPP ***************************************************/


