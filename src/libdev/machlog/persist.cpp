/*
 * P E R S I S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "ctl/list.hpp"
#include "mathex/point2d.hpp"

#include "world4d/generic.hpp"
#include "world4d/domain.hpp"

#include "sim/manager.hpp"

#include "machlog/persist.hpp"
#include "machlog/permach.hpp"
#include "machlog/percons.hpp"
#include "machlog/perweap.hpp"
#include "machlog/perops.hpp"
#include "machlog/perother.hpp"

#include "machlog/cntrl.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/actor.hpp"
#include "machlog/races.hpp"
#include "machlog/race.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/lsgextra.hpp"
#include "machlog/planet.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/minesite.hpp"
#include "machlog/dbhandlr.hpp"

PER_DEFINE_PERSISTENT( MachLogPersistence );

// static
MachLogPersistence& MachLogPersistence::instance()
{
    static MachLogPersistence instance_;
    return instance_;
}

MachLogPersistence::MachLogPersistence()
:	pSceneManager_( NULL ),
	pExtras_( NULL )
{
	MachLogMachinePersistence::instance();
	MachLogConstructionPersistence::instance();
	MachLogOtherPersistence::instance();
	MachLogWeaponPersistence::registerDerivedClasses();
	MachLogOperationPersistence::registerDerivedClasses();
	registerDerivedClasses();
    TEST_INVARIANT;
}

MachLogPersistence::~MachLogPersistence()
{
    TEST_INVARIANT;

}

void MachLogPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachLogPersistence::sceneManager( W4dSceneManager* pNewSceneManager )
{
	pSceneManager_ = pNewSceneManager;
}

ostream& operator <<( ostream& o, const MachLogPersistence& t )
{

    o << "MachLogPersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

MachLogMachinePersistence& MachLogPersistence::machines()
{
	return MachLogMachinePersistence::instance();
}

const MachLogMachinePersistence& MachLogPersistence::machines() const
{
	return MachLogMachinePersistence::instance();
}

MachLogConstructionPersistence& MachLogPersistence::constructions()
{
	return MachLogConstructionPersistence::instance();
}

const MachLogConstructionPersistence& MachLogPersistence::constructions() const
{
	return MachLogConstructionPersistence::instance();
}

MachLogOtherPersistence& MachLogPersistence::others()
{
	return MachLogOtherPersistence::instance();
}

const MachLogOtherPersistence& MachLogPersistence::others() const
{
	return MachLogOtherPersistence::instance();
}

void MachLogPersistence::setDataForWrite() const
{
	MachLogPersistence& nonConstPer = *( _CONST_CAST( MachLogPersistence*, this ) );
//removed for now	nonConstPer.planetName_ = MachLogRaces::instance().currentPlanet();
//removed for now	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
//removed for now	{
//removed for now		if( MachLogRaces::instance().raceObjects( i ).size() > 0 )
//removed for now			nonConstPer.racePopulated_[ (int)i ] = true;
//removed for now		else
//removed for now			nonConstPer.racePopulated_[ (int)i ] = false;
//removed for now	}
	//remove any left over from the time before.
	nonConstPer.controllers_.erase( nonConstPer.controllers_.begin(), nonConstPer.controllers_.end() );
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
	{
		if( MachLogRaces::instance().raceObjects( i ).size() > 0 )
		{
			MachLogController *pController = &MachLogRaces::instance().controller( i );
			if ( !pController )
				continue;

			nonConstPer.controllers_.push_back( pController );
		}
	}
}

void MachLogPersistence::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogController );
	PER_REGISTER_DERIVED_CLASS( MachLogAIController );
	PER_REGISTER_DERIVED_CLASS( MachLogPCController );
}

void perWrite( PerOstream& ostr, const MachLogPersistence& per )
{
	PRE( per.pExtras_ );
	bool resumeClock = not SimManager::instance().isSuspended();
	SimManager::instance().suspend();
	HAL_STREAM("MachLogPersistence::perWrite\n" );
	per.setDataForWrite();
	PhysAbsoluteTime now = SimManager::instance().currentTime();
	PER_WRITE_RAW_OBJECT( ostr, now );

    MachLogRaces::instance().databaseHandler().writePersistenceData( ostr );

	bool raceExists[ MachPhys::N_RACES ];

	HAL_STREAM("MachLogPersistence::perWrite pcController.race " << MachLogRaces::instance().pcController().race() << std::endl);
	MachPhys::Race pcControllerRace = MachLogRaces::instance().pcController().race();
	PER_WRITE_RAW_OBJECT( ostr, pcControllerRace );
	for( MachPhys::Race race = MachPhys::RED; race < MachPhys::N_RACES; ++race )
	{
		if( MachLogRaces::instance().raceInGame( race ) )
			raceExists[ race ] = true;
		else
			raceExists[ race ] = false;
	}
	PER_WRITE_RAW_DATA( ostr, raceExists, MachPhys::N_RACES * sizeof( bool ) );

	HAL_STREAM("gonna persist out vector of controllers\n" );
	ostr << per.controllers_;
	HAL_STREAM("MachLogPersistence::perWrite writing others collection " << std::endl);
	ostr << per.others();
	HAL_STREAM("MachLogPersistence::perWrite writing constructions collection " << std::endl);
	ostr << per.constructions();
	HAL_STREAM("MachLogPersistence::perWrite writing machines collection " << std::endl);
	ostr << per.machines();
	MachLogRaces::instance().persistenceWriteNumbers( ostr );
	per.pExtras_->perWriteExtras( ostr );
	if( resumeClock )
		SimManager::instance().resume();
}

void perRead( PerIstream& istr, MachLogPersistence& per )
{
	PRE( per.pExtras_ );
	ASSERT( per.pSceneManager_,"The scene manager has not been set on a load saved game.\n" );

	//stack data to read holding data into.
	string planetName( MachLogRaces::instance().currentPlanet() );
	MachPhys::Race pcControllerRace;

	bool raceExists[ MachPhys::N_RACES ];

	//read the game time
	PhysAbsoluteTime now;
	PER_READ_RAW_OBJECT( istr, now );

    MachLogRaces::instance().databaseHandler().readPersistenceData( istr );

	//each controller has been streamed but we still need to know which one belongs to the player race
	//this could be done by controller.type() but this will kill lmultiplayer if it every gets extended in
	//so for the sake of the few extra bytes we write it out with the save game info.
	HAL_STREAM("read pcControllerRace \n" );
	PER_READ_RAW_OBJECT( istr, (int&)pcControllerRace );
	HAL_STREAM("read in raceExist flags\n" );
	PER_READ_RAW_DATA( istr, raceExists, MachPhys::N_RACES * sizeof( bool ) );
	for( MachPhys::Race race = MachPhys::RED; race < MachPhys::N_RACES; ++race )
	{
		if( raceExists[ race ] )
		{
			HAL_STREAM("Creating a new race for " << race << std::endl );
			MachLogRace* pRace = _NEW( MachLogRace( race ) );
			MachLogRaces::instance().race( race, pRace, MachLogRaces::DO_NOT_CREATE_SQUADRONS );
		}
	}

	//recreate the artefacts race.
    MachLogRaces::instance().race( MachPhys::NORACE, _NEW( MachLogRace( MachPhys::NORACE ) ), MachLogRaces::DO_NOT_CREATE_SQUADRONS );

	//recreate all the MLRace(s) and controllers and assign them to MLRaces.
	//These have to be present before we start reading in actors.
	HAL_STREAM("Read in controllers\n" );
	istr >> per.controllers_;
	HAL_STREAM("number of controllers: " << per.controllers_.size() << std::endl );

	for( size_t i = 0; i < per.controllers_.size(); ++i )
	{
		HAL_STREAM("Recreating and registering controllers\n" );
		HAL_STREAM( "Controller at address " << (void*)per.controllers_[i] << std::endl );
		HAL_STREAM( "Dereference:: " << *(per.controllers_[i]) << std::endl );
		MexTransform3d localTransform;
	    W4dEntity* pAnyDomain = MachLogPlanetDomains::pDomainPosition( MexPoint3d(0,0,0), 0, &localTransform );
	    W4dEntity* pPhysObject = _NEW( W4dGeneric( pAnyDomain, localTransform ) );
		HAL_STREAM("setting object ptr of controller...lets see if this works!\n" );
		per.controllers_[i]->setObjectPtr( pPhysObject, localTransform );
		HAL_STREAM("derefercing element " << i << " to get race\n" );
		MachPhys::Race race = per.controllers_[i]->race();
		HAL_STREAM("creating new log race\n" );
		HAL_STREAM("registerting controller with races.\n" );
		MachLogRaces::instance().controller( race, per.controllers_[i] );
		if( per.controllers_[i]->type() == MachLogController::PLAYER_CONTROLLER )
		{
			HAL_STREAM(" controller is PLAYER_CONTROLLER registering with races\n" );
   			MachLogRaces::instance().pcController( _STATIC_CAST( MachLogPCController*, per.controllers_[i] ) );
		}

	}

	SimManager::instance().setSimulationTime( now );
	SimManager::instance().suspend();
	HAL_STREAM("MachLogPersistence perRead reading others collection " << std::endl );
	istr >> per.others();
	HAL_STREAM("MachLogPersistence perRead reading constructions collection " << std::endl );
	istr >> per.constructions();
	HAL_STREAM("MachLogPersistence perRead reading machines collection " << std::endl );
	istr >> per.machines();
	MachLogRaces::instance().persistenceReadNumbers( istr );
	per.pExtras_->perReadExtras( istr );
	/*
	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin(); i != MachLogPlanet::instance().sites().end(); ++i )
	{
		if( (*i)->hasBeenDiscovered() )
			MachLogActorMaker::newLogOreHolograph( (*i)->discoveredByRace(), 0, 0, (*i)->position() );
	}
	*/
	SimManager::instance().resume();
	HAL_STREAM("All persistence done SimManager now has in it:\n" << SimManager::instance() << std::endl );
}

void MachLogPersistence::extras( MachLogLoadSaveGameExtras* pExtras )
{
	PRE( pExtras );
	pExtras_ = pExtras;
}

/* End PERSIST.CPP **************************************************/
