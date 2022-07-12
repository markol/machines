/*
 * E D I T C O N S T R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editconstr.hpp"
#include "planeted/wdebug.hpp"

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "stdlib/string.hpp"
#include "afx/resource.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/mouse.hpp"
#include "utility/linetok.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "world4d/scenemgr.hpp"
#include "machphys/machphys.hpp"
#include "machphys/constron.hpp"
#include "machphys/pod.hpp"
#include "machphys/mine.hpp"
#include "machphys/smelter.hpp"
#include "machphys/hwlab.hpp"
#include "machphys/garrison.hpp"
#include "machphys/factory.hpp"
#include "machphys/beacon.hpp"
#include "machphys/missemp.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/consdata.hpp"
#include "machphys/marker.hpp"
#include "planeted/strings.hpp"
#include "planeted/race.hpp"


PedConstructionEditor::PedConstructionEditor()
: PedActorEditor( "construction" ),
  pSelectedConstruction_( NULL )
{
	initialiseActors();
    TEST_INVARIANT;
}

PedConstructionEditor::~PedConstructionEditor()
{
    TEST_INVARIANT;

	for ( Constructions::iterator i = constructionData_.begin(); i != constructionData_.end();
		  ++i )
  	{
  		_DELETE( *i );
	}
	for( mapIterator_ = constructionMap_.begin(); mapIterator_ != constructionMap_.end();
		++mapIterator_ )
	{
		_DELETE( (*mapIterator_).construction_ );
	}
}

// virtual
void PedConstructionEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
   	INVARIANT( ( pSelectedConstruction_ == (*mapIterator_).construction_ ) or
   			   ( pSelectedConstruction_ == NULL ) );
}

// virtual
void PedConstructionEditor::processInput( const DevButtonEvent& devButtonEvent )
{
	TEST_INVARIANT;

	PedActorEditor::processInput( devButtonEvent );
	if ( devButtonEvent.action() == DevButtonEvent::PRESS and active_ )
	{
		if ( devButtonEvent.scanCode() == DevKey::KEY_L )
		{
			processRotation( true );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_K )
		{
			processRotation( false );
		}
	}
	if ( 	devButtonEvent.scanCode() == DevKey::KEY_1 and
				not devButtonEvent.wasShiftPressed() and
				devButtonEvent.wasCtrlPressed() )
	{
		processHide( hidden_ );
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::displayKeyboardCtrls()
{
	PRE( pSceneManager_ != NULL );

	pSceneManager_->out() << "L : rotate " << actorType_ << " clockwise" << std::endl;
	pSceneManager_->out() << "K : rotate " << actorType_ << " anti-clockwise" << std::endl;
	pSceneManager_->out() << "C : race " << std::endl;

   	PedActorEditor::displayKeyboardCtrls();

	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::displayModeInfo()
{
	PedActorEditor::displayModeInfo();
	pSceneManager_->out() << "Construction: " << (*conDataIter_)->description_ << std::endl;
	pSceneManager_->out() << "Race: " << race_ << std::endl;
}

// virtual
void PedConstructionEditor::readScnFile( PedScenarioFile& scenarioFile )
{
	TEST_INVARIANT;

	// retrieve list of constructions from scenario file for a specific race
	PedScenarioFile::Constructions scnConstructions = scenarioFile.constructions();

	for ( PedScenarioFile::Constructions::iterator scnConIter = scnConstructions.begin();
		  scnConIter != scnConstructions.end(); ++scnConIter )
	{
		// Create construction
		W4dEntity* domain = NULL;
	    const MexTransform3d localTransform = localTransformAtPoint( &domain, (*scnConIter).position );
		ASSERT( domain != NULL, "for position:" );


		// Set iterator to legal construction data to point to correct set of construction data
		for( conDataIter_ = constructionData_.begin(); conDataIter_ != constructionData_.end(); ++conDataIter_ )
		{
			if 	( ( (*conDataIter_ )->constructionType_ == (*scnConIter).type )
				and ( (*conDataIter_ )->subType_ == (*scnConIter).subType )
				and ( (*conDataIter_ )->hardwareLevel_ == (*scnConIter).hardwareLevel )
				and	( (*conDataIter_ )->weaponCombo_ == (*scnConIter).weaponCombo ) )
			{
				break;
			}
		}
		// Set current race in order to create construction of the correct race
		race_ = (*scnConIter).race;
		// Create new construction ( using conDataIter_ ) and new mapping
		pSelectedConstruction_ = createConstruction( *domain, localTransform, **conDataIter_ );

		createMapping( pSelectedConstruction_, *conDataIter_, *scnConIter );

		rotateAfterMove();
	}
	changingMode();
	TEST_INVARIANT;
}

void PedConstructionEditor::writeScnFile( PedScenarioFile& scenarioFile )
{
	// Construct construction list to pass to scenarioFile
	PedScenarioFile::Constructions constructions;

	for( ConstructionMappings::iterator i = constructionMap_.begin(); i != constructionMap_.end(); ++i )
	{
		// Take account of any dragging/moving, etc
		(*i).scnConstruction_.position = (*i).construction_->globalTransform().position();
		constructions.push_back( (*i).scnConstruction_ );
	}
	scenarioFile.constructions( constructions );
}

// virtual
W4dEntity* PedConstructionEditor::currentActor()
{
	TEST_INVARIANT;
	return pSelectedConstruction_;
}

// virtual
void PedConstructionEditor::processCycle( PedActorEditor::CycleDir dir)
{
	TEST_INVARIANT;
	switch( dir )
	{
	   	case NEXT:
			if (conDataIter_ == (constructionData_.end() - 1) )
				conDataIter_ = constructionData_.begin();
			else ++conDataIter_;
			break;

		case PREV:
			if ( conDataIter_ == constructionData_.begin() )
				conDataIter_ = (constructionData_.end() - 1);
			else --conDataIter_;
			break;
	}
	if ( pSelectedConstruction_ )
	{
	   W4dEntity* domain = pSelectedConstruction_->pParent();
	   const MexTransform3d locTransform = pSelectedConstruction_->localTransform();
	   processDelete();
	   createEntity( *domain, locTransform );
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::processSelection()
{
	TEST_INVARIANT;
	W4dEntity* actor = NULL;

	if ( actorSelected( &actor ) )
	{
		if ( ( ( actor->id() == PedActorEditor::CONSTRUCTION ) and ( not alreadySelected_ ) ) or
			( ( actor->id() == PedActorEditor::CONSTRUCTION ) and ( alreadySelected_ ) and ( actor != pSelectedConstruction_ ) ) )

		{
			// actor is a construction of some sort
 			pSelectedConstruction_ = ( MachPhysConstruction* ) actor;	// Safe since we know entity is a MachPhysConstructor

			highlightSelected();

			pSelectedConstruction_->solid( W4dEntity::NOT_SOLID );

			// lookup selected construction in map and set correct position
			// in construction data list
 			for ( mapIterator_ = constructionMap_.begin(); mapIterator_ != constructionMap_.end();
 				  ++mapIterator_ )
			{
				if ( (*mapIterator_).construction_ == pSelectedConstruction_ )
				{
					for ( conDataIter_ = constructionData_.begin(); conDataIter_!= constructionData_.end();
						  conDataIter_++ )
					{
						if ( (*mapIterator_).data_ == *conDataIter_ )
							break;
					}
					break;
				}
			}
			alreadySelected_ = true;
		}
		else if  ( ( actor->id() == PedActorEditor::CONSTRUCTION ) and ( alreadySelected_ ) and ( actor == pSelectedConstruction_ ) )
		{
			pSelectedConstruction_->solid( W4dEntity::NOT_SOLID );
			mouseDrag_ = true;
		}
		else
		{
			// mouse pointer not pointing at object - so deselect selected object(s)
			_DELETE( pMarker_ );
			pMarker_ = NULL;
			pSelectedConstruction_ = NULL;
			alreadySelected_ = false;
			mouseDrag_ = false;
		}
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::processDelete()
{
	TEST_INVARIANT;
	if ( pSelectedConstruction_ != NULL )
	{
		const ConstructionMapping& map = *mapIterator_;

		ASSERT( pSelectedConstruction_ == map.construction_, "" );
		_DELETE( pMarker_ );
		pMarker_ = NULL;
		_DELETE( map.construction_ );
		pSelectedConstruction_ = NULL;	// Since was pointing to map.construction_
		constructionMap_.erase( mapIterator_ );
		mapIterator_ = constructionMap_.begin(); // To prevent dangling iterator
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::processRace()
{
	TEST_INVARIANT;
	PedActorEditor::processRace();

	if ( pSelectedConstruction_ )
	{
		ASSERT(  pSelectedConstruction_ == (*mapIterator_).construction_ , "" );
		(*mapIterator_).scnConstruction_.race = race_;
		pSelectedConstruction_->changeRace( race_ );
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::processRotation( bool clockwise )
{
	TEST_INVARIANT;
	if ( pSelectedConstruction_ )
	{
		ASSERT( pSelectedConstruction_ == (*mapIterator_).construction_, "" );

	 	MexDegrees orientation = ( clockwise ) ? -90 : 90 ;
		(*mapIterator_).scnConstruction_.orientation += orientation;
		MATHEX_SCALAR result = (*mapIterator_).scnConstruction_.orientation.asScalar();
		if ( ( result > 270 ) or ( result < -270 ) )
			(*mapIterator_).scnConstruction_.orientation = 0;
 		MexTransform3d rotateTransform = pSelectedConstruction_->localTransform();
		MexTransform3d rotateZ90( MexEulerAngles( orientation, 0, 0 ) );
		rotateTransform.transform( rotateZ90 );
		pSelectedConstruction_->localTransform( rotateTransform );
	}
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::processHidden( bool hidden )
{
	TEST_INVARIANT;
  	for ( ConstructionMappings::iterator i = constructionMap_.begin(); i != constructionMap_.end(); i++ )
  		  (*i).construction_->visible( not hidden );

	PedActorEditor::processHide( hidden );
	TEST_INVARIANT;
}

// virtual
void PedConstructionEditor::changeAllSolidities( W4dEntity::Solidity solidity )
{
	for ( 	ConstructionMappings::iterator i = constructionMap_.begin();
			i != constructionMap_.end(); i++ )
			(*i).construction_->solid( solidity );
}

void PedConstructionEditor::initialiseActors()
{
	TEST_INVARIANT;
	AfxResourceLib resourceLib("machstrg.xml");

	Construction* pConstruction = NULL;
	string resource;

	resource = resourceLib.getString( IDS_POD1 );
	pConstruction = _NEW( Construction( MachPhys::POD, 0, 1, resource,
					MachPhys::T_ION_ORBITAL_CANNON ) );
	constructionData_.push_back( pConstruction );

 	resource = resourceLib.getString( IDS_MINE1 );
	pConstruction = _NEW( Construction( MachPhys::MINE, 0, 1, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MINE3 );
	pConstruction = _NEW( Construction( MachPhys::MINE, 0, 3, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MINE5 );
	pConstruction = _NEW( Construction( MachPhys::MINE, 0, 5, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_SMELTER1 );
	pConstruction = _NEW( Construction( MachPhys::SMELTER, 0, 1, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_SMELTER3 );
	pConstruction = _NEW( Construction( MachPhys::SMELTER, 0, 3, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_HARDWARE_LAB_CIVILIAN1 );
	pConstruction = _NEW( Construction( MachPhys::HARDWARE_LAB, (int) MachPhys::LAB_CIVILIAN, 1,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_HARDWARE_LAB_CIVILIAN3 );
	pConstruction = _NEW( Construction( MachPhys::HARDWARE_LAB, (int) MachPhys::LAB_CIVILIAN, 3,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_HARDWARE_LAB_MILITARY1 );
	pConstruction = _NEW( Construction( MachPhys::HARDWARE_LAB, (int) MachPhys::LAB_MILITARY, 1,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_HARDWARE_LAB_MILITARY3 );
	pConstruction = _NEW( Construction( MachPhys::HARDWARE_LAB, (int) MachPhys::LAB_MILITARY, 3,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_GARRISON1 );
	pConstruction = _NEW( Construction( MachPhys::GARRISON, 0, 1,
						resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_CIVILIAN1 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::CIVILIAN, 1,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_CIVILIAN3 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::CIVILIAN, 3,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_CIVILIAN5 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::CIVILIAN, 5,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_MILITARY1 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::MILITARY, 1,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_MILITARY3 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::MILITARY, 3,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_MILITARY4 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::MILITARY, 4,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_FACTORY_MILITARY5 );
	pConstruction = _NEW( Construction( MachPhys::FACTORY, (int) MachPhys::MILITARY, 5,
					  	resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_BEACON1 );
 	pConstruction = _NEW( Construction( MachPhys::BEACON, 0, 1, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_BEACON3 );
	pConstruction = _NEW( Construction( MachPhys::BEACON, 0, 3, resource ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_TURRET1 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::TURRET, 1,
					  	resource, MachPhys::T_FLAME_THROWER1 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_TURRET2 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::TURRET, 2,
					  	resource, MachPhys::LR_AUTO_CANNON_X2 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_TURRET3 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::TURRET, 3,
					  	resource, MachPhys::LR_PULSE_RIFLE_X2 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_SENTRY3 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::SENTRY, 3,
					  	resource, MachPhys::LR_MULTI_LAUNCHER5_X2 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_SENTRY4 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::SENTRY, 4,
					  	resource, MachPhys::LR_MULTI_LAUNCHER6_X2 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_LAUNCHER4 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::LAUNCHER, 4,
					  	resource, MachPhys::LR_LARGE_MISSILE_X2 ) );
	constructionData_.push_back( pConstruction );

	resource = resourceLib.getString( IDS_MISSILE_EMPLACEMENT_ICBM5 );
	pConstruction = _NEW( Construction( MachPhys::MISSILE_EMPLACEMENT, (int) MachPhys::ICBM, 5,
					  	resource, MachPhys::T_NUCLEAR_MISSILE ) );
	constructionData_.push_back( pConstruction );


	conDataIter_ = constructionData_.begin();
	TEST_INVARIANT;
}

MachPhysConstruction* PedConstructionEditor::createConstruction( W4dEntity& pEntity, const MexTransform3d& locTransform, const Construction& con )
{
	TEST_INVARIANT;
	MachPhysConstruction* pCurrentConstruction = NULL;
	// Create a physical construction in world from Construction data passed in
	switch ( con.constructionType_ )
	{
		case MachPhys::POD:
			pCurrentConstruction = _NEW( MachPhysPod( &pEntity, locTransform,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::MINE:
			pCurrentConstruction = _NEW( MachPhysMine( &pEntity, locTransform,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::SMELTER:
			pCurrentConstruction = _NEW( MachPhysSmelter( &pEntity, locTransform,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::HARDWARE_LAB:
			pCurrentConstruction = _NEW( MachPhysHardwareLab( &pEntity, locTransform,
								( MachPhys::HardwareLabSubType ) con.subType_,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::GARRISON:
			pCurrentConstruction = _NEW( MachPhysGarrison( &pEntity, locTransform,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::FACTORY:
			pCurrentConstruction = _NEW( MachPhysFactory( &pEntity, locTransform,
								( MachPhys::FactorySubType ) con.subType_,
								con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::BEACON:
			pCurrentConstruction = _NEW( MachPhysBeacon( &pEntity, locTransform,
										con.hardwareLevel_, race_ ) );
			break;
		case MachPhys::MISSILE_EMPLACEMENT:
			pCurrentConstruction = _NEW( MachPhysMissileEmplacement( &pEntity, locTransform,
							( MachPhys::MissileEmplacementSubType ) con.subType_,
							con.hardwareLevel_, race_, con.weaponCombo_ ) );
			break;
	}
	pCurrentConstruction->percentageComplete( 100.0 );
	pCurrentConstruction->id( PedActorEditor::CONSTRUCTION );

	TEST_INVARIANT;
	return pCurrentConstruction;
}

ostream& operator <<( ostream& o, const PedConstructionEditor& t )
{

    o << "PedConstructionEditor " << (void*)&t << " start" << std::endl;
    o << "PedConstructionEditor " << (void*)&t << " end" << std::endl;

    return o;
}

void PedConstructionEditor::createEntity( W4dEntity& pEntity, const MexTransform3d& locTransform )
{
	TEST_INVARIANT;
	//
 	// Create new construction, new scnConstruction and new mapping.
	//

 	// Create new construction and new mapping from data pointed to by _conDatIter
	pSelectedConstruction_ = createConstruction( pEntity, locTransform, **conDataIter_ );

	// Copy construction data to scenario file format	TBD add constructor to PedScenarioFile::Construction
	const Construction& construction = **conDataIter_;
	PedScenarioFile::Construction scnConstruction;

	// Copy legal construction data to Scenario file format
	scnConstruction.type = construction.constructionType_;
	scnConstruction.subType = construction.subType_;
	scnConstruction.hardwareLevel = construction.hardwareLevel_;
	scnConstruction.weaponCombo = construction.weaponCombo_;
	scnConstruction.position = pSelectedConstruction_->globalTransform().position();
	scnConstruction.orientation = pSelectedConstruction_->globalTransform().rotationAsEulerAngles().roll();
	scnConstruction.race = pSelectedConstruction_->race();

	//Create new mapping for new construction
	createMapping( pSelectedConstruction_, *conDataIter_, scnConstruction );
	highlightSelected();

	TEST_INVARIANT;
}

void PedConstructionEditor::rotateAfterMove()
{
	TEST_INVARIANT;
	// Rotates actor after  it has undergone a transform
	PRE( pSelectedConstruction_ != NULL );

	ASSERT( pSelectedConstruction_ == (*mapIterator_).construction_, "" );
	if ( (*mapIterator_).scnConstruction_.orientation.asScalar() )
	{
		MexTransform3d rotateTransform = pSelectedConstruction_->localTransform();
 		MexTransform3d rotateZ90( MexEulerAngles( (*mapIterator_).scnConstruction_.orientation, 0, 0 ) );
		rotateTransform.transform( rotateZ90 );
		pSelectedConstruction_->localTransform( rotateTransform );
	}
	TEST_INVARIANT;
}

void PedConstructionEditor::createMapping( MachPhysConstruction* construction, Construction* legalData,
											PedScenarioFile::Construction scnConstruction )
{
	TEST_INVARIANT;
	// Create map element and add to map list.
	ConstructionMapping pMap( construction, legalData, scnConstruction );
	constructionMap_.push_back( pMap );
	mapIterator_ = constructionMap_.end()-1;
	TEST_INVARIANT;
}

/* End EDITCONSTR.CPP ***********************************************/
