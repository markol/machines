/*
 * E D I T A R T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/editart.hpp"
#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "device/keyboard.hpp"
#include "device/eventq.hpp"
#include "device/mouse.hpp"
#include "utility/linetok.hpp"
#include "afx/resource.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/degrees.hpp"
#include "mathex/eulerang.hpp"
#include "machphys/marker.hpp"
#include "machphys/artefact.hpp"
#include "machphys/terrtile.hpp"
#include "world4d/scenemgr.hpp"
#include "planeted/scenfile.hpp"


PedArtefactEditor::PedArtefactEditor()
:	PedActorEditor( "artefact" ),
	pSelectedArtefact_( NULL ),
    artefactIndex_( 0 )
{
	initialiseActors();
    TEST_INVARIANT;
}

PedArtefactEditor::~PedArtefactEditor()
{
    TEST_INVARIANT;

	/*for ( 	Artefacts::iterator i = artefactData_.begin();
			i != artefactData_.end(); i++ )
  		_DELETE( *i );	  */

	deleteArtefacts();
}

// virtual
void PedArtefactEditor::CLASS_INVARIANT
{
    INVARIANT( this != NULL )
}

// virtual
void PedArtefactEditor::processInput( const DevButtonEvent& devButtonEvent )
{
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
		else if ( devButtonEvent.scanCode() == DevKey::KEY_J  and
			 	  not devButtonEvent.wasShiftPressed() )
		{
			processHeightChange( 1 );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_J  and
			 	  devButtonEvent.wasShiftPressed() )
		{
			processHeightChange( 10 );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_N  and
			 	  not devButtonEvent.wasShiftPressed() )
		{
			processHeightChange( -1 );
		}
		else if ( devButtonEvent.scanCode() == DevKey::KEY_N  and
			 	  devButtonEvent.wasShiftPressed() )
		{
			processHeightChange( -10 );
		}
	}
	if ( 	devButtonEvent.scanCode() == DevKey::KEY_3 and
				not devButtonEvent.wasShiftPressed() and
				devButtonEvent.wasCtrlPressed() )
	{
		processHide( hidden_ );
	}
}

// virtual
void PedArtefactEditor::displayModeInfo( void )
{
	PedActorEditor::displayModeInfo();
	if ( artefactStore_.nGenerics() )
		pSceneManager_->out() << "Artefact: " << artefactStore_.key( artefactIndex_ ) << std::endl;
	if( pSelectedArtefact_ )
	{
		pSceneManager_->out() << "Height: " << pSelectedArtefact_->globalTransform().position().z() << std::endl;
	}
}

// virtual
void PedArtefactEditor::displayKeyboardCtrls( void )
{
	PRE( pSceneManager_ != NULL );

	pSceneManager_->out() << "L : rotate " << actorType_ << " clockwise" << std::endl;
	pSceneManager_->out() << "K : rotate " << actorType_ << " anti-clockwise" << std::endl;
	pSceneManager_->out() << "J ( + shift ): increase height 1m ( 10m )" << std::endl;
	pSceneManager_->out() << "N ( + shift ): decrease height 1m ( 10m )" << std::endl;
   	PedActorEditor::displayKeyboardCtrls();

	TEST_INVARIANT;
}

// virtual
void PedArtefactEditor::readScnFile( PedScenarioFile& scenarioFile )
{
	if( ( scenarioFile.arfFilename().c_str() != "" ) and ( scenarioFile.arfFilename().existsAsFile() ) )
	{
		readArfFile( scenarioFile.arfFilename() );
		PedScenarioFile::Artefacts artefacts = scenarioFile.artefacts();

		for( PedScenarioFile::Artefacts::iterator i = artefacts.begin(); i != artefacts.end(); ++i )
		{
			W4dEntity* dom = NULL;
		    const MexTransform3d localTransform = localTransformAtPoint( &dom, (*i).position );
			ASSERT( dom != NULL, "for position:" );
			createEntity( *dom, localTransform, (*i).key, (*i).orientation );
			rotateAfterMove();
		}
		changingMode();
	}
}

// virtual
void PedArtefactEditor::writeScnFile( PedScenarioFile& scenarioFile )
{
	// Construct list of artefacts to pass to scenario file
	PedScenarioFile::Artefacts artefacts;

	uint nArtefactGroups = artefactStore_.nGenerics();
	if ( nArtefactGroups )	// determine if there is any artefact data
	{
		for ( int col = 0; col < nArtefactGroups; col++ )
		{
			uint nArtefacts = artefacts_[ col ].size();
			if ( nArtefacts )
			{
				ASSERT( artefactIndex_ < artefactStore_.nGenerics(), "" );
				for ( 	ArtefactGroup::iterator row = artefacts_[ col ].begin();
						row != artefacts_[ col ].end(); row++ )
				{
					PedScenarioFile::Artefact artefact;
					W4dEntity* pArtefact = (*row).artefact_;

					artefact.key = artefactStore_.key( col );
					artefact.position = pArtefact->globalTransform().position();
					artefact.orientation = (*row).orientation_;
					artefacts.push_back( artefact );
				}
			}
		}
		// Override scn loaded artefacts only when data is specified
		scenarioFile.artefacts( artefacts );
	}
}

void PedArtefactEditor::readArfFile( const SysPathName& arfFileName )
{
	// reads available artefacts from file and sets up empty artefact vector of vectors.
	PRE( arfFileName.existsAsFile() );

	artefactStore_.readGenericFile( arfFileName );
	uint nArtefactTypes = artefactStore_.nGenerics();
	ArtefactGroup group;

	for ( uint col = 0; col < nArtefactTypes; col++ )
		artefacts_.push_back( group );

	arfFileName_ = arfFileName;
	POST( artefactStore_.nGenerics() );
}

// virtual
W4dEntity* PedArtefactEditor::currentActor( void )
{
	return pSelectedArtefact_;
}

// virtual
void PedArtefactEditor::processCycle( PedActorEditor::CycleDir dir)
{
	if ( artefactStore_.nGenerics() )
	{
		uint nArtefacts = artefactStore_.nGenerics();
		switch( dir )
		{
		   	case NEXT:
		   		++artefactIndex_;
				if ( artefactIndex_ == nArtefacts )
					artefactIndex_ = 0;
				break;

			case PREV:
	 			if ( artefactIndex_ == 0 )
					artefactIndex_ = nArtefacts - 1;
				else
					--artefactIndex_;
	 			break;
		}
		if ( pSelectedArtefact_ )
		{
		   W4dEntity* domain = pSelectedArtefact_->pParent();
		   const MexTransform3d locTransform = pSelectedArtefact_->localTransform();
		   processDelete();
		   createEntity( *domain, locTransform );
		}
	}
}

// virtual
void PedArtefactEditor::processSelection( void )
{
	if ( artefactStore_.nGenerics() )
	{
		W4dEntity* actor = NULL;

		if ( actorSelected( &actor ) )
		{
			// If artefact is being selected for first time, or an artefact has been selected, but it is a different one
			// to the one now selected
			if ( ( ( actor->id() == PedActorEditor::ARTEFACT ) and ( not alreadySelected_ ) ) or
			( ( actor->id() == PedActorEditor::ARTEFACT ) and ( alreadySelected_ ) and ( actor != pSelectedArtefact_ ) ) )
			{
				// actor is a artefact of some sort
	 			pSelectedArtefact_ = actor;	// Safe since we know entity is a Artefact

				highlightSelected();

				pSelectedArtefact_->solid( W4dEntity::NOT_SOLID );
				alreadySelected_ = true;
			}
			// if artefact is being selected for second time, signal ok to drag
			else if ( ( actor->id() == PedActorEditor::ARTEFACT ) and ( alreadySelected_ ) and ( actor == pSelectedArtefact_ ) )
			{
				pSelectedArtefact_->solid( W4dEntity::NOT_SOLID );
				mouseDrag_ = true;
			}
			else
			{
				// mouse pointer not pointing at object - so deselect selected object(s)
				_DELETE( pMarker_ );
				pMarker_ = NULL;
				pSelectedArtefact_ = NULL;
				alreadySelected_ = false;
				mouseDrag_ = false;
			}
		}
	}
}

// virtual
void PedArtefactEditor::processRotation( bool clockwise )
{
	if ( artefactStore_.nGenerics() )
	{
		artefactIndex index = storeIndex( pSelectedArtefact_ );
		ArtefactMapping map = *(index.row_);
	 	MexDegrees orientation = ( clockwise ) ? -90 : 90 ;
		map.orientation_ += orientation;
 		MATHEX_SCALAR result = map.orientation_.asScalar();
	   	if ( ( result > 270 ) or ( result < -270 ) )
			map.orientation_ = 0;
		*(index.row_) = map;
		MexTransform3d rotateTransform = pSelectedArtefact_->localTransform();
		MexTransform3d rotateZ90( MexEulerAngles( orientation, 0, 0 ) );
		rotateTransform.transform( rotateZ90 );
		pSelectedArtefact_->localTransform( rotateTransform );
	}
}

// virtual
void PedArtefactEditor::processDelete( void )
{
	if ( artefactStore_.nGenerics() )
	{
		if ( pSelectedArtefact_ != NULL )
		{
			_DELETE( pMarker_ );
			pMarker_ = NULL;

	 		// Retrieve ptr to artefact and delete artefact
			artefactIndex index = storeIndex( pSelectedArtefact_ );
		   	_DELETE( pSelectedArtefact_ );

			// remove ptr from current artefact list
		   	(*( index.col_ )).erase( index.row_ );
	  		pSelectedArtefact_ = NULL;
		}
	}
}

// virtual
void PedArtefactEditor::processRace( void )
{
}

// virtual
void PedArtefactEditor::processHide( bool hidden )
{
	for ( ArtefactTable::iterator col = artefacts_.begin(); col != artefacts_.end(); col++ )
		for ( ArtefactGroup::iterator row = (*col).begin(); row != (*col).end(); row++ )
			( *row ).artefact_->visible( not hidden );
	PedActorEditor::processHide( hidden );
}

// virtual
void PedArtefactEditor::rotateAfterMove( void )
{
	if ( artefactStore_.nGenerics() )
	{
		// Rotates actor after  it has undergone a transform
		PRE( pSelectedArtefact_ != NULL );

		// lookup selected artefact in map
		artefactIndex i = storeIndex( pSelectedArtefact_ );

		MexTransform3d rotateTransform = pSelectedArtefact_->localTransform();
		MexTransform3d rotateZ90( MexEulerAngles( (*i.row_).orientation_, 0, 0 ) );
		rotateTransform.transform( rotateZ90 );
		pSelectedArtefact_->localTransform( rotateTransform );
	}
}

void PedArtefactEditor::initialiseActors( void )
{
}

void PedArtefactEditor::createEntity( W4dEntity& Entity, const MexTransform3d& locTransform )
{
	// Create a new entity, set it to be the selected entity
	if ( artefactStore_.nGenerics() )
	{
		ArtefactMapping currentArtefact;
		ASSERT( artefactIndex_ < artefactStore_.nGenerics(), "" );
		const string& key = artefactStore_.key( artefactIndex_ );
		currentArtefact.artefact_ = artefactStore_.cloneEntity( key, &Entity, locTransform );
		currentArtefact.artefact_->id( PedActorEditor::ARTEFACT );
		pSelectedArtefact_ = currentArtefact.artefact_;
		artefacts_[ artefactIndex_ ].push_back( currentArtefact );
		pSelectedArtefact_->solid( W4dEntity::SOLID );
		highlightSelected();
	}
}

void PedArtefactEditor::createEntity( W4dEntity& Entity, const MexTransform3d& locTransform, const string& key,
										MexDegrees orientation )
{
	// Create a new artefact, set it to be the selected artefact
	PRE( artefactStore_.nGenerics() );

	W4dEntity* pCurrentArtefact = NULL;
	bool artefactValid = false;
    uint nArtefactGroups = artefactStore_.nGenerics();

	uint artefactIndex = 0;
	// Determine artefact type to be created is in list of artefact types
	for ( artefactIndex = 0; artefactIndex < nArtefactGroups; artefactIndex++ )
	{
    	if ( artefactStore_.key( artefactIndex ) == key )
        {
			artefactValid = true;
        	break;
		}
    }
	ASSERT_INFO( key );
    ASSERT( artefactValid, "type of artefact to be created is not in list of available types" );
	ArtefactMapping currentArtefact;
	currentArtefact.artefact_ = artefactStore_.cloneEntity( key, &Entity, locTransform );
	ASSERT( currentArtefact.artefact_, "W4dGenericRepository::cloneEntity failed" );
	currentArtefact.orientation_ = orientation;
	currentArtefact.artefact_->id( PedActorEditor::ARTEFACT );
	pSelectedArtefact_ = currentArtefact.artefact_;
	artefacts_[ artefactIndex ].push_back( currentArtefact );
	pSelectedArtefact_->solid( W4dEntity::SOLID );
}

// virtual
void PedArtefactEditor::changeAllSolidities( W4dEntity::Solidity solidity)
{
	for ( ArtefactTable::iterator col = artefacts_.begin(); col != artefacts_.end(); col++ )
		for ( ArtefactGroup::iterator row = (*col).begin(); row != (*col).end(); row++ )
			( *row ).artefact_->solid( solidity );
}

//virtual
void PedArtefactEditor::processHeightChange( int deltaz )
{
	if( pSelectedArtefact_ )
	{
		MexTransform3d heightTrans = pSelectedArtefact_->localTransform();
		heightTrans.translate( MexPoint3d( 0, 0, deltaz ) );
		pSelectedArtefact_->localTransform( heightTrans );
	}
}

ostream& operator <<( ostream& o, const PedArtefactEditor& t )
{

    o << "PedArtefactEditor " << (void*)&t << " start" << std::endl;
    o << "PedArtefactEditor " << (void*)&t << " end" << std::endl;

    return o;
}

void PedArtefactEditor::deleteArtefacts( void )
{
	if ( artefactStore_.nGenerics() )
	{
		uint nArtefactTypes = artefacts_.size();

		for ( uint col = 0; col < nArtefactTypes; col++ )
		{
			uint nArtefacts = artefacts_[ col ].size();
			for ( uint row = 0; row < nArtefacts; row++ )
				_DELETE( artefacts_[ col ][ row ].artefact_ );
			artefacts_[ col ].empty();
		}
	}
}

PedArtefactEditor::artefactIndex PedArtefactEditor::storeIndex( W4dEntity* pSelectedArtefact )
{
	PRE( pSelectedArtefact != NULL );

	ArtefactTable::iterator col;
	ArtefactGroup::iterator row;
	bool indexFound = false;

	for ( col = artefacts_.begin(); col != artefacts_.end(); col++ )
	{
		for ( row = (*col).begin(); row != (*col).end(); row++ )
		{
			if ( (*row).artefact_ == pSelectedArtefact )
			{
				indexFound = true;
				return artefactIndex( col, row );
			}
		}
	}
	POST( indexFound );
	return artefactIndex( col, row );	// will never do this...
}


void PedArtefactEditor::createCeilingArtefact( const MachPhysTerrainTile& terrainTile )
{
	string key = "";
	key = tileToKey( terrainTile );
	if( key != "" )
	{
		createEntity( *(terrainTile.pParent()), terrainTile.localTransform(),
					  key, terrainTile.localTransform().rotationAsEulerAngles().roll() );
	}
}

void PedArtefactEditor::deleteCeilingArtefact( const MachPhysTerrainTile& terrainTile )
{
	string key = "";
	// Find type name of artefact
	key = tileToKey( terrainTile );

	if( key != "" )
	{
		bool artefactFound = false;
		uint nGenerics = artefactStore_.nGenerics();
		uint artefactGroup;
		// Determine position of artefact type in list of artefact types
		for( artefactGroup = 0; artefactGroup < nGenerics; ++artefactGroup )
		{
			WAYNE_STREAM( "artefactStore_.key( artefactGroup ) " << artefactStore_.key( artefactGroup ) << std::endl );
			WAYNE_STREAM( "key " << key << std::endl );
			if( artefactStore_.key( artefactGroup ) == key )
			{
				WAYNE_STREAM( "Group " << key << std::endl );
				break;
			}
		}
		// Use artefact type position to obtain list of artefacts of that type
		ArtefactGroup::iterator j;
		for( j = artefacts_[ artefactGroup ].begin();
			 j != artefacts_[ artefactGroup ].end(); ++j )
		{
			// is artefact the one positioned at tile coords
			if( (*j).artefact_->globalTransform().position() == terrainTile.globalTransform().position() )
			{
				WAYNE_STREAM( "Artefact found " << std::endl );
				artefactFound = true;
				break;
			}
		}
		if( artefactFound )
		{
			// delete instance of artefact, and then remove mapping info from artefact list
			_DELETE( (*j).artefact_ );
			artefacts_[ artefactGroup ].erase( j );
		}
	}
}

string PedArtefactEditor::tileToKey( const MachPhysTerrainTile& terrainTile )
{
	UtlLineTokeniser parser( arfFileName_ );

	bool done = false;
	string returnKey = "";

	while( not parser.finished() and not done )
	{
		ASSERT( parser.tokens().size() == 3, "" );
		SysPathName artefactFile( parser.tokens()[ 1 ] );
		SysPathName lodFile( terrainTile.lodFileName() );
		// remove extensions
		artefactFile.extension( "" );
		lodFile.extension( "" );
		string artefactString = *(artefactFile.components().end() - 1);
		string lodString = *(lodFile.components().end() - 1);
		if( artefactString == lodString )
		{
			returnKey = parser.tokens()[ 0 ];
			done = true;
		}
		else
		{
			parser.parseNextLine();
		}
	}
	return returnKey;
}

/* End EDITCONSTR.CPP ***********************************************/
