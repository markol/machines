/*
 * S C E N F I L E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedScenarioFile

    Manages reading and writing of scenario files
*/

#ifndef _PLANETED_SCENFILE_HPP
#define _PLANETED_SCENFILE_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "utility/linetok.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/degrees.hpp"
#include "machphys/machphys.hpp"
#include "machlog/camera.hpp"


//class UtlLineTokeniser;
class SysPathName;

class PedScenarioFile
// Canonical form revoked
{
public:
    PedScenarioFile();
    ~PedScenarioFile();

    void CLASS_INVARIANT;

	typedef string RaceAI;
	typedef string MineralSite;
	typedef string Miscellanious;
	typedef string AssemblyPoint;
	typedef string Action;
	typedef string Condition;

	// Structure for storing camera position
	struct Camera
	{
		Camera()
		:
		type( MachLogCamera::GROUND )
		{}
		MexTransform3d transform;
		MachLogCamera::Type type;
	};

	struct Construction
	{
		Construction( 	MachPhys::ConstructionType conType = MachPhys::HARDWARE_LAB,
						int sType = 0,
						size_t hwLevel = 1,
 						MachPhys::WeaponCombo weaponCom = MachPhys::L_BOLTER,
						MachPhys::Race rc = MachPhys::RED
 					 )
		:	type( conType ),
			subType( sType ),
			hardwareLevel( hwLevel) ,
			weaponCombo( weaponCom ),
			race( rc )
		{}
 		MachPhys::ConstructionType type;
		int subType;
		size_t hardwareLevel;
		MachPhys::WeaponCombo weaponCombo;
		MexPoint3d position;	// replace with a transform
		MexDegrees orientation;
		ctl_vector< string > build;
		MachPhys::Race race;
	};

	struct Machine
	{
		Machine( MachPhys::MachineType machType = MachPhys::AGGRESSOR,
					int subType = 0,
					size_t bLevel = 1,
					size_t bbLevel = 1,
					string desc = "",
					MachPhys::WeaponCombo weaponCom = MachPhys::L_BOLTER,
					MachPhys::Race rc = MachPhys::RED,
					bool inSqd = false,
					uint sqd = 0 )
		:	type( machType ),
			subType( subType ),
			bodyLevel( bLevel),
			brainboxLevel( bbLevel ),
			description( desc ),
			weaponCombo( weaponCom ),
			race( rc ),
			inSquad( inSqd ),
			squad( sqd )
		{}
		MachPhys::MachineType type;
		int subType;
		size_t bodyLevel;
		size_t brainboxLevel;
		string description;
		MachPhys::WeaponCombo weaponCombo;
		MachPhys::Race race;
		MexPoint3d position;   // replace with transform
		bool inSquad;
		uint squad;
	};

	struct Artefact
	{
		Artefact()
		{}
		string key;
		MexPoint3d position;	 // replace with transform
		MexDegrees orientation;
	};

	typedef ctl_vector< RaceAI > RaceAIs;
	typedef ctl_vector< MineralSite > MineralSites;
	typedef ctl_vector< AssemblyPoint > AssemblyPoints;
	typedef ctl_vector< Miscellanious > Miscellani;
	typedef ctl_vector< Construction > Constructions;
	typedef ctl_vector< Machine > Machines;
	typedef ctl_vector< Artefact > Artefacts;
	typedef ctl_vector< Condition > Conditions;
	typedef ctl_vector< Action > Actions;

	// File manipulation
	void read( const SysPathName& fileName );
	// PRE fileName.existsAsFile()
	void write( std::ofstream& );

	Constructions constructions() const;
	void constructions( const Constructions& );

	Machines machines() const;
	void machines( const Machines& );

	const SysPathName& arfFilename() const { return artefactModelFile_; }
	void arfFilename( SysPathName fileName ) { artefactModelFile_ = fileName; }
	Artefacts artefacts() const;
	void artefacts( const Artefacts& );

	Camera& camera( MachPhys::Race );

private:
   	struct RaceInfo
	{
		RaceInfo( MachPhys::Race rc = MachPhys::NORACE )
		:
		race( rc )
		{}
		MachPhys::Race race;
		ctl_vector< string > raceParameters;
		Camera camera;
		Constructions constructions;
		Machines machines;
		AssemblyPoints assemblyPoints;
		ctl_vector< string > miscellani;
	};
	typedef ctl_vector< RaceInfo > RacesInfo;

	// File read methods
	void readAI();
	void readMineralSite();
	void readConditions();
	void readActions();
	void readRace( RaceInfo&, MachPhys::Race race );
	void readMiscellani();
	void readArtefacts();
    Camera parseCamera( const UtlLineTokeniser::Tokens& );
	Construction parseConstruction( const UtlLineTokeniser::Tokens& );
	Machine parseMachine( const UtlLineTokeniser::Tokens& );
	AssemblyPoint parseAssemblyPoint( const UtlLineTokeniser::Tokens& );

	// file write methods
	void writeAI( std::ofstream& );
	void writeMineralSite( std::ofstream& );
	void writeConditions( std::ofstream& );
	void writeActions( std::ofstream& );
	void writeRace( std::ofstream&, RaceInfo& );
	void writeMiscellani( std::ofstream& );
	void writeArtefacts( std::ofstream& ) const;
	void writeCamera( std::ofstream&, const Camera& ) const;
	void writeConstruction( std::ofstream&, const Construction& ) const;
	void writeMachine( std::ofstream&, const Machine& ) const;
	void writeAssemblyPoint( std::ofstream&, const AssemblyPoint& ) const;

	MachPhys::ConstructionType constructionType( const string& ) const;
	MachPhys::WeaponCombo weaponCombo( const string& ) const;
	int constructionSubType( const string& ) const;
	MachPhys::MachineType machineType( const string& ) const;
	int	machineSubType( const string& ) const;
	uint nRaces();
	void errorBox( const string& errorMessage ) const;


	// Data
	SysPathName fileName_;
	UtlLineTokeniser* pParser_;

	RaceAIs	raceAIs_;
	MineralSites mineralSites_;
	Actions actions_;
	Conditions conditions_;
	ctl_vector< RaceInfo > racesInfo_;
 	Miscellani miscellani_;
	Artefacts artefacts_;
	SysPathName artefactModelFile_;

	// constants
	static const string AIToken_;
	static const string endAIToken_;

	static const string conditionsToken_;
	static const string endConditionsToken_;

	static const string actionsToken_;
	static const string endActionsToken_;

	static const string artefactToken_;
	static const string endArtefactToken_;
	static const string artefactModelsToken_;
	static const string artefactDumbToken_;
	static const string artefactActiveToken_;
	static const string artefactAtToken_;

	static const string cameraToken_;
	static const string cameraGroundToken_;
	static const string cameraZenithToken_;

	static const string mineralToken_;
	static const string siteToken_;
	static const string endMineralSiteToken_;
	static const string raceToken_;
	static const string endRaceToken_;
	static const string constructionToken_;
	static const string machineToken_;
	static const string	assemblyPointToken_;

	static const string	squadToken_;

    friend ostream& operator <<( ostream& o, const PedScenarioFile& t );

    PedScenarioFile( const PedScenarioFile& );
    PedScenarioFile& operator =( const PedScenarioFile& );

};


#endif

/* End SCENFILE.HPP *************************************************/
