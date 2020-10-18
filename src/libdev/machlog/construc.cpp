/*
 * C O N S T R U C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"

#include "world4d/domain.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "machphys/construc.hpp"
#include "machphys/machdata.hpp"
#include "machphys/ctordata.hpp"

#include "machlog/constree.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"

#include "world4d/soundman.hpp"


/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogConstructor );

MachLogConstructor::MachLogConstructor( const MachPhys::ConstructorSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location )
: MachLogMachine( MachPhys::CONSTRUCTOR, hwLevel, swLevel, pRace,
                  pNewPhysConstructor( subType, hwLevel, swLevel, pRace, location ),
                  MachLog::CONSTRUCTOR ),
	subType_( subType ),
	isDoingSuperConstruct_( false ),
	isConstructing_( false )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces& races = MachLogRaces::instance();
	ctl_append( &races.constructors( pRace->race() ), this );
	races.constructionTree().updateAvailableConstructions( pRace->race(),
																			  data().mostAdvancedConstructionType(),
																			  data().mostAdvancedSubType(),
																			  data().mostAdvancedConstructionLevel() );
}

MachLogConstructor::MachLogConstructor( const MachPhys::ConstructorSubType& subType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, UtlId withId )
: MachLogMachine( MachPhys::CONSTRUCTOR, hwLevel, swLevel, pRace,
                  pNewPhysConstructor( subType, hwLevel, swLevel, pRace, location ),
                  MachLog::CONSTRUCTOR,
                  withId ),
	subType_( subType ),
	isDoingSuperConstruct_( false ),
	isConstructing_( false )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces& races = MachLogRaces::instance();
	ctl_append( &races.constructors( pRace->race() ), this );
	races.constructionTree().updateAvailableConstructions( pRace->race(),
																			  data().mostAdvancedConstructionType(),
																			  data().mostAdvancedSubType(),
																			  data().mostAdvancedConstructionLevel() );
}




MachLogConstructor::~MachLogConstructor()
{
	/* Intentionally Empty	*/
}

void MachLogConstructor::constructing( bool newIsConstructingValue, UtlId constructionId )
{
	// do nothing if no change in isConstructing status
	if( newIsConstructingValue == isConstructing_ )
		return;

	isConstructing_ = newIsConstructingValue;

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
		network.messageBroker().sendPlayConstructAnimationMessage( id(), constructionId, isConstructing_ );

	if( isConstructing_ )
	{
		MachActor& constructionActor = MachLogRaces::instance().actor( constructionId );
		ASSERT( constructionActor.objectIsConstruction(), "constructionId should be the ID of a construction." );

		physConstructor().startConstructing( constructionActor.asConstruction().physConstruction(), SimManager::instance().currentTime() );

		W4dEntity& physObj = physObject();
 		W4dSoundManager::instance().stop(&physObj);
		W4dSoundManager::instance().play(&physObj, SID_CONSTRUCTING, PhysAbsoluteTime(0), 0);

	}
	else
	{
		physConstructor().stopConstructing();

		SoundId idleId;
   		SoundId moveId;
		physMachine().getLocomotionSounds(&idleId, &moveId, physMachine().machineData());
		W4dEntity& physObj = physObject();
		W4dSoundManager::instance().stop(&physObj);
		W4dSoundManager::instance().play(&physObj, idleId, PhysAbsoluteTime(0), 0);
	}
}

PhysRelativeTime MachLogConstructor::pickUp()
{
	return 1.0;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogConstructor::doOutputOperator( ostream& o ) const
{
	MachLogMachine::doOutputOperator( o );
	o << " Construction Rate " << data().constructionRate() << std::endl;
	if( isConstructing_ )
		o << " Is Constructing " << std::endl;
	if( isDoingSuperConstruct_ )
		o << " Has further superconstruct tasks after this one " << std::endl;
}

/* //////////////////////////////////////////////////////////////// */
//static
MachPhysConstructor* MachLogConstructor::pNewPhysConstructor
(
    const MachPhys::ConstructorSubType& subType, Level hwLevel, Level swLevel, MachLogRace * pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysConstructor( pDomain, localTransform, subType, hwLevel, swLevel, pRace->race() ) );
}
/* //////////////////////////////////////////////////////////////// */

const MachPhysConstructor& MachLogConstructor::physConstructor() const
{
    return _STATIC_CAST( const MachPhysConstructor&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysConstructor& MachLogConstructor::physConstructor()
{
    return _STATIC_CAST( MachPhysConstructor&, physObject() );
}
//virtual
const MachPhysMachineData& MachLogConstructor::machineData() const
{
	return data();
}

const MachPhysConstructorData& MachLogConstructor::data() const
{
	//return  _STATIC_CAST( MachPhysConstructorData&, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysConstructorData&, physMachine().machineData() );
}

const MachPhys::ConstructorSubType& MachLogConstructor::subType() const
{
	return subType_;
}

void MachLogConstructor::isDoingSuperConstruct( bool status )
{
	isDoingSuperConstruct_ = status;
}

bool MachLogConstructor::isDoingSuperConstruct() const
{
	return isDoingSuperConstruct_;
}







///////////////////////////////////////////////////// persistence ////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogConstructor& actor )
{
	HAL_STREAM("MachLogConstructor::perWrite\n ");
	const MachLogMachine& base1 = actor;

	HAL_STREAM("MachLogConstructor::perWrite MLMachine \n ");
	ostr << base1;
	HAL_STREAM("MachLogConstructor::perWrite subType\n ");
	ostr << actor.subType_;
	ostr << actor.isConstructing_;
	ostr << actor.isDoingSuperConstruct_;

	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	HAL_STREAM("MachLogConstructor::perWrite global transfrom \n " << trans << std::endl );
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	HAL_STREAM("MachLogConstructor::perWrite id " << id << "\n ");
	PER_WRITE_RAW_OBJECT( ostr, id );

	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogConstructor& actor )
{
	MachLogMachine& base1 = actor;

	HAL_STREAM("MachLogConstructor::perRead MLMachine\n ");
	istr >> base1;
	HAL_STREAM("MachLogConstructor::perRead subType\n ");
	istr >> actor.subType_;
	istr >> actor.isConstructing_;
	istr >> actor.isDoingSuperConstruct_;


	MexTransform3d trans;
	W4dId id;
	HAL_STREAM("MachLogConstructor::perRead trans\n ");
	PER_READ_RAW_OBJECT( istr, trans );
	HAL_STREAM("MachLogConstructor::perRead id\n ");
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysConstructor* pPhysConstructor = MachLogConstructor::pNewPhysConstructor( actor.subType_, base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position() );
	actor.setObjectPtr( pPhysConstructor, trans );
	actor.id( id );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogConstructor::MachLogConstructor( PerConstructor con )
:	MachLogMachine( con )
{
}

bool MachLogConstructor::constructing() const
{
	return isConstructing_;
}

bool MachLogConstructor::constructingMissileEmplacement() const
{
	bool result = false;
	if( not strategy().queueIsEmpty() )
	{
		const MachLogOperation& operation =  strategy().operationCurrentlyExecuting();

		result = operation.operationType() == MachLogOperation::CONSTRUCT_OPERATION
				 //and _STATIC_CAST( MachLogConstructOperation&, operation ).constructingMissileEmplacement();
				 and _STATIC_CAST( const MachLogConstructOperation&, operation ).constructingMissileEmplacement();
	}

	return result;
}


//////////////////////////////////////////////////////////////////////////////////////////
/* End CONSTRUC.CPP *************************************************/
