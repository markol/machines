/*
 * E D I T C O N S T R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editmach.hpp"
#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/mouse.hpp"
#include "utility/linetok.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/machphys.hpp"
#include "machphys/machdata.hpp"
#include "machphys/machphys.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/construc.hpp"
#include "machphys/administ.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/apc.hpp"
#include "machphys/technici.hpp"
#include "machphys/levels.hpp"
#include "machphys/legcombo.hpp"
#include "machphys/marker.hpp"
#include "world4d/scenemgr.hpp"
#include "stdlib/string.hpp"
#include "afx/resource.hpp"
#include "planeted/strings.hpp"


PedMachineEditor::PedMachineEditor()
:	PedActorEditor( "machine" ),
	pSelectedMachine_( NULL )
{
	initialiseActors();
    TEST_INVARIANT;
}

PedMachineEditor::~PedMachineEditor()
{
    TEST_INVARIANT;

	for ( 	Machines::iterator i = machineData_.begin();
			i != machineData_.end(); i++ )
	{
  		_DELETE( *i );
	}
	for( mapIterator_ = machineMap_.begin(); mapIterator_ != machineMap_.end();
		++mapIterator_ )
	{
		_DELETE( (*mapIterator_).machine_ );
	}
}

// virtual
void PedMachineEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL )
   	INVARIANT( ( pSelectedMachine_ == (*mapIterator_).machine_ ) or
   			   ( pSelectedMachine_ == NULL ) );

}

// virtual
void PedMachineEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	PedActorEditor::processInput( devButtonEvent );
	if ( 	devButtonEvent.scanCode() == DevKey::KEY_2 and
				not devButtonEvent.wasShiftPressed() and
				devButtonEvent.wasCtrlPressed() )
	{
		processHide( hidden_ );
	}
}

// virtual
void PedMachineEditor::displayModeInfo()
{
	PedActorEditor::displayModeInfo();
	pSceneManager_->out() << "Machine: " << (*machDataIter_)->description_ << std::endl;
	pSceneManager_->out() << "Race: " << race_ << std::endl;
}

void PedMachineEditor::displayKeyboardCtrls()
{
	PRE( pSceneManager_ != NULL );
	pSceneManager_->out() << "C : race " << std::endl;

   	PedActorEditor::displayKeyboardCtrls();
}

// virtual
void PedMachineEditor::readScnFile( PedScenarioFile& scenarioFile )
{
	// retrieve list of machines from scenario file for a specific race
	PedScenarioFile::Machines scnMachines = scenarioFile.machines();

	for ( PedScenarioFile::Machines::iterator scnMachIter = scnMachines.begin();
		  scnMachIter != scnMachines.end(); ++scnMachIter )
	{
		// Create machine
		W4dEntity* domain = NULL;
	    const MexTransform3d localTransform = localTransformAtPoint( &domain, (*scnMachIter).position );
		ASSERT( domain != NULL, "for position:" );

 		// Set iterator to legal machine data to point to correct set of machine data
		for( machDataIter_ = machineData_.begin(); machDataIter_ != machineData_.end(); ++machDataIter_ )
		{
			if 	( ( (*machDataIter_ )->machineType_ == (*scnMachIter).type )
				and ( (*machDataIter_ )->subType_ == (*scnMachIter).subType )
				and ( (*machDataIter_ )->bodyLevel_ == (*scnMachIter).bodyLevel )
				and ( (*machDataIter_ )->brainboxLevel_ == (*scnMachIter).brainboxLevel )
				and	( (*machDataIter_ )->weaponCombo_ == (*scnMachIter).weaponCombo ) )
			{
				break;
			}
		}
		// Set current race in order to create construction of the correct race
		race_ = (*scnMachIter).race;
		// Create new machine ( using machDataIter_ ) and new mapping
		pSelectedMachine_ = createMachine( *domain, localTransform, **machDataIter_ );

		createMapping( pSelectedMachine_, *machDataIter_, *scnMachIter );
	}

	changingMode();
}

// virtual
void PedMachineEditor::writeScnFile( PedScenarioFile& scenarioFile )
{
	// Construct machine list to pass to scenarioFile
	PedScenarioFile::Machines machines;

	for( MachineMappings::iterator i = machineMap_.begin(); i != machineMap_.end(); ++i )
	{
		// Take account of any dragging/moving, etc
		(*i).scnMachine_.position = (*i).machine_->globalTransform().position();
		machines.push_back( (*i).scnMachine_ );
	}
	scenarioFile.machines( machines );
}

// virtual
W4dEntity* PedMachineEditor::currentActor()
{
	return pSelectedMachine_;
}

// virtual
void PedMachineEditor::processCycle( PedActorEditor::CycleDir dir)
{
	switch( dir )
	{
	   	case NEXT:
			if (machDataIter_ == (machineData_.end() - 1) )
				machDataIter_ = machineData_.begin();
			else ++machDataIter_;
			break;

		case PREV:
			if ( machDataIter_ == machineData_.begin() )
				machDataIter_ = (machineData_.end() - 1);
			else --machDataIter_;
			break;
	}
	if ( pSelectedMachine_ )
	{
	   W4dEntity* domain = pSelectedMachine_->pParent();
	   const MexTransform3d locTransform = pSelectedMachine_->localTransform();
	   processDelete();
	   createEntity( *domain, locTransform );
	}
}

// virtual
void PedMachineEditor::processSelection()
{
	W4dEntity* actor = NULL;

	if ( actorSelected( &actor ) )
	{
		if ( ( ( actor->id() == PedActorEditor::MACHINE ) and ( not alreadySelected_ ) ) or
			( ( actor->id() == PedActorEditor::MACHINE ) and ( alreadySelected_ ) and ( actor != pSelectedMachine_ ) ) )
		{
			// actor is a machine of some sort
 			pSelectedMachine_ = ( MachPhysMachine* ) actor;	// Safe since we know entity is a MachPhysConstructor

			highlightSelected();

			pSelectedMachine_->solid( W4dEntity::NOT_SOLID );

			// lookup selected machine in map and set correct position
			// in machine data list
 			for ( mapIterator_ = machineMap_.begin(); mapIterator_ != machineMap_.end();
 				  ++mapIterator_ )
			{
				if ( (*mapIterator_).machine_ == pSelectedMachine_ )
				{
					for ( machDataIter_ = machineData_.begin(); machDataIter_!= machineData_.end();
						  machDataIter_++ )
					{
						if ( (*mapIterator_).data_ == *machDataIter_ )
							break;
					}
					break;
				}
			}
			alreadySelected_ = true;
		}
		else if  ( ( actor->id() == PedActorEditor::MACHINE ) and ( alreadySelected_ ) and ( actor == pSelectedMachine_ ) )
		{
			pSelectedMachine_->solid( W4dEntity::NOT_SOLID );
			mouseDrag_ = true;
		}
		else
		{
			// mouse pointer not pointing at object - so deselect selected object(s)
			_DELETE( pMarker_ );
			pMarker_ = NULL;
			pSelectedMachine_ = NULL;
			alreadySelected_ = false;
			mouseDrag_ = false;
		}
	}
}

// virtual
void PedMachineEditor::processDelete()
{
	if ( pSelectedMachine_ != NULL )
	{
		const MachineMapping& map = *mapIterator_;

		ASSERT( pSelectedMachine_ == map.machine_, "" );
		_DELETE( pMarker_ );
		pMarker_ = NULL;
		_DELETE( map.machine_ );
		pSelectedMachine_ = NULL;	// Since was pointing to map.machine_
		machineMap_.erase( mapIterator_ );
		mapIterator_ = machineMap_.begin(); // To prevent dangling iterator
	}
}

// virtual
void PedMachineEditor::processRace()
{
	PedActorEditor::processRace();

	if ( pSelectedMachine_ )
	{
		ASSERT(  pSelectedMachine_ == (*mapIterator_).machine_ , "" );
		(*mapIterator_).scnMachine_.race = race_;
		pSelectedMachine_->changeRace( race_ );
	}
}

// virtual
void PedMachineEditor::rotateAfterMove()
{
	// A bit naughty...
}

// virtual
void PedMachineEditor::processHide( bool hidden )
{
	for ( MachineMappings::iterator i = machineMap_.begin(); i != machineMap_.end(); i++ )
		(*i).machine_->visible( not hidden );

	PedActorEditor::processHide( hidden );
}

// virtual
void PedMachineEditor::changeAllSolidities( W4dEntity::Solidity solidity)
{
	for ( MachineMappings::iterator i = machineMap_.begin(); i != machineMap_.end(); i++ )
		(*i).machine_->solid( solidity );

}

void PedMachineEditor::initialiseActors()
{
	AfxResourceLib resourceLib("machstrg.xml");

	Machine* pMachine = NULL;
	string resource;
	MachPhysLevels& levels = MachPhysLevels::instance();
	MachPhysLegalWeaponCombos& weaponCombos = MachPhysLegalWeaponCombos::instance();

 	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT1 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 1, 1, resource, MachPhys::R_BOLTER ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT1 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 1, 1, resource, MachPhys::R_ELECTRIC_CHARGE ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT2 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 2, 1, resource, MachPhys::L_BOLTER ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT2 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 2, 1, resource, MachPhys::L_ELECTRIC_CHARGE ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT2 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 2, 1, resource, MachPhys::L_FLAME_THROWER2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_GRUNT3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::GRUNT, 3, 1, resource, MachPhys::LR_HEAVY_BOLTER1_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_ASSASSIN2 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::ASSASSIN, 2, 1, resource, MachPhys::L_AUTO_CANNON ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_ASSASSIN2 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::ASSASSIN, 2, 1, resource, MachPhys::L_PLASMA_RIFLE ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_ASSASSIN3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::ASSASSIN, 3, 1, resource, MachPhys::LR_PLASMA_RIFLE_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_ASSASSIN3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::ASSASSIN, 3, 1, resource, MachPhys::LR_AUTO_CANNON_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 3, 1, resource, MachPhys::LR_HEAVY_BOLTER2_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 3, 1, resource, MachPhys::LR_PLASMA_CANNON1_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_BALLISTA3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::BALLISTA, 3, 1, resource, MachPhys::T_MULTI_LAUNCHER1 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_BALLISTA4 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::BALLISTA, 4, 1, resource, MachPhys::LR_MULTI_LAUNCHER2_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT4 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 4, 1, resource, MachPhys::LR_PLASMA_CANNON2_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT4 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 4, 1, resource, MachPhys::LR_MULTI_LAUNCHER3_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT5 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 5, 1, resource, MachPhys::LRT_MULTI_LAUNCHER4_X3 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_AGGRESSOR_KNIGHT5 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::KNIGHT, 5, 1, resource, MachPhys::LRT_PLASMA_CANNON2_X3 ) );
	machineData_.push_back( pMachine );


	resource = resourceLib.getString( IDS_AGGRESSOR_BALLISTA3 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::BALLISTA, 3, 1, resource, MachPhys::T_VORTEX ) );
	machineData_.push_back( pMachine );


	resource = resourceLib.getString( IDS_AGGRESSOR_NINJA5 );
	pMachine = _NEW( Machine( MachPhys::AGGRESSOR, MachPhys::NINJA, 5, 1, resource, MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_TECHNICIAN_LAB_TECH2 );
	pMachine = _NEW( Machine( MachPhys::TECHNICIAN, MachPhys::LAB_TECH, 2, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_TECHNICIAN_TECH_BOY3 );
	pMachine = _NEW( Machine( MachPhys::TECHNICIAN, MachPhys::TECH_BOY, 3, 3, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_TECHNICIAN_BRAIN_BOX5 );
	pMachine = _NEW( Machine( MachPhys::TECHNICIAN, MachPhys::BRAIN_BOX, 5, 5, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_CONSTRUCTOR_DOZER1 );
	pMachine = _NEW( Machine( MachPhys::CONSTRUCTOR, MachPhys::DOZER, 1, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_CONSTRUCTOR_BUILDER3 );
	pMachine = _NEW( Machine( MachPhys::CONSTRUCTOR, MachPhys::BUILDER, 3, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_CONSTRUCTOR_BEHEMOTH4 );
	pMachine = _NEW( Machine( MachPhys::CONSTRUCTOR, MachPhys::BEHEMOTH, 4, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_CONSTRUCTOR_BEHEMOTH5 );
	pMachine = _NEW( Machine( MachPhys::CONSTRUCTOR, MachPhys::BEHEMOTH, 5, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_RESOURCE_CARRIER1 );
	pMachine = _NEW( Machine( MachPhys::RESOURCE_CARRIER, 0, 1, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_RESOURCE_CARRIER2 );
	pMachine = _NEW( Machine( MachPhys::RESOURCE_CARRIER, 0, 2, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_RESOURCE_CARRIER3 );
	pMachine = _NEW( Machine( MachPhys::RESOURCE_CARRIER, 0, 3, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_RESOURCE_CARRIER4 );
	pMachine = _NEW( Machine( MachPhys::RESOURCE_CARRIER, 0, 4, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_RESOURCE_CARRIER5 );
	pMachine = _NEW( Machine( MachPhys::RESOURCE_CARRIER, 0, 5, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_GEO_LOCATOR1 );
	pMachine = _NEW( Machine( MachPhys::GEO_LOCATOR, 0, 1, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_GEO_LOCATOR2 );
	pMachine = _NEW( Machine( MachPhys::GEO_LOCATOR, 0, 2, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_GEO_LOCATOR3 );
	pMachine = _NEW( Machine( MachPhys::GEO_LOCATOR, 0, 3, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_BOSS1 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::BOSS, 1, 1, resource, MachPhys::T_WASP_LIGHT_STING ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_BOSS1 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::BOSS, 1, 1, resource, MachPhys::T_WASP_METAL_STING ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_BOSS2 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::BOSS, 2, 1, resource, MachPhys::T_BEE_BOMB ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_OVERSEER2 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::OVERSEER, 2, 1, resource, MachPhys::LR_PULSE_RIFLE_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_OVERSEER2 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::OVERSEER, 2, 1, resource, MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_OVERSEER3 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::OVERSEER, 3, 1, resource, MachPhys::LR_PULSE_RIFLE_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_OVERSEER3 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::OVERSEER, 3, 1, resource, MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_COMMANDER4 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::COMMANDER, 4, 1, resource, MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_COMMANDER4 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::COMMANDER, 4, 1, resource, MachPhys::LR_MULTI_LAUNCHER5_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_COMMANDER4 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::COMMANDER, 4, 1, resource, MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_COMMANDER5 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::COMMANDER, 5, 1, resource, MachPhys::LR_PULSE_CANNON_X2 ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_ADMINISTRATOR_COMMANDER5 );
	pMachine = _NEW( Machine( MachPhys::ADMINISTRATOR, MachPhys::COMMANDER, 5, 1, resource, MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_SPY_LOCATORS );
	pMachine = _NEW( Machine(  MachPhys::SPY_LOCATOR, 0, 3, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_SPY_LOCATORS );
	pMachine = _NEW( Machine(  MachPhys::SPY_LOCATOR, 0, 5, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_APC2 );
	pMachine = _NEW( Machine( MachPhys::APC, 0, 2, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_APC4 );
	pMachine = _NEW( Machine( MachPhys::APC, 0, 4, 1, resource ) );
	machineData_.push_back( pMachine );

	resource = resourceLib.getString( IDS_APC5 );
	pMachine = _NEW( Machine( MachPhys::APC, 0, 5, 1, resource ) );
	machineData_.push_back( pMachine );

	machDataIter_ = machineData_.begin();
}

MachPhysMachine* PedMachineEditor::createMachine( W4dEntity& pEntity, const MexTransform3d& locTransform, const Machine& mach )
{
	// Create a new entity

	MachPhysMachine* pCurrentMachine = NULL;

	switch ( mach.machineType_ )
	{
		case MachPhys::AGGRESSOR:
			pCurrentMachine = _NEW( MachPhysAggressor( 	&pEntity, locTransform,
														( MachPhys::AggressorSubType )mach.subType_,
														mach.bodyLevel_, mach.brainboxLevel_, race_,
														mach.weaponCombo_ ) );
			break;
		case MachPhys::GEO_LOCATOR:
			pCurrentMachine = _NEW( MachPhysGeoLocator( &pEntity, locTransform, mach.bodyLevel_,
														mach.brainboxLevel_, race_ ) );
			break;
		case MachPhys::SPY_LOCATOR:
			pCurrentMachine = _NEW( MachPhysSpyLocator( &pEntity, locTransform, mach.bodyLevel_,
														mach.brainboxLevel_, race_ ) );

			break;
	   	case MachPhys::CONSTRUCTOR:
			pCurrentMachine = _NEW( MachPhysConstructor( &pEntity, locTransform,
														( MachPhys::ConstructorSubType ) mach.subType_,
														mach.bodyLevel_, mach.brainboxLevel_, race_ ) );
			break;
		case MachPhys::ADMINISTRATOR:
			pCurrentMachine = _NEW( MachPhysAdministrator( 	&pEntity, locTransform,
														( MachPhys::AdministratorSubType )mach.subType_,
														mach.bodyLevel_, mach.brainboxLevel_, race_,
														mach.weaponCombo_ ) );

			break;
		case MachPhys::RESOURCE_CARRIER:
			pCurrentMachine = _NEW( MachPhysResourceCarrier( &pEntity, locTransform, mach.bodyLevel_,
															mach.brainboxLevel_, race_ ) );

			break;
		case MachPhys::APC:
			pCurrentMachine = _NEW( MachPhysAPC( &pEntity, locTransform, mach.bodyLevel_,
												mach.brainboxLevel_, race_ ) );

			break;
		case MachPhys::TECHNICIAN:
			pCurrentMachine = _NEW( MachPhysTechnician( &pEntity, locTransform,
														( MachPhys::TechnicianSubType ) mach.subType_,
														mach.bodyLevel_, mach.brainboxLevel_, race_ ) );

			break;
		default:
			ASSERT( true, "Unknown machine type" );
	}
	pCurrentMachine->id( PedActorEditor::MACHINE );

	return pCurrentMachine;
}

void PedMachineEditor::createEntity( W4dEntity& pEntity, const MexTransform3d& locTransform )
{
	TEST_INVARIANT;
	//
 	// Create new machine, new scnMachine and new mapping.
	//

 	// Create new machine and new mapping from data pointed to by _machDatIter
	pSelectedMachine_ = createMachine( pEntity, locTransform, **machDataIter_ );

	// Copy machine data to scenario file format	TBD add constructor to PedScenarioFile::Machine
	const Machine& machine = **machDataIter_;
	PedScenarioFile::Machine scnMachine;

	// Copy legal machine data to Scenario file format
	scnMachine.type = machine.machineType_;
	scnMachine.subType = machine.subType_;
	scnMachine.brainboxLevel = machine.brainboxLevel_;
	scnMachine.bodyLevel = machine.bodyLevel_;
	scnMachine.weaponCombo = machine.weaponCombo_;
	scnMachine.position = pSelectedMachine_->globalTransform().position();
	scnMachine.race = pSelectedMachine_->race();
	//Create new mapping for new machine
	createMapping( pSelectedMachine_, *machDataIter_, scnMachine );
	highlightSelected();

	TEST_INVARIANT;
}

void PedMachineEditor::createMapping( MachPhysMachine* machine, Machine* legalData, PedScenarioFile::Machine scnMachine )
{
	TEST_INVARIANT;
	// Create map element and add to map list.
	MachineMapping pMap( machine, legalData, scnMachine );
	machineMap_.push_back( pMap );
	mapIterator_ = machineMap_.end()-1;
	TEST_INVARIANT;
}

ostream& operator <<( ostream& o, const PedMachineEditor& t )
{

    o << "PedMachineEditor " << (void*)&t << " start" << std::endl;
    o << "PedMachineEditor " << (void*)&t << " end" << std::endl;

    return o;
}


/* End EDITCONSTR.CPP ***********************************************/
