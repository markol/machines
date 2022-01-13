/*
 * E D I T C O N S T R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedConstructionEditor

    Construction mode edit.
*/

#ifndef _PLANETED_EDITCONSTR_HPP
#define _PLANETED_EDITCONSTR_HPP

#include "planeted/editactor.hpp"
#include "ctl/pvector.hpp"
#include "ctl/vector.hpp"
#include "device/butevent.hpp"
#include "mathex/degrees.hpp"
#include "planeted/scenfile.hpp"

class MachPhysConstruction;
class W4dComposite;
//class DevButtonEvent;
class SysPathName;
class ofstream;
class MexTransform3d;

class PedConstructionEditor	: public PedActorEditor
// Canonical form revoked
{
public:
    PedConstructionEditor( );
    ~PedConstructionEditor( void );

    void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );

	virtual void displayKeyboardCtrls( void );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );
	virtual void displayModeInfo( void );

	virtual void readScnFile( PedScenarioFile& );

	virtual void writeScnFile( PedScenarioFile& );

protected:

	void initialiseActors( void );	void processRotation( bool clockwise );
	virtual W4dEntity* currentActor( void );
	virtual void processCycle( PedActorEditor::CycleDir dir );
 	virtual void processSelection( void );
 	virtual void processDelete( void );
	virtual void processRace( void );
	virtual void createEntity( W4dEntity&, const MexTransform3d& );
	virtual void rotateAfterMove( void );
	virtual void processHidden( bool hidden );
	virtual void changeAllSolidities( W4dEntity::Solidity );

private:

	struct Construction
	{
		Construction( MachPhys::ConstructionType conType = MachPhys::HARDWARE_LAB, int subType = 0,
					  size_t hardwareLevel = 1, string description = "",
					  MachPhys::WeaponCombo weaponCombo = MachPhys::L_BOLTER )
		: constructionType_( conType ),
		  subType_( subType ),
		  hardwareLevel_(hardwareLevel),
		  description_( description ),
		  weaponCombo_( weaponCombo )
		{}

 		MachPhys::ConstructionType constructionType_;
		int subType_;
		size_t hardwareLevel_;
		string description_;
		MachPhys::WeaponCombo weaponCombo_;
	};

	// Mapping between physical construction, logical construction, and scenario file data for construction
	struct ConstructionMapping
	{
		ConstructionMapping()
		{}
		ConstructionMapping( MachPhysConstruction* con, Construction* data, PedScenarioFile::Construction ScnCon )
		: construction_( con ),
		  data_( data ),
		  scnConstruction_( ScnCon )
		{}
		MachPhysConstruction* construction_;
		Construction* data_;
		PedScenarioFile::Construction scnConstruction_;
	};

	typedef ctl_pvector< Construction > Constructions;
 	typedef ctl_vector< ConstructionMapping > ConstructionMappings;

    friend ostream& operator <<( ostream& o, const PedConstructionEditor& t );

    PedConstructionEditor( const PedConstructionEditor& );
    PedConstructionEditor& operator =( const PedConstructionEditor& );

	MachPhysConstruction* createConstruction( W4dEntity&, const MexTransform3d&, const Construction& );
	void createMapping( MachPhysConstruction*, Construction*, PedScenarioFile::Construction );

	// Data
	Constructions constructionData_; // List of all legal construction types with valid sub types, hardware levels, etc.
	Constructions::iterator conDataIter_; // Iterator into list of all legal construction types
	ConstructionMappings constructionMap_; // List of actual constructions each with a mapping to their type and orientation
	ConstructionMappings::iterator mapIterator_; // Iterator to construction map
	MachPhysConstruction* pSelectedConstruction_; // Iterator index into list of actual constructions
};


#endif

/* End EDITCONSTR.HPP ***********************************************/
