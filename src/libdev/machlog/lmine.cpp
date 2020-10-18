/*
 * L M I N E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/domain.hpp"
#include "machphys/lmine.hpp"
#include "machphys/consdata.hpp"
#include "machphys/mphydata.hpp"
#include "sim/manager.hpp"

#include "machlog/lmine.hpp"
#include "machlog/machine.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/lineproj.hpp"
#include "machlog/dyingent.hpp"

PER_DEFINE_PERSISTENT( MachLogLandMine );
//////////////////////////////////////////////////////////////////////////////////////////

MachLogLandMine::MachLogLandMine
(
    MachLogRace* pRace, const MexPoint3d& location
)
:   MachActor( pRace, pNewLandMine( pRace, location), MachLog::LAND_MINE )
{
	hp(0);
	armour(0);
	HAL_STREAM("MLLandMine::CTOR\n" );
    TEST_INVARIANT;
	pPhysLandMine_ = (MachPhysLandMine*)&physObject();

}

MachLogLandMine::MachLogLandMine
(
    MachLogRace* pRace, const MexPoint3d& location, UtlId withId
)
:   MachActor( pRace, pNewLandMine( pRace, location), MachLog::LAND_MINE, withId )
{
	hp(0);
	armour(0);
	HAL_STREAM("MLLandMine::CTOR\n" );
    TEST_INVARIANT;
	pPhysLandMine_ = (MachPhysLandMine*)&physObject();

}
//////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhysLandMine* MachLogLandMine::pNewLandMine
(
    MachLogRace* pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the holograph
    return _NEW( MachPhysLandMine( pDomain, localTransform, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogLandMine::~MachLogLandMine()
{
	HAL_STREAM("MLLandMine::DTOR\n" );
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogLandMine::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogLandMine::beDestroyed()
{
	PhysRelativeTime result = pPhysLandMine_->beDestroyed() * 1.05;
	SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, result, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//virtual
void MachLogLandMine::doStartExplodingAnimation()
{
}

//virtual
void MachLogLandMine::doEndExplodingAnimation()
{
}

//virtual
const MachPhysObjectData& MachLogLandMine::objectData() const
{
	ASSERT(false,"Called objectData for a landMine object\n" );
	return (MachPhysObjectData&)MachPhysData::instance().oreHolographData();
}

//virtual
void MachLogLandMine::doVisualiseSelectionState()
{
	//Do nothing - they can't be selected
}

//virtual
PhysRelativeTime MachLogLandMine::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR junk )
{
	PhysRelativeTime guaranteedMinTime = 2.5;

	if( not isDead() )
	{
		MachLogRaces& races = MachLogRaces::instance();

		MATHEX_SCALAR sqrClosestFound = 999999999;

//		race() == MachPhys::RED ? targetRace = MachPhys::BLUE : targetRace = MachPhys::RED;
		for( MachPhys::Race ridx = MachPhys::RED;
			 not isDead() and ridx != MachPhys::N_RACES;
			 ++ridx )
		{
			MachLogRaces::DispositionToRace disp = races.dispositionToRace( race(), ridx );
			if( disp != MachLogRaces::ALLY and
				disp != MachLogRaces::OUR_RACE )
			{
				for( MachLogRaces::Objects::iterator i = races.raceObjects( ridx ).begin();
					 not isDead() and i != races.raceObjects( ridx ).end();
					 ++i )
				{
					if( (*i)->objectIsMachine() )
					{
						MachLogMachine& machine = (*i)->asMachine();

						if( not( machine.machineIsGlider() )
							and not machine.isDead() )
						{
							MATHEX_SCALAR sqrDistanceToMachine = position().sqrEuclidianDistance( machine.position() );
							if( sqrDistanceToMachine < sqrClosestFound )
							{
								sqrClosestFound = sqrDistanceToMachine;
							}

							// detonate if within 5m of machine UNLESS another landmine is closer
							if( sqrDistanceToMachine < sqr( machine.highClearence() + 5 ) )
							{
								// well, we're certainly close enough. But is there another landmine that's actually closer?
								if( thisIsClosestLandmineTo( machine.position() ) )
								{
									MachActor::beHit( 10 );
									isDead( true );
									machine.beHit( 80 );
//									MachLogLinearProjectile::genericCheckForDamage( position(), 10, MachLogLinearProjectile::CONSTANT_DAMAGE, 80, MachPhys::PULSE_RIFLE );
									break;
								}
  							}
						}
					}
				}
			}
		}

		// shorten callback time if enemy machines are close
		if( sqrClosestFound < 10000 )		  // within 100m?
		{
			if( sqrClosestFound < 900 )		  // within 30m?
				guaranteedMinTime = 0.2;
			else
				guaranteedMinTime = 0.7;
		}

	}
	MachActor::update( maxCPUTime, junk );

	return guaranteedMinTime;
}

//virtual
void MachLogLandMine::beHit( const int& /*damage*/, MachPhys::WeaponType /*byType*/ )
{
	if( not isDead() )
	{
		//MachLogLinearProjectile::genericCheckForDamage( position(), 1, MachLogLinearProjectile::CONSTANT_DAMAGE, 80, MachPhys::BOLTER );
		MachActor::beHit( 10 );
		isDead( true );
	}
}

// virtual
void MachLogLandMine::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
		//MachLogLinearProjectile::genericCheckForDamage( position(), 1, MachLogLinearProjectile::CONSTANT_DAMAGE, 80, MachPhys::BOLTER );
		MachActor::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );
		isDead( true );
	}
}

void perWrite( PerOstream& ostr, const MachLogLandMine& actor )
{
	const MachActor& base1 = actor;

	ostr << base1;
	ostr << actor.damage_;
	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogLandMine& actor )
{
	MachActor& base1 = actor;

	istr >> base1;
	istr >> actor.damage_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	actor.pPhysLandMine_ = MachLogLandMine::pNewLandMine( &actor.logRace(), trans.position() );
	actor.setObjectPtr( actor.pPhysLandMine_, trans );
	actor.id( id );
	actor.actorCreated();
	actor.persistenceReadStrategy( istr );

}

MachLogLandMine::MachLogLandMine( PerConstructor con )
:	MachActor( con )
{
}

bool MachLogLandMine::thisIsClosestLandmineTo( const MexPoint2d& targetPosition ) const
{
	MexPoint2d myPosition = position();
	MATHEX_SCALAR myDistanceToPosition = myPosition.sqrEuclidianDistance( targetPosition );

	bool nothingCloserFound = true;

	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race myRace = race();

	for( MachLogRaces::Objects::iterator i = races.raceObjects( myRace ).begin();
		 nothingCloserFound and i != races.raceObjects( myRace ).end();
		 ++i )
	{
		if( (*i)->objectIsLandMine() )
		{
			MachLogLandMine& landMine = (*i)->asLandMine();
			MexPoint2d minePosition = (*i)->position();
			if( minePosition.sqrEuclidianDistance( targetPosition ) < myDistanceToPosition )
			{
				nothingCloserFound = false;
			}
		}
	}

	return nothingCloserFound;
}

/* End OREHOLO.CPP **************************************************/
