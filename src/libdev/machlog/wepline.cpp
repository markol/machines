/*
 * W E P L I N E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation
#include "phys/motchunk.hpp" //Only needed for template instantiation
#include "sim/manager.hpp"
#include "world4d/domain.hpp"
#include "world4d/garbcoll.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepline.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/lineproj.hpp"

#include "machlog/actor.hpp"
#include "machlog/canattac.hpp"
#include "machlog/constron.hpp"
#include "machlog/lineproj.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/misshold.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/race.hpp"
#include "machlog/wepline.hpp"
#include "machlog/missilem.hpp"

#include "machlog/internal/firedata.hpp"

//added for gun targetting.
//#include "machlog/constron.hpp"
//#include "machlog/machine.hpp"
//#include "phys/cspace2.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogLinearWeapon );


MachLogLinearWeapon::MachLogLinearWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogWeapon( pRace, pPhysWeapon, pOwner ),
	pPhysLinearWeapon_( pPhysWeapon )
{

    TEST_INVARIANT;
}

MachLogLinearWeapon::~MachLogLinearWeapon()
{
    TEST_INVARIANT;

}

void MachLogLinearWeapon::setPhysicalLinearWeapon( MachPhysLinearWeapon* pPhysWeapon )
{
	pPhysLinearWeapon_ = pPhysWeapon;
}

void MachLogLinearWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachPhysLinearWeapon& MachLogLinearWeapon::physLinearWeapon()
{
	return *pPhysLinearWeapon_;
}

//virtual
void MachLogLinearWeapon::doFire( const MexPoint3d& position )
{
	//HAL_STREAM("MLLinearWeapon::doFire at " << pTarget->id() << std::endl );
	//create Linear blobs (the logical entites here).
	MachLogNetwork& network = MachLogNetwork::instance();
	bool isNetworkGame = network.isNetworkGame();
	PhysAbsoluteTime launchTime = SimManager::instance().currentTime();
	//HAL_STREAM(" rounds per burst " << physWeapon().weaponData().nRoundsPerBurst() << std::endl );
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;
	MexPoint3d startPosition( owner().position() );
    W4dDomain* pParent = MachLogPlanetDomains::pDomainPosition( startPosition, zAngle, &localTransform );
    if( owner().objectIsMachine()
    	and &owner().asMachine().motionSeq().currentConfigSpace() != &MachLogPlanet::instance().configSpace()
    	and owner().asMachine().insideBuilding() )
    {
		pParent = _CONST_CAST( W4dDomain*, &owner().asMachine().insideWhichBuilding().interiorDomain() );
	}

	MachLogMessageBroker::ProjectileDestroyData extraData;

	//get domain and transform to use
    MexTransform3d localTileTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( position, 0, &localTileTransform );

	for( int i = 0; i < physWeapon().weaponData().nRoundsPerBurst(); ++i )
	{
		//HAL_STREAM(" create new Linear blob\n" );
		//PhysAbsoluteTime startTime = Phys::time();
		MachLogLinearProjectile* pLogProj = createLinearProjectile( launchTime, i, pParent, *pDomain, localTileTransform.position() );
		if( isNetworkGame )
		{
			MachLogMessageBroker::SingleProjectileDestroyData* pDestroyData = _NEW( MachLogMessageBroker::SingleProjectileDestroyData );
			pDestroyData->destroyAtTime_ = pLogProj->destructionTime();
			pDestroyData->strikeType_ = pLogProj->strikeType();
			extraData.push_back( pDestroyData );
		}
		//PhysAbsoluteTime endTime = Phys::time();
		//DEBUG_STREAM( DIAG_MISC, owner().id() << "," << physWeapon().type() << ",createLinearProjectile," << endTime - startTime << std::endl );
		//HAL_STREAM(" create new Linear blob done\n" );
	}
	if( isNetworkGame and network.remoteStatus( owner().race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendEchoLinearProjectileMessage( owner().id(), mounting(), 0, launchTime, physWeapon().weaponData().nRoundsPerBurst(), extraData, position );
		while( extraData.size() )
		{
			_DELETE( extraData.back() );
			//unfortunatley ctl_pvectors do not have rend hence this slightly convuluted looking erase.
			extraData.erase( extraData.begin() + extraData.size() - 1 );
		}
	}
	//HAL_STREAM("MLLinearWeapon::doFire exit " << std::endl );
}

//virtual
void MachLogLinearWeapon::doFire( MachActor* pTarget, const MachLogFireData& fireData )
{
	ASSERT( fireData.burstSeed != MachLogWeapon::DUMMY_SEED, "MachLogLinearWeapon::doFire has received a dummy fireData struct as parameter." );

	//HAL_STREAM("MLLinearWeapon::doFire at " << pTarget->id() << std::endl );
	//create Linear blobs (the logical entites here).
	MachLogNetwork& network = MachLogNetwork::instance();
	bool isNetworkGame = network.isNetworkGame();
	PhysAbsoluteTime launchTime = SimManager::instance().currentTime();
	//HAL_STREAM(" rounds per burst " << physWeapon().weaponData().nRoundsPerBurst() << std::endl );
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;
	MexPoint3d startPosition( owner().position() );
    W4dDomain* pParent = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );
	if( owner().objectIsMachine() and &owner().asMachine().motionSeq().currentConfigSpace() != &MachLogPlanet::instance().configSpace() )
	{
		pParent = _CONST_CAST( W4dDomain*, &owner().asMachine().insideWhichBuilding().interiorDomain() );
	}

	MachLogMessageBroker::ProjectileDestroyData extraData;
	const MachActor& cactor = *pTarget;

	MATHEX_SCALAR distanceToTarget = startPosition.euclidianDistance( cactor.position() );

	// TBD!!! Need speed of weapon
	PhysRelativeTime approxTimeToTarget = distanceToTarget / physWeapon().weaponData().projectileSpeed();

	const W4dEntity& targetEntity = cactor.physObject();

	//MexPoint3d originalTargetOffset = cactor.predictedTargetOffset( approxTimeToTarget );
	MexPoint3d originalTargetOffset = _CONST_CAST(MachActor&, cactor).predictedTargetOffset( approxTimeToTarget );

	MexPoint3d finalOffsetPoint;

	// is this a completely accurate shot?

	if ( fireData.missed == true )
	{
		// not a completely accurate shot - have to work out the inaccuracy drift

		MexTransform3d weaponToTarget;
		physWeapon().globalTransform().transformInverse( targetEntity.globalTransform(), &weaponToTarget );

		MexTransform3d targetToWeapon;
		weaponToTarget.invert( &targetToWeapon );

		MexPoint3d accurateOffsetPoint( originalTargetOffset );
		weaponToTarget.transform( &accurateOffsetPoint );

		MexVec3 weaponToOffsetPoint( accurateOffsetPoint );
		MATHEX_SCALAR distanceToOffsetPoint = weaponToOffsetPoint.modulus();

		weaponToOffsetPoint.makeUnitVector();

		// using firstDrift as y-drift and secondDrift as z-drift
		// z drift is (on average) less than half y-drift, creating a flattened cone of inaccuracy
		// Note that there is a safety threshold to prevent shooting the ground too near to yourself.

		MATHEX_SCALAR yDrift = fireData.firstDrift / 150.0;
		MATHEX_SCALAR zDrift = std::max( -0.05, (fireData.secondDrift / 300.0 ) );

		// SPECIAL CASE HACK LOGIC FOR HORRIBLE LEVIATHANS
		if( owner().objectIsMissileEmplacement() and owner().asMissileEmplacement().subType() == MachPhys::LAUNCHER )
		{
			MachLogCanAttack::Weapons& weapons = owner().asCanAttack().weapons();
			ASSERT( weapons.size() == 2, "Special-case hack compromised by change to number of weapons on LAUNCHER 4." );

			if( this == weapons.front() )
			{
				// this is the right-mounted launcher
				MATHEX_SCALAR limbo = yDrift;
				yDrift = -zDrift;
				zDrift = limbo;
			}
			else
			{
				// this is the left-mounted launcher
				MATHEX_SCALAR limbo = yDrift;
				yDrift = zDrift;
				zDrift = -limbo;
			}
		}

		MexVec3 inaccuracyVector( 0, yDrift, zDrift  );

		weaponToOffsetPoint += inaccuracyVector;

		weaponToOffsetPoint.makeUnitVector();

		weaponToOffsetPoint *= distanceToOffsetPoint;

		finalOffsetPoint = weaponToOffsetPoint;
		targetToWeapon.transform( &finalOffsetPoint );
	}
	else
		// completely accurate shot
		finalOffsetPoint = originalTargetOffset;



	// --------------------------------------------------

	int numberInBurst;

	if( physWeapon().weaponData().fixedBurst() )
		numberInBurst = physWeapon().weaponData().nRoundsPerBurst();
	else
		numberInBurst = (int)( fireData.burstSeed *(float)( physWeapon().weaponData().nRoundsPerBurst() ) ) + 1;

	for( int i = 0; i < numberInBurst; ++i )
	{
		//HAL_STREAM(" create new Linear blob\n" );
		//PhysAbsoluteTime startTime = Phys::time();
		MachLogLinearProjectile* pLogProj = createLinearProjectile( launchTime, i, pParent, cactor.physObject(), finalOffsetPoint );
		if( isNetworkGame )
		{
			MachLogMessageBroker::SingleProjectileDestroyData* pDestroyData = _NEW( MachLogMessageBroker::SingleProjectileDestroyData );
			pDestroyData->destroyAtTime_ = pLogProj->destructionTime();
			pDestroyData->strikeType_ = pLogProj->strikeType();
			extraData.push_back( pDestroyData );
		}
		//PhysAbsoluteTime endTime = Phys::time();
		//DEBUG_STREAM( DIAG_MISC, owner().id() << "," << physWeapon().type() << ",createLinearProjectile," << endTime - startTime << std::endl );
		//HAL_STREAM(" create new Linear blob done\n" );
	}
	if( isNetworkGame and network.remoteStatus( owner().race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendEchoLinearProjectileMessage( owner().id(), mounting(), cactor.id(), launchTime, numberInBurst, extraData, finalOffsetPoint );
		while( extraData.size() )
		{
			_DELETE( extraData.back() );
			//unfortunatley ctl_pvectors do not have rend hence this slightly convuluted looking erase.
			extraData.erase( extraData.begin() + extraData.size() - 1 );
		}
	}
	//HAL_STREAM("MLLinearWeapon::doFire exit " << std::endl );
}


void MachLogLinearWeapon::doEchoFire( MachActor* pTarget, MexPoint3d targetOffset, const PhysAbsoluteTime& launchTime, int nRounds, const MachLogMessageBroker::ProjectileDestroyData& extraData, MachLogLinearWeapon::CreateHolder createHolder )
{
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;
	MexPoint3d startPosition( owner().position() );
    W4dDomain* pParent = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );
	if( owner().objectIsMachine() and &owner().asMachine().motionSeq().currentConfigSpace() != &MachLogPlanet::instance().configSpace() )
	{
		pParent = _CONST_CAST( W4dDomain*, &owner().asMachine().insideWhichBuilding().interiorDomain() );
	}

	const MachActor& cactor = *pTarget;
	for( int i = 0; i < nRounds; ++i )
	{
		MachPhysLinearProjectile* pPhysLinearProjectile = createPhysLinearProjectile( launchTime, i, pParent, cactor.physObject(), targetOffset );
        PhysAbsoluteTime destructTime = extraData[ i ]->destroyAtTime_;
		PhysRelativeTime relTime = pPhysLinearProjectile->beDestroyedAt( destructTime, extraData[i]->strikeType_ );
		if( createHolder == MachLogLinearWeapon::CREATE_HOLDER )
		{
			_NEW( MachLogMissileHolder( &MachLogRaces::instance().race( owner().race() ), pPhysLinearProjectile, destructTime ) );
		}
		else
			W4dGarbageCollector::instance().add( pPhysLinearProjectile, relTime + destructTime );

        //Set up intersect relations for the projectile
        const MexLine3d& flightPath = pPhysLinearProjectile->flightPath();
        MachLogPlanetDomains::addIntersectRelations( pPhysLinearProjectile, flightPath.end1(), flightPath.end2() );
	}
}

void MachLogLinearWeapon::doEchoFireForTerrain( MexPoint3d position, const PhysAbsoluteTime& launchTime, int nRounds, const MachLogMessageBroker::ProjectileDestroyData& extraData, MachLogLinearWeapon::CreateHolder createHolder )
{
    //get domain and transform to use
    MexTransform3d localTransform;

    const MexRadians    zAngle = 0.0;
	MexPoint3d startPosition( owner().position() );
    W4dDomain* pParent = MachLogPlanetDomains::pDomainPosition(
      startPosition, zAngle, &localTransform );
	if( owner().objectIsMachine() and &owner().asMachine().motionSeq().currentConfigSpace() != &MachLogPlanet::instance().configSpace() )
	{
		pParent = _CONST_CAST( W4dDomain*, &owner().asMachine().insideWhichBuilding().interiorDomain() );
	}

	//get domain and transform to use
    MexTransform3d localTileTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( position, 0, &localTileTransform );

	for( int i = 0; i < nRounds; ++i )
	{
		MachPhysLinearProjectile* pPhysLinearProjectile = createPhysLinearProjectile( launchTime, i, pParent, *pDomain, localTileTransform.position() );
        PhysAbsoluteTime destructTime = extraData[ i ]->destroyAtTime_;
		PhysRelativeTime relTime = pPhysLinearProjectile->beDestroyedAt( destructTime, extraData[i]->strikeType_ );
		if( createHolder == MachLogLinearWeapon::CREATE_HOLDER )
		{
			_NEW( MachLogMissileHolder( &MachLogRaces::instance().race( owner().race() ), pPhysLinearProjectile, destructTime ) );
		}
		else
			W4dGarbageCollector::instance().add( pPhysLinearProjectile, relTime + destructTime );

        //Set up intersect relations for the projectile
        const MexLine3d& flightPath = pPhysLinearProjectile->flightPath();
        MachLogPlanetDomains::addIntersectRelations( pPhysLinearProjectile, flightPath.end1(), flightPath.end2() );
	}
}


MachPhysLinearProjectile* MachLogLinearWeapon::createPhysLinearProjectile(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	//HAL_STREAM("MLLinearWeapon::createPhysLinearProjectile " << burstStartTime << " index " << index << "\n" );
	//dtlw("", true );
	MachPhysLinearProjectile* result = pPhysLinearWeapon_->createProjectile( burstStartTime, index, pParent, target, targetOffset );
	//dtlw("MLLinearWeapon:: createPhysLinearProjectile", false );

    //Give it the planet surface pointer for any special effects
    result->planetSurface( MachLogPlanet::instance().surface() );

	return result;
}



ostream& operator <<( ostream& o, const MachLogLinearWeapon& t )
{

    o << "MachLogLinearWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogLinearWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogLinearWeapon::dtlw( const char* text, bool start )
{
	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( start )
		startTime = Phys::time();
	else
	{
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_MISC, owner().id() << "," << physWeapon().type() << "," << text << "," << endTime - startTime << std::endl );
	}
}

/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogLinearWeapon& weapon )
{
	const MachLogWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogLinearWeapon& weapon )
{
	MachLogWeapon& base1 = weapon;

	istr >> base1;
}

MachLogLinearWeapon::MachLogLinearWeapon( PerConstructor con )
:	MachLogWeapon( con )
{
}

/* End WEPLINE.CPP *************************************************/
