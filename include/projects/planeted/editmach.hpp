/*
 * E D I T M A C H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedMachineEditor

    Editor mode for machine of machines
*/

#ifndef _EDITMACH_HPP
#define _EDITMACH_HPP

#include "planeted/editactor.hpp"
#include "ctl/pvector.hpp"
#include "ctl/vector.hpp"
#include "planeted/scenfile.hpp"

class MachPhysMachine;
class W4dComposite;
class W4dEntity;
class MexTransform3d;
class ofstream;

class PedMachineEditor : public PedActorEditor
// Canonical form revoked
{
public:
    PedMachineEditor( void );
    ~PedMachineEditor( void );

	void CLASS_INVARIANT;

	virtual void processInput( const DevButtonEvent& );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayModeInfo( void );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

	virtual void displayKeyboardCtrls( void );
	// PRE( pSceneManager_ != NULL );
	// PRE( pPlanet_ != NULL );

    //virtual void preRenderUpdate( void );
//	virtual void displayModeInfo( void );

	virtual void readScnFile( PedScenarioFile& );

	virtual void writeScnFile( PedScenarioFile& );


protected:

	void initialiseActors( void );
	virtual W4dEntity* currentActor( void );
	virtual void processCycle( PedActorEditor::CycleDir dir );
 	virtual void processSelection( void );
 	virtual void processDelete( void );
	virtual void processRace( void );
	virtual void createEntity( W4dEntity&, const MexTransform3d& );
	virtual void rotateAfterMove( void );
	virtual void processHide( bool hidden );
	virtual void changeAllSolidities( W4dEntity::Solidity );

private:

	struct Machine
	{
		Machine( MachPhys::MachineType machType = MachPhys::AGGRESSOR,
					int subType = 0,
					size_t bodyLevel = 1,
					size_t brainboxLevel = 1,
					string description = "",
					MachPhys::WeaponCombo weaponCombo = MachPhys::L_BOLTER )
		:	machineType_( machType ),
			subType_( subType ),
			bodyLevel_( bodyLevel),
			brainboxLevel_( brainboxLevel ),
			description_( description ),
			weaponCombo_( weaponCombo )
		{}

		MachPhys::MachineType machineType_;
		int subType_;
		size_t bodyLevel_;
		size_t brainboxLevel_;
		string description_;
		MachPhys::WeaponCombo weaponCombo_;
	};

	struct MachineMapping
	{
		MachineMapping()
		{}
		MachineMapping( MachPhysMachine* machine, Machine* data, PedScenarioFile::Machine scnMach )
		:	machine_( machine ),
			data_( data ),
			scnMachine_( scnMach )
		{}

		MachPhysMachine* machine_;
		Machine* data_;
		PedScenarioFile::Machine scnMachine_;
	};

	typedef ctl_pvector< Machine > Machines;
 	typedef ctl_vector< MachineMapping > MachineMappings;

    PedMachineEditor( const PedMachineEditor& );
    PedMachineEditor& operator =( const PedMachineEditor& );

//	void initialiseActors();
 	void deleteMachines( void );
	MachPhys::MachineType machineType( const string& type );
	int subType( const string& type );

    friend ostream& operator <<( ostream& o, const PedMachineEditor& t );

	MachPhysMachine* createMachine( W4dEntity&, const MexTransform3d&, const Machine& );
	void createMapping( MachPhysMachine*, Machine*, PedScenarioFile::Machine );

	Machines machineData_;
	MachineMappings machineMap_;
	Machines::iterator machDataIter_;
	MachineMappings::iterator mapIterator_;
	MachPhysMachine* pSelectedMachine_;

};


#endif

/* End EDITMACH.HPP *************************************************/
