/*
 * FACTORY . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/cvexpgon.hpp"

#include "utility/linetok.hpp"

#include "phys/phys.hpp"
#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "world4d/domain.hpp"
#include "sim/manager.hpp"

#include "machphys/factory.hpp"
#include "machphys/factdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/aggrdata.hpp"
#include "machphys/admndata.hpp"
#include "machphys/ctordata.hpp"
#include "machphys/glocdata.hpp"
#include "machphys/slocdata.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/apcdata.hpp"
#include "machphys/techdata.hpp"
#include "machphys/random.hpp"


#include "machlog/factory.hpp"
#include "machlog/internal/factoryi.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/machine.hpp"
#include "machlog/machlog.hpp"
#include "machlog/machvman.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/move.hpp"
#include "machlog/network.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/produnit.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/resitem.hpp"
#include "machlog/scenario.hpp"
#include "machlog/spacial.hpp"
#include "machlog/stats.hpp"
#include "machlog/cntrl.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>


#ifndef _INLINE
    #include "machlog/factory.ipp"
#endif

#define CB_MachLogFactory_DEPIMPL()	\
		PRE( pImpl_ )	\
		CB_DEPIMPL( MachLogFactory::ProductionLine		,productionLine_ ) \
		CB_DEPIMPL( MachPhys::BuildingMaterialUnits		,amountBuilt_ ) \
		CB_DEPIMPL( PhysAbsoluteTime					,lastUpdateTime_ ) \
		CB_DEPIMPL( MachPhys::FactorySubType 			,subType_ ) \
		CB_DEPIMPL( MexPoint2d 							,specifiedDeployPoint_ ) \
		CB_DEPIMPL( bool								,deployPointIsSet_ );


//////////////////////////////////////////////////////////////////////////////////////////
PER_DEFINE_PERSISTENT( MachLogFactory );

MachLogFactory::MachLogFactory( MachPhys::FactorySubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle )
:   MachLogConstruction( pRace, pNewPhysFactory( subType, pRace, level, location, angle ),
						 MachLog::FACTORY,
                         MachPhysData::instance().factoryData( subType, level ) ),
	pImpl_( _NEW( MachLogFactoryImpl( subType ) ) )
{

    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().factories( pRace->race() ).push_back( this );

}

MachLogFactory::MachLogFactory( MachPhys::FactorySubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysFactory( subType, pRace, level, location, angle ),
						 MachLog::FACTORY,
						 withId,
                         MachPhysData::instance().factoryData( subType, level ) ),
	pImpl_( _NEW( MachLogFactoryImpl( subType ) ) )
{

    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().factories( pRace->race() ).push_back( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogFactory::~MachLogFactory()
{
	//HAL_STREAM("(" << id() << ") MLFactory::DTOR\n" );
	CB_MachLogFactory_DEPIMPL();
	if( not productionLine_.empty() )
		clearProductionLine();
	_DELETE( pImpl_ );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogFactory::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogFactory::doOutputOperator( ostream& o ) const
{
	CB_MachLogFactory_DEPIMPL();
    o << "(" << id() << ") " << objectType() << " " << (void*)this << std::endl;
	o << " sub Type " << subType() << std::endl;
	MachLog::ObjectType 	type;
	int 					subType;
	MachLogMachine::Level 	hwLevel;
	MachLogMachine::Level 	swLevel;
	MachPhys::WeaponCombo	wc;
	o << " ProductionLine.size() " << productionLine_.size() << std::endl;
	o << " Build rate BMU/s " << data().buildRate() / 60 << std::endl;
	o << " Build rate BMU/min " << data().buildRate() << std::endl;

	bool building = currentlyBuilding( type, subType, hwLevel, swLevel, wc );
	o << " Currently building " << ( building ? "TRUE" : "FALSE");

	if( building )
	{
		o << " " << type << " sub: " << subType << " HW " << hwLevel << " SW " << swLevel << " wc " << wc << std::endl;
		o << " % " << percentComplete() << std::endl;
		o << " Amount Built " << amountBuilt() << std::endl;
		o << " current Total Cost " << currentTotalCost() << std::endl;
	}
	else
		o << std::endl;
	o << "Build Items " << buildItems().size() << std::endl;
    for( MachLogResearchTree::ResearchItems::const_iterator i = buildItems().begin(); i != buildItems().end(); ++i)
    {
            if( not (*i)->researched( race() ) )
                    o << "(NR) ";
            if( not (*i)->available( race() ) )
                    o << "(NA) ";
            o << (*i)->objectType() << " sub " << (*i)->subType() << " hw " << (*i)->hwLevel() << " wc " << (*i)->weaponCombo() << std::endl;
    }
	if( deployPointIsSet_ )
	{
		o << "Set deploy point is " << specifiedDeployPoint_ << std::endl;
	}
	else
		o << "No deploy point set." << std::endl;

	MachActor::doOutputOperator( o );
}

//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogFactory& t )
{
	t.doOutputOperator( o );
    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysFactory* MachLogFactory::pNewPhysFactory
(
    MachPhys::FactorySubType subType, MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysFactory( pDomain, localTransform, subType, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogFactory::createNewMachine( const MachLogProductionUnit& newMachine, const MexPoint2d& entranceInternalPoint, const MexPoint2d& entranceExternalPoint, MachLogRace * pRace )
{
	CB_MachLogFactory_DEPIMPL();
	MachPhys::Race race = pRace->race();
	MexPoint3d pos( entranceExternalPoint.x(), entranceExternalPoint.y(), 0 );

	#ifndef NDEBUG
	MexVec3 xBasis3;
	globalTransform().xBasis( &xBasis3 );
	MexVec2 xBasis2( xBasis3 );
	MexVec2 directionToPos( pos.x() - position().x() , pos.y() - position().y() );
	ASSERT_INFO( globalTransform() );
	ASSERT_INFO( pos );
	ASSERT_INFO( xBasis2 );
	ASSERT_INFO( fabs( xBasis2.angleBetween( directionToPos ).asScalar() ) );
	ASSERT( fabs( xBasis2.angleBetween( directionToPos ).asScalar() ) < Mathex::PI_DIV_2, "The angle from the factory xbasis to the creation point mag > 90 degrees\n" );
	#endif
	//HAL_STREAM("MLFactory::creatNewmachine\n" );
	//HAL_STREAM(" " << newMachine.type << std::endl );
	MachLogMachine* pMach = NULL;
	pMach = MachLogActorMaker::newLogMachine( newMachine.type(), newMachine.subType(),newMachine.hwLevel(), newMachine.swLevel(), pRace->race() , pos, newMachine.weaponCombo() );

	if( displayMapAndIconRace() != race )
		pMach->displayAsRace( displayMapAndIconRace() );

	pMach->teleportIntoWorld();
	//if this is an AI Race then if the constructionId field is set then it represents the id
	//of the squadron id to add it to.
	if( newMachine.constructionId() != -1 and MachLogRaces::instance().controller( pRace->race() ).type() == MachLogController::AI_CONTROLLER )
		pMach->squadron( MachLogRaces::instance().squadrons( pRace->race() )[ newMachine.constructionId() ] );

	// give voicemail announcing arrival into world
	MachLogMachineVoiceMailManager::instance().postNewMail( pMach->objectType(), pMach->subType(), MachLogMachineVoiceMailManager::MEV_BUILT, pMach->id(),  pMach->race() );

	// Now make the little fella beetle off to a deploy point.
	moveToDeployPoint( pMach, entranceInternalPoint, entranceExternalPoint );

		// and update the race "machines built" counter
	MachLogRaces::instance().machinesBuiltScoreAdjust( race, *pMach );
}

//virtual
PhysRelativeTime MachLogFactory::update( const PhysRelativeTime&, MATHEX_SCALAR )
{
	CB_MachLogFactory_DEPIMPL();
    bool complete = isComplete();
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	PhysRelativeTime interval = 10.0;

    if( not updateCompletionVisualisation() )
	{
		interval = 0.1;
		pPhysFactory()->isWorking( false );
	}

	if( complete )
	{
		if( productionLine_.empty() )
		{
			lastUpdateTime_ = timeNow;
			if( pPhysFactory()->isWorking() )
			{
					pPhysFactory()->isWorking( false );
				if( MachLogNetwork::instance().isNetworkGame() )
					MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), false );
			}
			interval = 6.0;
		}
		else
		{
            //The physical working animation needs to be refreshed periodically, so make the call every time

			pPhysFactory()->isWorking( true );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );

			MachLogProductionUnit* current = *productionLine_.begin();
			MachPhys::BuildingMaterialUnits cost = currentTotalCost();

			int roundedOffAmountToAdd = 0;

			if( current->amountBuilt() < cost )
			{
				//check for storage here....
				float amountToAdd = ( timeNow - lastUpdateTime_ ) * data().buildRate() / 60.0;
				float remainingCost = _STATIC_CAST( float, cost - current->amountBuilt() );

				// don't need to pay any more than the remaining cost
				amountToAdd = std::min( amountToAdd, remainingCost );

				roundedOffAmountToAdd = _STATIC_CAST( int, amountToAdd );

				roundedOffAmountToAdd = MachLogRaces::instance().smartSubtractBMUs( race(), roundedOffAmountToAdd );

				//Cheat for the AI (since it is so stupid) - basically enabling them to add at least one BMU
				//irrespective of amount stored away - note that specialActorUpdate has to be checked so you don't
				//get the instance build bug.
				if( roundedOffAmountToAdd == 0 and
					MachLogRaces::instance().controller( race() ).type() == MachLogController::AI_CONTROLLER and
					not MachLogRaces::instance().inSpecialActorUpdate() )
					roundedOffAmountToAdd = 1;

				current->amountBuilt( current->amountBuilt() + roundedOffAmountToAdd );
				amountBuilt( current->amountBuilt() );
				//decrement storage here......
			}

		    interval = 2.5;

			if( current->amountBuilt() >= cost and not MachLogRaces::instance().maxUnitsExist( race() ) )
			{
		    	PhysConfigSpace2d::PolygonId junk;
				bool found = false;
	    		PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();

				// may use this later to get hold of possible fristd::endly machine blocking build pad
				PhysConfigSpace2d::ObjectId collisionObjectId = PhysConfigSpace2d::ObjectId ::invalidId();

				for( size_t i=0; i<nEntrances() and not found; ++i )
				{
				    //Now construct a motion chunk as if we had moved from nearPoint to location2d,
				    //and add this
					MexPoint2d location2d( entranceExternalPoint( i ) );
					MexPoint2d nearPoint( location2d.x() + 0.01, location2d.y() + 0.01 );
				    PhysMotionChunk motionChunk
				    (
				        nearPoint,
				        location2d,
				        5,			//TBD: get correct clearance out using some new spanky function.
				        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
				        PhysAbsoluteTime( timeNow - 0.01 ),
				        PhysRelativeTime( 0.0 ),
                        MexDouble::minusInfinity(), // TBD: Get correct value
                        MexDouble::plusInfinity() // TBD: Get correct value
				    );

				    //Add the motion chunk to the space
				    PhysRelativeTime collideTime = 0;
					PhysConfigSpace2d::MotionChunkId foundChunkId = PhysConfigSpace2d::MotionChunkId::invalidId();
				    found = cs.add( motionChunk, id(), &foundChunkId, &collisionObjectId, &collideTime );
					if( found )
					{
						cs.removeMotionChunk( foundChunkId );
						createNewMachine( *productionLine_.front(), entranceInternalPoint( i ), entranceExternalPoint( i ), &logRace() );
						_DELETE( *productionLine_.begin() );
						productionLine_.erase( productionLine_.begin() );
						amountBuilt( 0 );

		                //Notify observers that the machine has been built
		                notifyObservers( CLIENT_SPECIFIC, MachLog::MACHINE_BUILT );
					}
					else
					{
						// attempt to move any machine blocking the pad out of the way
						//if they're ours to control
						if( collisionObjectId != 0 )
						{
							if( MachLogRaces::instance().actorExists( collisionObjectId.asScalar() ) )
							{
								MachActor* pActor = &( MachLogRaces::instance().actor( collisionObjectId.asScalar() ) );

								if( pActor->race() == race()
									and pActor->objectIsMachine()
									and pActor->isIdle()
									and not pActor->asMachine().isIn1stPersonView() )
								{
									// right, let's move it out of the way.
									moveToDeployPoint( &pActor->asMachine(), entranceInternalPoint( i ), entranceExternalPoint( i ), BLOCKING_MACHINE );
								}
							}
						}
					}
				}
				//No space to deploy... wait 5 seconds to allow machine to move out of the way.
				if( not found )
				{
					interval = 5.0;
				}

			}

			if( roundedOffAmountToAdd > 0 )
				lastUpdateTime_ = timeNow;
		}
	}
	else
		// factory not yet built - update lastUpdateTime to timeNow so that the factory, when completed, will not
		// receive manufacturing points from the time period before it was completed
		lastUpdateTime_ = timeNow;

	PhysRelativeTime alteredMaxCPUTime = 0.1;
	MATHEX_SCALAR junk = 0;
	MachActor::update( alteredMaxCPUTime, junk );

	return interval;
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysFactory* MachLogFactory::pPhysFactory()
{
    return (MachPhysFactory*)( &physObject() );
}

const MachPhysFactory* MachLogFactory::pPhysFactory() const
{
    return (MachPhysFactory*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogFactory::constructionData() const
{
	return data();
}

const MachPhysFactoryData& MachLogFactory::data() const
{
	return pPhysFactory()->data();
}

bool MachLogFactory::buildMachine( const MachLog::ObjectType type, const int subType, const MachLogMachine::Level hwLevel,
									const MachLogMachine::Level swLevel, int priority, MachPhys::WeaponCombo wc )
{
	CB_MachLogFactory_DEPIMPL();
	HAL_STREAM("MachLogFactory::buildMachine:\n\t" << type << "\n\tsubType " << subType << "\n\thwLevel " << hwLevel << " swLevel" << swLevel << "\n\twc" << wc << std::endl );
	if( isAllowedToBuild( type, subType, hwLevel, wc ) )
	{
		MachLogProductionUnit* newUnit = _NEW( MachLogProductionUnit( type, subType, hwLevel, swLevel, priority ) );
		newUnit->amountBuilt( 0 );
		newUnit->weaponCombo( wc );
		insertIntoProductionLine( newUnit );
//		productionLine_.push_back( newUnit );
		return true;
	}
	return false;
}
/*
bool operator==(MachLogProductionUnit a, MachLogProductionUnit b)
{
	return a.type() == b.type() and a.hwLevel() == b.hwLevel() and a.swLevel() == b.swLevel();
}
*/
bool MachLogFactory::buildMachineUnique( const MachLog::ObjectType type, const int subType,
										const MachLogMachine::Level hwLevel, const MachLogMachine::Level swLevel,
										int priority, MachPhys::WeaponCombo wc )
{
	CB_MachLogFactory_DEPIMPL();
	if( isAllowedToBuild( type, subType, hwLevel, wc ) )
	{
		MachLogProductionUnit* newUnit = _NEW( MachLogProductionUnit( type, subType, hwLevel, swLevel, priority ) );
		newUnit->weaponCombo( wc );
		newUnit->amountBuilt( 0 );
		bool found = false;
		for( ProductionLine::iterator i = productionLine_.begin(); i!= productionLine_.end() and not found ; ++i )
			if( **i == *newUnit )
				found = true;
		if( not found )
		{
			insertIntoProductionLine( newUnit );
//			productionLine_.push_back( newUnit );
		}
		else
		{
			_DELETE( newUnit );
		}
		return true;
	}
	return false;
}

void MachLogFactory::clearProductionLine()
{
   //HAL_STREAM("(" << id() << ") MLFactory:;clearProductionLine " << productionLine_.size() << std::endl );
	CB_MachLogFactory_DEPIMPL();
	while( not productionLine_.empty() )
	{
		_DELETE( *productionLine_.begin() );
		productionLine_.pop_front();
	}
}
/*
template< class CONTAINER, class T >
void
factory_ctl_erase( CONTAINER *cPtr, const T& t )
{
	cPtr->erase( remove( cPtr->begin(), cPtr->end(), t ), cPtr->end() );
}
*/

void MachLogFactory::removeMachine( const MachLog::ObjectType )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogFactory::cancelAllProduction()
{
	CB_MachLogFactory_DEPIMPL();
	if( not productionLine_.empty() )
	{
		cancelProductionUnit( *productionLine_.begin() );
		amountBuilt( 0 );
		clearProductionLine();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogFactory::cancelProductionUnit( MachLogProductionUnit* pUnit )
{
	CB_MachLogFactory_DEPIMPL();
	if( not productionLine_.empty() )
	{
		ProductionLine::iterator i = find( productionLine_.begin(), productionLine_.end(), pUnit );

		ASSERT( i != productionLine_.end(), "Production unit not found in queue!" );

		// refund BMUs if this is the first (= possibly partially built) item
		if (i == productionLine_.begin() )
		{
				MachPhys::BuildingMaterialUnits refundedBMUs = ( (float)(*i)->amountBuilt() * MachLogRaces::instance().stats().cancelledRefundablePercentage() );
				MachLogRaces::instance().smartAddBMUs( race(), refundedBMUs );
		}

		_DELETE( *i );
		productionLine_.erase( i );
	}
}

void MachLogFactory::moveProductionUnit( MachLogProductionUnit* pUnit,
										 MachLogFactory::MoveDirection direction)
{
	CB_MachLogFactory_DEPIMPL();
	ProductionLine::iterator tempIterator = productionLine_.end();
	--tempIterator;
	for( ProductionLine::iterator i = productionLine_.begin();
		i != productionLine_.end(); ++i )
		if( (*i) == pUnit )
		{
//			_DELETE( (*i) );
			if( direction == TOWARDS_FRONT and i == productionLine_.begin() )
				return;
			if( direction == TOWARDS_BACK and i == tempIterator )
				return;

			ProductionLine::iterator insertIterator = i;
			switch( direction )
			{
			case TOWARDS_FRONT:
				--insertIterator;
				productionLine_.insert( insertIterator, pUnit );
				productionLine_.erase( i );
				break;

			case TOWARDS_BACK:
				insertIterator++;
				insertIterator++;
				productionLine_.insert( insertIterator, pUnit );
				productionLine_.erase( i );
				break;
			default:
				ASSERT_BAD_CASE;
			}
		}
}


MachPhys::BuildingMaterialUnits
MachLogFactory::amountBuilt() const
{
	CB_MachLogFactory_DEPIMPL();
	return amountBuilt_;
}

MachPhys::BuildingMaterialUnits
MachLogFactory::amountBuilt()
{
	CB_MachLogFactory_DEPIMPL();
	return amountBuilt_;
}


void MachLogFactory::amountBuilt( const MachPhys::BuildingMaterialUnits& newAmount)
{
	CB_MachLogFactory_DEPIMPL();
	PRE( newAmount >= 0 );
	amountBuilt_ = newAmount;
}


bool MachLogFactory::currentlyBuilding
(
	MachLog::ObjectType& reftype, int& refSubType,
	MachLogMachine::Level& refhwLevel, MachLogMachine::Level& refswLevel,
	MachPhys::WeaponCombo& refWc
) const
{
	CB_MachLogFactory_DEPIMPL();
	if( productionLine_.empty() )
	{
		return false;
	}

	MachLogProductionUnit* current = *productionLine_.begin();
	reftype = current->type();
	refSubType = current->subType();
	refhwLevel = current->hwLevel();
	refswLevel = current->swLevel();
	refWc = current->weaponCombo();
	return true;
}

bool MachLogFactory::currentlyBuildingProductionUnit( MachLogProductionUnit* pReturn ) const
{
	CB_MachLogFactory_DEPIMPL();
	if( productionLine_.empty() )
	{
		return false;
	}
	MachLogProductionUnit* pCurrent = *productionLine_.begin();
	pReturn->type( pCurrent->type() );
	pReturn->subType( pCurrent->subType() );
	pReturn->hwLevel( pCurrent->hwLevel() );
	pReturn->swLevel( pCurrent->swLevel() );
	pReturn->priority( pCurrent->priority() );
	pReturn->weaponCombo( pCurrent->weaponCombo() );
	return true;

}

MATHEX_SCALAR	MachLogFactory::percentComplete() const
{
	CB_MachLogFactory_DEPIMPL();
	MATHEX_SCALAR result;
	result = 100.0 * (float)(*productionLine_.begin() )->amountBuilt() / (float)currentTotalCost();
	return result;
}

MachPhys::BuildingMaterialUnits MachLogFactory::currentTotalCost() const
{
	CB_MachLogFactory_DEPIMPL();
	if( productionLine_.empty() )
		return 0;

	const MachLogProductionUnit* current = *productionLine_.begin();
	HAL_STREAM("(" << id() << ") MLFactory::currentTotalCost of: " << *current << std::endl );
	MachPhys::BuildingMaterialUnits cost = MachLogRaces::instance().researchTree().researchItem( current->type(), current->subType(), current->hwLevel(), current->weaponCombo() ).factoryInstanceCost();
	return cost;
}

const MachPhys::FactorySubType& MachLogFactory::subType() const
{
	CB_MachLogFactory_DEPIMPL();
	return subType_;
}

const MachLogResearchTree::ResearchItems& MachLogFactory::buildItems() const
{
	CB_MachLogFactory_DEPIMPL();
	return buildItems( subType_, level() );
}

//static
const MachLogResearchTree::ResearchItems& MachLogFactory::buildItems( MachPhys::FactorySubType subType, int level )
{
	return researchItems()[ subType ][ level ];
}

bool MachLogFactory::isAllowedToBuild( MachLog::ObjectType type, int subType, int hwLevel, MachPhys::WeaponCombo wc ) const
{
	CB_MachLogFactory_DEPIMPL();
	bool found = false;
	for( MachLogResearchTree::ResearchItems::const_iterator i = buildItems().begin(); i != buildItems().end() and not found; ++i )
		if( (*i)->researched( race() ) and (*i)->objectType() == type and (*i)->subType() == subType and (*i)->hwLevel() == hwLevel and (*i)->weaponCombo() == wc )
			found = true;
	return found;
}

const MachLogFactory::ProductionLine& MachLogFactory::productionLine() const
{
	CB_MachLogFactory_DEPIMPL();
    return productionLine_;
}

void MachLogFactory::insertIntoProductionLine( MachLogProductionUnit* newUnit )
{
	CB_MachLogFactory_DEPIMPL();
	bool inserted = false;
	for( ProductionLine::iterator i = productionLine_.begin();
		i != productionLine_.end() and not inserted; ++i )
		{
			if( newUnit->priority() > (*i)->priority() )
			{
				inserted = true;
				productionLine_.insert( i, newUnit );
			}
		}
	if( not inserted )
		productionLine_.push_back( newUnit );
}

//static
MachLogFactory::ResearchItemsArray& MachLogFactory::researchItems()
{
	static MachLogResearchTree::ResearchItems items[ 5 ][ 5 ];
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        for( size_t i = 0; i != 5; ++i )
            for( size_t j = 0; j != 5; ++j )
                items[i][j].reserve( 32 );
    }

	return items;
}

//static
void MachLogFactory::loadGame()
{
	const SysPathName factoryItemsPath( "data/factory.bld" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, factoryItemsPath, ios::text ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( SysMetaFileIstream( metaFile, factoryItemsPath, std::ios::in ) ) );
    }
    else
    {
        ASSERT_FILE_EXISTS( factoryItemsPath.c_str() );
        //pIstream = _NEW( ifstream( factoryItemsPath.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( std::ifstream( factoryItemsPath.c_str(), std::ios::in ) ) );
    }

	UtlLineTokeniser parser( *pIstream, factoryItemsPath );
	int i = 0;
	int j = 0;
	while( not parser.finished() )
	{
		size_t lineSize = parser.tokens().size();
		if( lineSize > 0 and parser.tokens()[0] != "//" )
		{
			bool working = false;
			if( parser.tokens()[0] == "CIVILIAN" )
			{
				//HAL_STREAM("Processing CIVILIAN\n" );
				i = 0;
				j = atoi( parser.tokens()[1].c_str() );
				working = true;
				parser.parseNextLine();
			}
			if( parser.tokens()[0] == "MILITARY" )
			{
				//HAL_STREAM("Processing MILITARY\n" );
				i = 1;
				j = atoi( parser.tokens()[1].c_str() );
				working = true;
				parser.parseNextLine();
			}
			while( working )
			{
				if( parser.tokens().size() > 0 )
				{
					MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
					if( parser.tokens()[0] == "END" )
						working = false;
					else if( parser.tokens().size() == 2 )
					{
						MachLogResearchItem* pRI = &MachLogRaces::instance().researchTree().researchItem( MachLogScenario::objectType( parser.tokens()[0] ), 0, atoi( parser.tokens()[1].c_str() ), wc );
						researchItems()[i][j].push_back( pRI );
					}
					else if( parser.tokens().size() == 3 )
					{
						MachLog::ObjectType obType = MachLogScenario::objectType( parser.tokens()[0] );
						MachLogResearchItem* pRI = &MachLogRaces::instance().researchTree().researchItem( obType, MachLogScenario::objectSubType( obType, parser.tokens()[2] ), atoi( parser.tokens()[1].c_str() ), wc );
						researchItems()[i][j].push_back( pRI );
					}
					else if( parser.tokens().size() == 4 )
					{
						MachLog::ObjectType obType = MachLogScenario::objectType( parser.tokens()[0] );
						wc = MachLogScenario::weaponCombo( parser.tokens()[3] );
						int hwLevel = atoi( parser.tokens()[1].c_str() );
						MachLogResearchItem* pRI = &MachLogRaces::instance().researchTree().researchItem( obType, MachLogScenario::objectSubType( obType, parser.tokens()[2] ), hwLevel, wc );
						researchItems()[i][j].push_back( pRI );
						if( obType == MachLog::AGGRESSOR and hwLevel == 4 )
						{
							MachLogResearchItem* pRI2 = &MachLogRaces::instance().researchTree().researchItem( obType, MachLogScenario::objectSubType( obType, parser.tokens()[2] ), hwLevel, MachPhys::LR_LARGE_MISSILE_X2 );
							researchItems()[i][1].push_back( pRI2 );
							researchItems()[i][3].push_back( pRI2 );
							researchItems()[i][5].push_back( pRI2 );
						}
					}
				}
				if( working )
					parser.parseNextLine();
			}
		}
		parser.parseNextLine();
	}
}

//static
void MachLogFactory::unloadGame()
{
	for ( size_t i = 0; i < 5; ++i )
	{
		for ( size_t j = 0; j < 5; ++j )
		{
			while ( not researchItems()[i][j].empty() )
				researchItems()[i][j].pop_back();
		}
	}
}

void MachLogFactory::specifiedDeployPoint(  const MexPoint2d& dest )
{
	CB_MachLogFactory_DEPIMPL();
	specifiedDeployPoint_.x( dest.x() );
	specifiedDeployPoint_.y( dest.y() );
	deployPointIsSet_ = true;
}

const MexPoint2d& MachLogFactory::specifiedDeployPoint()
{
	CB_MachLogFactory_DEPIMPL();
	return specifiedDeployPoint_;
}

void perWrite( PerOstream& ostr, const MachLogFactory& actor )
{
	const MachLogConstruction& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysFactory()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );

	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogFactory& actor )
{
	MachLogConstruction& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysFactory* pPhysFactory = MachLogFactory::pNewPhysFactory( (MachPhys::FactorySubType)actor.subType(), &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysFactory, trans );
	actor.id( id );
	actor.constructionCreated( pPhysFactory );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysFactory->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );
}

MachLogFactory::MachLogFactory( PerConstructor con )
:	MachLogConstruction( con )
{
}

//	virtual
void MachLogFactory::assignToDifferentRace( MachLogRace& newRace )
{
	CB_MachLogFactory_DEPIMPL();

	deployPointIsSet_ = false;
	MachLogConstruction::assignToDifferentRace( newRace );
}

void MachLogFactory::moveToDeployPoint( MachLogMachine* pMach, const MexPoint2d& entranceInternalPoint, const MexPoint2d& entranceExternalPoint, BuiltOrBlocking builtOrBlocking )
{
	CB_MachLogFactory_DEPIMPL();

	// determine where the machine is to deploy to.
	MexPoint2d dest;

	MexTransform3d deployPointAsTransform;

	MATHEX_SCALAR radius;

	bool movingBlockingMachine = ( builtOrBlocking == BLOCKING_MACHINE );

	if ( deployPointIsSet_ and not movingBlockingMachine )
	{
		deployPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( position(), specifiedDeployPoint_, MachLogSpacialManipulation::DESTINATION_END_IS_CENTRE );
		radius = 5;
	}
	else
	{
		MATHEX_SCALAR separation = entranceInternalPoint.euclidianDistance( entranceExternalPoint );
	    //MexPoint2d dest3( entranceInternalPoint, entranceExternalPoint, (separation + 35.0) / separation );
		deployPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( position(), entranceExternalPoint, MachLogSpacialManipulation::DESTINATION_END_IS_CENTRE );

		if( movingBlockingMachine )
			radius = 15;
		else
			radius = 25;
	}

	bool finished = false;
	while( not finished )
	{
		if( not MachLogSpacialManipulation::getNearestFreeSpacePoint( deployPointAsTransform, radius , pMach->highClearence(), &dest ) )
			radius += 20;
		else
			finished = true;
	}

	// set the fellow scuttling off to his deploy point
	MATHEX_SCALAR tolerance = 10;
	if( deployPointIsSet_ )
		tolerance = 25;
	pMach->newOperation( _NEW( MachLogMoveToOperation( pMach, dest, false, tolerance ) ) );
}

bool MachLogFactory::buildMachineUniqueWithSquadIndex( const MachLog::ObjectType type, const int subType,
										const MachLogMachine::Level hwLevel, const MachLogMachine::Level swLevel,
										int priority, int squadIndex, MachPhys::WeaponCombo wc )
{
	CB_MachLogFactory_DEPIMPL();
	if( isAllowedToBuild( type, subType, hwLevel, wc ) )
	{
		MachLogProductionUnit* newUnit = _NEW( MachLogProductionUnit( type, subType, hwLevel, swLevel, priority ) );
		newUnit->weaponCombo( wc );
		newUnit->amountBuilt( 0 );
		newUnit->constructionId( squadIndex );
		bool found = false;
		for( ProductionLine::iterator i = productionLine_.begin(); i!= productionLine_.end() and not found ; ++i )
			if( **i == *newUnit )
				found = true;
		if( not found )
		{
			insertIntoProductionLine( newUnit );
//			productionLine_.push_back( newUnit );
		}
		else
		{
			_DELETE( newUnit );
		}
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End FACTORY.CPP **************************************************/
