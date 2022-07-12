/*
 * E D I T A R T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedArtefactEditor

    Editor mode for artefacts
*/

#ifndef _EDITART_HPP
#define _EDITART_HPP

#include "planeted/editactor.hpp"
#include "ctl/pvector.hpp"
#include "ctl/vector.hpp"
#include "world4d/genrepos.hpp"
#include "machphys/machphys.hpp"
#include "mathex/degrees.hpp"


class MachPhysArtefact;
class MachPhysTerrainTile;
class W4dComposite;
class W4dEntity;
class MexTransform3d;
class ofstream;
class PedScenarioFile;

class PedArtefactEditor : public PedActorEditor
// Canonical form revoked
{
public:
    PedArtefactEditor();
    ~PedArtefactEditor();

	void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayModeInfo();
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

    //virtual void preRenderUpdate();
	//virtual void displayModeInfo();

	virtual void readScnFile( PedScenarioFile& );
	// PRE( cspFileName.existsAsFile() );

	virtual void writeScnFile( PedScenarioFile& );

	void displayKeyboardCtrls();
	//virtual void changingMode();

	void createCeilingArtefact( const MachPhysTerrainTile& terrainTile );

	void deleteCeilingArtefact( const MachPhysTerrainTile& terrainTile );

protected:

	void initialiseActors();
	virtual W4dEntity* currentActor();
	virtual void processCycle( PedActorEditor::CycleDir dir );
 	virtual void processSelection();
 	virtual void processDelete();
	virtual void processRace();
	virtual void processRotation( bool );
	virtual void createEntity( W4dEntity&, const MexTransform3d& );
	virtual void rotateAfterMove();
	virtual void processHide( bool hidden );
	virtual void changeAllSolidities( W4dEntity::Solidity );
	virtual void processHeightChange( int deltaz );

private:

	class ArtefactMapping
	{
		public:
		ArtefactMapping( W4dEntity* artefact = NULL, MexDegrees orientation = 0 )
		:	artefact_( artefact ),
			orientation_( orientation )
		{}
		W4dEntity* artefact_;
		MexDegrees orientation_;
	};

	typedef ctl_vector< ArtefactMapping > ArtefactGroup;
	typedef ctl_vector< ArtefactGroup > ArtefactTable;

	struct artefactIndex
	{
		artefactIndex( ArtefactTable::iterator col, ArtefactGroup::iterator row )
		:	col_( col ),
			row_( row )
		{}

		ArtefactGroup::iterator row_;
		ArtefactTable::iterator col_;
	};

    PedArtefactEditor( const PedArtefactEditor& );
    PedArtefactEditor& operator =( const PedArtefactEditor& );

	void createEntity(  W4dEntity&, const MexTransform3d&, const string& key, MexDegrees orientation );

//	void initialiseActors();
 	void deleteArtefacts();
   //	MachPhys::ArtefactType artefactType( const string& type );
	int subType( const string& type );
	artefactIndex storeIndex( W4dEntity* pSelectedArtefact );

	// Reads artefact file to populate artefact repository
	void readArfFile( const SysPathName& );

	// Converts between a lanscape tile's name and the ceiling artefact associated with	it
	// ( if there is one ) - else returns ""
	string tileToKey( const MachPhysTerrainTile& terrainTile );

    friend ostream& operator <<( ostream& o, const PedArtefactEditor& t );

	//Artefact artefactData_;

	W4dEntity* pSelectedArtefact_;
	W4dGenericRepository artefactStore_;
	ArtefactTable artefacts_;
	uint artefactIndex_;


	SysPathName arfFileName_ ;
};


#endif

/* End EDITART.HPP *************************************************/
