/*
 * S C E N F I L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/scenfile.hpp"
#include "utility/linetok.hpp"
#include "planeted/race.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/radians.hpp"
#include "mathex/degrees.hpp"
#include "system/winapi.hpp"

const string PedScenarioFile::artefactToken_ 		= "ARTEFACTS";
const string PedScenarioFile::endArtefactToken_ 	= "ENDARTEFACTS";
const string PedScenarioFile::artefactModelsToken_ 	= "MODELS";
const string PedScenarioFile::artefactDumbToken_ 	= "DUMB";
const string PedScenarioFile::artefactActiveToken_ 	= "ACTIVE";
const string PedScenarioFile::artefactAtToken_ 		= "AT";
const string PedScenarioFile::cameraToken_ 			= "CAMERA_POS";
const string PedScenarioFile::cameraGroundToken_ 	= "GROUND";
const string PedScenarioFile::cameraZenithToken_	= "ZENITH";
const string PedScenarioFile::AIToken_ 				= "RACES_DEFAULT";
const string PedScenarioFile::endAIToken_ 			= "ENDRACES_DEFAULT";
const string PedScenarioFile::conditionsToken_ 		= "CONDITIONS";
const string PedScenarioFile::endConditionsToken_ 	= "ENDCONDITIONS";
const string PedScenarioFile::actionsToken_ 		= "ACTIONS";
const string PedScenarioFile::endActionsToken_ 		= "ENDACTIONS";
const string PedScenarioFile::mineralToken_ 		= "MINERAL";
const string PedScenarioFile::siteToken_ 			= "SITES";
const string PedScenarioFile::endMineralSiteToken_ 	= "ENDSITES";
const string PedScenarioFile::raceToken_ 			= "RACE";
const string PedScenarioFile::endRaceToken_ 		= "ENDRACE";
const string PedScenarioFile::constructionToken_ 	= "CONSTRUCTIONS";
const string PedScenarioFile::machineToken_ 		= "MACHINES";
const string PedScenarioFile::assemblyPointToken_ 	= "ASSEMBLY";
const string PedScenarioFile::squadToken_ 			= "SQUAD";

PedScenarioFile::PedScenarioFile()
:	pParser_( NULL ),
	artefactModelFile_( "" )
{
	// Initialise race information structures
	RaceInfo red( MachPhys::RED );
	RaceInfo blue( MachPhys::BLUE );
	RaceInfo green( MachPhys::GREEN );
	RaceInfo yellow( MachPhys::YELLOW );

	racesInfo_.push_back( red );
	racesInfo_.push_back( blue );
	racesInfo_.push_back( green );
	racesInfo_.push_back( yellow );

    TEST_INVARIANT;
}

PedScenarioFile::~PedScenarioFile()
{
    TEST_INVARIANT;

}

void PedScenarioFile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedScenarioFile& t )
{

    o << "PedScenarioFile " << (void*)&t << " start" << std::endl;
    o << "PedScenarioFile " << (void*)&t << " end" << std::endl;

    return o;
}

void PedScenarioFile::read( const SysPathName& fileName )
{
 	PRE( fileName.existsAsFile() );

	fileName_ = fileName;

	readAI();
	readMineralSite();
	readConditions();
	readActions();
	// Does not just assume there is four races as there doesn't necessarily have to be
	for( ctl_vector< RaceInfo >::iterator i = racesInfo_.begin(); i != racesInfo_.end(); ++i )
	{
		readRace( *i, (*i).race );
	}
	readMiscellani();
	readArtefacts();
}

void PedScenarioFile::write( std::ofstream& out )
{
	writeAI( out );
	writeMineralSite( out );
	writeConditions( out );
	writeActions( out );
	writeArtefacts( out );
	for( ctl_vector< RaceInfo >::iterator i = racesInfo_.begin(); i != racesInfo_.end(); ++i )
	{
		writeRace( out, *i );
	}
	writeMiscellani( out );

}

PedScenarioFile::Constructions PedScenarioFile::constructions( void ) const
{
	// Construct a list of all constructions from raceInfo lists
	Constructions constructions;

	for(  RacesInfo::const_iterator raceIter = racesInfo_.begin(); raceIter != racesInfo_.end(); ++raceIter )
	{
		for( Constructions::const_iterator i = (*raceIter).constructions.begin();
			 i != (*raceIter).constructions.end(); ++i )
		{
			constructions.push_back( *i );
		}
 	}
	return  constructions;
}

void  PedScenarioFile::constructions( const Constructions& constructions )
{
	for(  RacesInfo::iterator raceIter = racesInfo_.begin(); raceIter != racesInfo_.end(); ++raceIter )
	{
		// Empty construction list for race
		PedScenarioFile::Constructions& raceConstructions = (*raceIter).constructions;
		raceConstructions.erase( raceConstructions.begin(), raceConstructions.end() );

		for( Constructions::const_iterator i = constructions.begin(); i != constructions.end(); ++i )
		{
			if( (*i).race == (*raceIter).race )
			{
				raceConstructions.push_back( *i );
			}
		}
 	}
}

PedScenarioFile::Machines PedScenarioFile::machines( void ) const
{
	// Construct a list of all machines from raceInfo lists
	Machines machines;

	for(  RacesInfo::const_iterator raceIter = racesInfo_.begin(); raceIter != racesInfo_.end(); ++raceIter )
	{
		for( Machines::const_iterator i = (*raceIter).machines.begin();
			 i != (*raceIter).machines.end(); ++i )
		{
			machines.push_back( *i );
		}
 	}
	return  machines;
}

void  PedScenarioFile::machines( const Machines& machines )
{
	for(  RacesInfo::iterator raceIter = racesInfo_.begin(); raceIter != racesInfo_.end(); ++raceIter )
	{
		// Empty machine list for race
		PedScenarioFile::Machines& raceMachines = (*raceIter).machines;
		raceMachines.erase( raceMachines.begin(), raceMachines.end() );

		for( Machines::const_iterator i = machines.begin(); i != machines.end(); ++i )
		{
			if( (*i).race == (*raceIter).race )
			{
				raceMachines.push_back( *i );
			}
		}
 	}
}

PedScenarioFile::Artefacts PedScenarioFile::artefacts( void ) const
{
	return artefacts_;
}

void PedScenarioFile::artefacts( const Artefacts& artefacts )
{
	artefacts_.erase( artefacts_.begin(), artefacts_.end() ); // May not be necessary...
	artefacts_ = artefacts;
}

PedScenarioFile::Camera& PedScenarioFile::camera( MachPhys::Race race )
{
	ctl_vector< RaceInfo >::iterator raceIter;
	// Return handle to camera for specified race
	for(  raceIter = racesInfo_.begin();  (*raceIter).race != race; ++raceIter )
	{
 		// Intentionally empty
 	}
	return  (*raceIter).camera;
}

void PedScenarioFile::readAI( void )
{
 	bool parsingAI = false;
	bool done = false;

	// Reset parser
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

    while( not pParser_->finished() and not done )
	{
		if ( pParser_->tokens()[0] == AIToken_ )
		{
			parsingAI = true;
		}
		else if ( parsingAI and ( pParser_->tokens()[0] == endAIToken_ ) )
		{
			parsingAI = false;
			done = true;
		}
		else if ( parsingAI )
		{
			raceAIs_.push_back( pParser_->line() );
		}
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readMineralSite( void )
{
 	bool parsingMineralsSites = false;
	bool done = false;

	// Reset parser
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

    while( not pParser_->finished() and not done )
	{
		if ( ( pParser_->tokens()[0] == mineralToken_ ) and  ( pParser_->tokens()[1] == siteToken_ ) )
		{
			parsingMineralsSites = true;
		}
		else if ( parsingMineralsSites and ( pParser_->tokens()[0] == endMineralSiteToken_ ) )
		{
			parsingMineralsSites = false;
			done = true;
		}
		else if ( parsingMineralsSites )
		{
			mineralSites_.push_back( pParser_->line() );
		}
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readConditions( void )
{
 	bool parsing = false;
	bool done = false;

	// Reset parser
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

    while( not pParser_->finished() and not done )
	{
		if ( pParser_->tokens()[0] == conditionsToken_ )
		{
			parsing  = true;
		}
		else if ( parsing and ( pParser_->tokens()[0] == endConditionsToken_ ) )
		{
			parsing = false;
			done = true;
		}
		else if ( parsing )
		{
			conditions_.push_back( pParser_->line() );
		}
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readActions( void )
{
 	bool parsing = false;
	bool done = false;

	// Reset parser
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

    while( not pParser_->finished() and not done )
	{
		if ( pParser_->tokens()[0] == actionsToken_ )
		{
			parsing = true;
		}
		else if ( parsing and ( pParser_->tokens()[0] == endActionsToken_ ) )
		{
			parsing = false;
			done = true;
		}
		else if ( parsing )
		{
			actions_.push_back( pParser_->line() );
		}
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readRace( RaceInfo& raceInfo, MachPhys::Race race )
{
	bool parsingConstructions = false;
	bool parsingMiscellani = false;
	bool parsingMachines = false;
	bool parsingRace = false;
	bool done = false;
	raceInfo.race = race;

	// Reset parser
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

    while( not pParser_->finished() and not done )
	{
		if ( ( pParser_->tokens()[0] == raceToken_ ) and ( race == PedRace::parse( pParser_->tokens()[1] ) ) )
		{
			parsingRace = true;
			for ( uint parameter = 2; parameter < pParser_->tokens().size(); parameter++ )
			{
				raceInfo.raceParameters.push_back( pParser_->tokens()[ parameter ] );
			}
		}
		else if ( pParser_->tokens()[0] == endRaceToken_ and parsingRace )
		{
			done = true;
		}
		else if ( pParser_->tokens()[0] == cameraToken_  and parsingRace )
		{
			raceInfo.camera = parseCamera( pParser_->tokens() );
		}
		else if ( pParser_->tokens()[0] == constructionToken_  and parsingRace )
		{
 			parsingMachines = parsingMiscellani = false;
			parsingConstructions = true;
		}
		else if ( pParser_->tokens()[0] == machineToken_  and parsingRace )
		{
			parsingConstructions = parsingMiscellani = false;
			parsingMachines = true;
		}
		else if ( pParser_->tokens()[0] == assemblyPointToken_ and parsingRace )
		{
			//parsingMachines = parsingConstructions = false;
			parsingConstructions = false;
			AssemblyPoint assemblyPoint = parseAssemblyPoint( pParser_->tokens() );
			raceInfo.assemblyPoints.push_back( assemblyPoint );
		}
		else if ( pParser_->tokens()[0] == endRaceToken_ and parsingRace )
		{
			parsingMachines = parsingConstructions = parsingMiscellani = false;
			done = true;
		}
		else if ( parsingRace and parsingMachines and ( pParser_->tokens().size() < 6  or pParser_->tokens().size() > 11 ) )
		{
			parsingMachines = parsingConstructions = false;
			parsingMiscellani = true;
			// This line must be included in races list of miscellani
			raceInfo.miscellani.push_back( pParser_->line() );
		}
		else if ( parsingConstructions )
		{
			Construction construction = parseConstruction( pParser_->tokens() );
			construction.race = race;
			raceInfo.constructions.push_back( construction );
		}
		else if ( parsingMachines )
		{
			Machine machine = parseMachine( pParser_->tokens() );
			machine.race = race;
			raceInfo.machines.push_back( machine );
		}
		else if ( parsingMiscellani )
		{
			raceInfo.miscellani.push_back( pParser_->line() );
		}
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readMiscellani( void )
{
	uint raceNumber = nRaces();
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

	// Skip to end of races
    for( uint i = 0; i < raceNumber; ++i )
	{
		bool parsedEndRace = false;
		while( not parsedEndRace )
		{
			if ( pParser_->tokens()[0] == endRaceToken_ )
			{
				parsedEndRace = true;
			}
			pParser_->parseNextLine();
		}
	}
	// Read everything past last race until eof
    while( not pParser_->finished() )
	{
		miscellani_.push_back( pParser_->line() );
		pParser_->parseNextLine();
	}
}

void PedScenarioFile::readArtefacts( void )
{
	bool parsingKey = false;
	bool parsingArtefacts = false;
	string key;

	artefacts_.erase( artefacts_.begin(), artefacts_.end() );
	_DELETE( pParser_ );
	pParser_ = _NEW( UtlLineTokeniser( fileName_ ) );

	while (	not pParser_->finished() )
	{
		if ( ( pParser_->tokens().size() >= 5 ) and parsingArtefacts
				and  parsingKey and pParser_->tokens()[0] == "AT" )
		{
			// Read artefact data
			Artefact artefact;
			artefact.position.x( atof( pParser_->tokens()[1].c_str() ) );
			artefact.position.y( atof( pParser_->tokens()[2].c_str() ) );
			artefact.position.z( atof( pParser_->tokens()[3].c_str() ) );
			artefact.orientation = atof( pParser_->tokens()[4].c_str() );
 			artefact.key = key;
 			artefacts_.push_back( artefact );
 		}
		else if ( pParser_->tokens()[0] == artefactToken_ )
		{
			parsingArtefacts = true;
		}
		else if ( pParser_->tokens()[0] == endArtefactToken_ )
		{
 			parsingArtefacts = false;
		}
		else if ( ( pParser_->tokens()[0] == artefactDumbToken_ or pParser_->tokens()[0]  == artefactActiveToken_ )
					and parsingArtefacts )
		{
			key = pParser_->tokens()[1];
			parsingKey = true;
		}
		else if ( pParser_->tokens()[0] == artefactModelsToken_ )
		{
			//artefactModelFile_ = ( pParser_->tokens()[1] );
			artefactModelFile_ = string( pParser_->tokens()[1] );
		}
		else parsingKey = false;
		pParser_->parseNextLine();
	}
}

PedScenarioFile::Camera PedScenarioFile::parseCamera( const UtlLineTokeniser::Tokens& tokens )
{
	ASSERT_INFO( pParser_->lineNumber() );
	ASSERT( tokens.size() == 8, "incorrect no. of arguments" );

	Camera camera;

	// parse camera position and rotation
	MexPoint3d position;
	MexEulerAngles rotation;
	position.x( atof( tokens[1].c_str() ) );
	position.y( atof( tokens[2].c_str() ) );
	position.z( atof( tokens[3].c_str() ) );
	rotation.azimuth( MexRadians( MexDegrees( atof( tokens[4].c_str() ) ) ) );
	rotation.elevation( MexRadians( MexDegrees( atof( tokens[5].c_str() ) ) ) );
	rotation.roll( MexRadians( MexDegrees( atof( tokens[6].c_str() ) ) ) );
	camera.transform = MexTransform3d( rotation, position );

	// parse camera type
	if ( tokens[7] == cameraGroundToken_ )
	{
		camera.type = MachLogCamera::GROUND;
	}
	else if ( tokens[7] == cameraZenithToken_ )
	{
		camera.type = MachLogCamera::ZENITH;
	}
	else
	{
		ASSERT_INFO( pParser_->lineNumber() );
		ASSERT_INFO( tokens[7] );
		ASSERT( false, "invalid camera type" );
	}
	return camera;
}

PedScenarioFile::Construction PedScenarioFile::parseConstruction( const UtlLineTokeniser::Tokens& tokens )
{
	ASSERT_INFO( pParser_->lineNumber() );
	ASSERT( tokens.size() >= 6, "not enough arguments" );

	if( not ( tokens.size() >= 6 ) )
	{
		string error = "Invalid construction ";
		error += tokens[0].c_str();
		error += " - too few arguments" ;
		errorBox( error );
	}
	// Do reading of construction data
 	Construction construction;
	uint buildParametersIndex = 6;

	construction.type = constructionType( tokens[0].c_str() );
	construction.hardwareLevel = atol( tokens[1].c_str() );

	if ( ( construction.type == MachPhys::POD ) )
	{
		construction.weaponCombo = weaponCombo( tokens[6].c_str() );
		buildParametersIndex = 7;
	}
	if ( ( construction.type == MachPhys::FACTORY )
		or ( construction.type == MachPhys::HARDWARE_LAB )
		or ( construction.type == MachPhys::MISSILE_EMPLACEMENT ) )
	{
		construction.subType = constructionSubType( tokens[6].c_str() );
		buildParametersIndex = 7;
	}
	if ( ( construction.type == MachPhys::MISSILE_EMPLACEMENT ) )
	{
		construction.weaponCombo = weaponCombo( tokens[7].c_str() );
		buildParametersIndex = 8;
	}
	construction.position.x( atof( tokens[2].c_str() ) );
	construction.position.y( atof( tokens[3].c_str() ) );
	construction.position.z( atof( tokens[4].c_str() ) );
	construction.orientation = atof( tokens[5].c_str() );

	for( uint i = buildParametersIndex; i < tokens.size(); ++i )
	{
		construction.build.push_back( tokens[ i ].c_str() );
	}
	WAYNE_STREAM( "construction: " << construction.type << std::endl <<
				  "sub type: " << construction.subType << std::endl	 <<
				  "hw level: " << construction.hardwareLevel << std::endl <<
				  "weapon combo: " << construction.weaponCombo << std::endl <<
				  "position: " << construction.position << std::endl <<
				  "orientation: " <<construction.orientation << std::endl <<
				  "build: " << construction.build << std::endl <<
				  "race: " << construction.race << std::endl );

	return construction;
}

PedScenarioFile::Machine PedScenarioFile::parseMachine( const UtlLineTokeniser::Tokens& tokens )
{

	ASSERT_INFO( pParser_->lineNumber() );
	ASSERT( tokens.size() >= 6, "too few arguments" );
	ASSERT( tokens.size() <= 11, "too many arguments" );

	if( not ( tokens.size() >= 6 ) )
	{
		string error = "Invalid machine ";
		error += tokens[0].c_str();
		error += " - too few arguments" ;
		errorBox( error );
	}
	else if( not ( tokens.size() <= 11 ) )
	{
		string error = "Invalid machine ";
		error += tokens[0].c_str();
		error += " - too many arguments";
		errorBox( error );
	}
	// Do reading of machine data
	Machine machine;
	uint squadIndex = 6;
	machine.type = machineType( tokens[0].c_str() );
	// take account of sub type being omitted
	if( tokens.size() > 6 )
	{
		machine.subType = machineSubType( tokens[6].c_str() );
	}
	else
	{
		machine.subType = 0;
	}
	machine.bodyLevel = atol( tokens[1].c_str() );
	machine.brainboxLevel = atol( tokens[2].c_str() );

	if ( ( machine.type == MachPhys::AGGRESSOR )
		or ( machine.type == MachPhys::ADMINISTRATOR ) )
	{
		machine.weaponCombo = weaponCombo( tokens[7].c_str() );
		squadIndex = 8;
	}

	machine.position.x( atof( tokens[3].c_str() ) );
	machine.position.y( atof( tokens[4].c_str() ) );
	machine.position.z( atof( tokens[5].c_str() ) );

	for( uint i = squadIndex; i < tokens.size(); ++i )
	{
		if ( tokens[i].c_str() == squadToken_ )
		{
			machine.inSquad = true;
			machine.squad = atol( tokens[i+1].c_str() );
		}
	}

	return machine;
}

PedScenarioFile::AssemblyPoint PedScenarioFile::parseAssemblyPoint( const UtlLineTokeniser::Tokens& tokens )
{
	AssemblyPoint assemblyPoint;

	for (uint i = 0; i < tokens.size(); i++ )
	{
		assemblyPoint += tokens[ i ] + " ";
	}

	return assemblyPoint;
}

void PedScenarioFile::writeAI( std::ofstream& out )
{
	if( raceAIs_.size() )
	{
		out << AIToken_ << std::endl;
		for ( RaceAIs::iterator i = raceAIs_.begin(); i != raceAIs_.end(); ++i )
		{
			out << *i;
		}
		out << endAIToken_ << std::endl << std::endl;
	}
}

void PedScenarioFile::writeMineralSite( std::ofstream& out )
{
	if( mineralSites_.size() )
	{
		out << mineralToken_ << " " << siteToken_ << std::endl;
		for ( MineralSites::iterator i = mineralSites_.begin(); i != mineralSites_.end(); ++ i )
		{
			out << *i;
		}
		out << endMineralSiteToken_ << std::endl << std::endl << std::endl;
	}
}

void PedScenarioFile::writeConditions( std::ofstream& out )
{
	if( conditions_.size() )
	{
		out << conditionsToken_ << std::endl;
		for ( Conditions::iterator i = conditions_.begin(); i != conditions_.end(); ++i )
		{
			out << *i;
		}
		out << endConditionsToken_ << std::endl << std::endl;
	}
}

void PedScenarioFile::writeActions( std::ofstream& out )
{
	if( actions_.size() )
	{
		out << actionsToken_ << std::endl;
		for ( Actions::iterator i = actions_.begin(); i != actions_.end(); ++i )
		{
			out << *i;
		}
		out << endActionsToken_ << std::endl << std::endl;
	}
}

void PedScenarioFile::writeRace( std::ofstream& out, RaceInfo& raceInfo )
{
	out << raceToken_ << " " << raceInfo.race;
	//if ( raceInfo.raceParameters.size() == 0 )
	//{
	//	out << " none << std::endl;
	//}
	//else
	//{
	for( ctl_vector< string >::iterator i = raceInfo.raceParameters.begin(); i != raceInfo.raceParameters.end(); ++i )
	{
		out << " " << *i;
	}
	out << std::endl;
	//}
	writeCamera( out, raceInfo.camera );
	out << constructionToken_ << std::endl;

	for( Constructions::iterator i = raceInfo.constructions.begin(); i != raceInfo.constructions.end(); ++i )
	{
		writeConstruction( out, *i );
	}
	out << machineToken_ << std::endl;

	for( Machines::iterator i = raceInfo.machines.begin(); i != raceInfo.machines.end(); ++i )
	{
		writeMachine( out, *i );
	}

	for( AssemblyPoints::iterator i = raceInfo.assemblyPoints.begin(); i != raceInfo.assemblyPoints.end(); i++ )
	{
		writeAssemblyPoint( out, *i );
	}

	for( ctl_vector< string>::iterator i = raceInfo.miscellani.begin(); i != raceInfo.miscellani.end(); ++i )
	{
		out << *i;
	}
	out << endRaceToken_ << std::endl << std::endl;
}

void PedScenarioFile::writeMiscellani( std::ofstream& out )
{
	for( Miscellani::iterator i = miscellani_.begin(); i != miscellani_.end(); ++i )
	{
		out << *i;
	}
	out << std::endl;
}

void PedScenarioFile::writeArtefacts( std::ofstream& out ) const
{
	if( artefacts_.size() )
	{
	    out << artefactToken_ << std::endl;
		out << artefactModelsToken_ << " " << arfFilename() << std::endl;

		string key = "";
		for( Artefacts::const_iterator i = artefacts_.begin(); i != artefacts_.end(); ++i )
		{
			if ( not ( (*i).key == key ) )
			{
				key = (*i).key;
				out << artefactDumbToken_ << " " << key << std::endl;
			}
			out << artefactAtToken_ << " ";
			out << (*i).position.x() << " ";
			out << (*i).position.y() << " ";
			out << (*i).position.z() << " ";
			out << (*i).orientation << std::endl;
		}
		out << endArtefactToken_ << std::endl << std::endl;
	}
}

void PedScenarioFile::writeCamera( std::ofstream& out, const Camera& camera ) const
{
	MexPoint3d position = camera.transform.position();
	MexEulerAngles angles = camera.transform.rotationAsEulerAngles();

	out << cameraToken_ << " ";
	out << position.x() << " ";
	out << position.y() << " ";
	out << position.z() << " ";
	out << MexDegrees( angles.azimuth() ) << " ";
	out << MexDegrees( angles.elevation() ) << " ";
	out << MexDegrees( angles.roll() ) << " ";

	//ASSERT_INFO( camera.type );
	ASSERT( camera.type == MachLogCamera::ZENITH or camera.type == MachLogCamera::GROUND,
			"invalid camera type" );

	switch ( camera.type )
	{
		case( MachLogCamera::ZENITH ): 	out << cameraZenithToken_; break;
		case( MachLogCamera::GROUND ):	out << cameraGroundToken_; break;
	}
	out << std::endl;
}

void PedScenarioFile::writeConstruction( std::ofstream& out, const Construction& construction ) const
{
	out << construction.type << " ";
	out << construction.hardwareLevel << " ";
	out << construction.position.x() << " ";
	out << construction.position.y() << " ";
	out << construction.position.z() << " ";
	out << construction.orientation << " ";
	switch ( construction.type )
	{
		case MachPhys::FACTORY:
			out << ( MachPhys::FactorySubType ) construction.subType << " ";
			break;
		case MachPhys::HARDWARE_LAB:
			out << ( MachPhys::HardwareLabSubType ) construction.subType << " ";
			break;
		case MachPhys::MISSILE_EMPLACEMENT:
			out << ( MachPhys::MissileEmplacementSubType ) construction.subType << " ";
			break;
		default:
			break;
	}

	if ( ( construction.type == MachPhys::MISSILE_EMPLACEMENT )
		or ( construction.type == MachPhys::POD ) )
	{
		out << construction.weaponCombo;
	}
	for( ctl_vector< string >::const_iterator buildIter = construction.build.begin(); buildIter != construction.build.end();
		 ++buildIter )
	{
		out << " " << *buildIter;
	}
	out << std::endl;
}

void PedScenarioFile::writeMachine( std::ofstream& out, const Machine& machine ) const
{
	out << machine.type << " ";
	out << machine.bodyLevel << " ";
	out << machine.brainboxLevel << " ";
	out << machine.position.x() << " ";
	out << machine.position.y() << " ";
	out << machine.position.z() << " ";

	switch ( machine.type )
	{
		case MachPhys::AGGRESSOR:
			out << ( MachPhys::AggressorSubType ) machine.subType << " ";
			break;
		case MachPhys::TECHNICIAN:
			out << ( MachPhys::TechnicianSubType ) machine.subType << " ";
			break;
		case MachPhys::CONSTRUCTOR:
			out << ( MachPhys::ConstructorSubType ) machine.subType << " ";
			break;
		case MachPhys::ADMINISTRATOR:
			out << ( MachPhys::AdministratorSubType ) machine.subType << " ";
			break;
	}
	if 	( ( machine.type == MachPhys::AGGRESSOR )
		or ( machine.type == MachPhys::ADMINISTRATOR ) )
		out << machine.weaponCombo;

	if ( machine.inSquad )
	{
		out << " " << squadToken_ << " " << machine.squad;
	}
	out << std::endl;
}

void PedScenarioFile::writeAssemblyPoint( std::ofstream& out, const AssemblyPoint& assemblyPoint ) const
{
	// Provided for future design issues
	out << assemblyPoint << std::endl;
}

MachPhys::ConstructionType PedScenarioFile::constructionType( const string& type ) const
{
	//check for building types
	if( type == "BEACON" )
		return MachPhys::BEACON;
	if( type == "FACTORY" )
		return MachPhys::FACTORY;
	if( type == "GARRISON" )
		return MachPhys::GARRISON;
	if( type == "HARDWARE_LAB" )
		return MachPhys::HARDWARE_LAB;
	if( type == "HWLAB" )
		return MachPhys::HARDWARE_LAB;
	if( type == "MINE" )
		return MachPhys::MINE;
	if( type == "MISSILE_EMPLACEMENT" )
		return MachPhys::MISSILE_EMPLACEMENT;
	if( type == "MISSILE" )
		return MachPhys::MISSILE_EMPLACEMENT;
	if( type == "POD" )
		return MachPhys::POD;
	if( type == "SMELTER" )
		return MachPhys::SMELTER;

	return MachPhys::SMELTER;
}

MachPhys::WeaponCombo PedScenarioFile::weaponCombo( const string& weaponCombo ) const
{
	// determine weapon type

	if( weaponCombo == "L_BOLTER" )
        return MachPhys::L_BOLTER;
	if( weaponCombo == "T_ION_ORBITAL_CANNON" )
        return MachPhys::T_ION_ORBITAL_CANNON;
	if( weaponCombo == "L_ELECTRIC_CHARGE" )
        return MachPhys::L_ELECTRIC_CHARGE;
	if( weaponCombo == "R_BOLTER" )
        return MachPhys::R_BOLTER;
	if( weaponCombo == "R_ELECTRIC_CHARGE" )
        return MachPhys::R_ELECTRIC_CHARGE;
	if( weaponCombo == "LR_PULSE_RIFLE_X2" )
        return MachPhys::LR_PULSE_RIFLE_X2;
	if( weaponCombo == "L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED" )
        return MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED;
	if( weaponCombo == "LR_PLASMA_RIFLE_X2" )
        return MachPhys::LR_PLASMA_RIFLE_X2;
	if( weaponCombo == "L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER" )
        return MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER;
	if( weaponCombo == "L_MULTI_LAUNCHER5_R_TREACHERY" )
        return MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY;
	if( weaponCombo == "LR_PULSE_CANNON_X2" )
        return MachPhys::LR_PULSE_CANNON_X2;
	if( weaponCombo == "L_PULSE_CANNON_R_SUPERCHARGE_SUPER" )
        return MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER;
	if( weaponCombo == "L_PULSE_CANNON_R_VIRUS" )
        return MachPhys::L_PULSE_CANNON_R_VIRUS;
	if( weaponCombo == "L_FLAME_THROWER2" )
        return MachPhys::L_FLAME_THROWER2;
	if( weaponCombo == "LR_HEAVY_BOLTER1_X2" )
        return MachPhys::LR_HEAVY_BOLTER1_X2;
	if( weaponCombo == "L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7" )
        return MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7;
//	if( weaponCombo == "LR_GRENADE_LAUNCHER_X2" )
//        return MachPhys::LR_GRENADE_LAUNCHER_X2;
	if( weaponCombo == "L_AUTO_CANNON" )
        return MachPhys::L_AUTO_CANNON;
	if( weaponCombo == "L_PLASMA_RIFLE" )
        return MachPhys::L_PLASMA_RIFLE;
	if( weaponCombo == "LR_AUTO_CANNON_X2" )
        return MachPhys::LR_AUTO_CANNON_X2;
	if( weaponCombo == "T_NUCLEAR_MISSILE" )
        return MachPhys::T_NUCLEAR_MISSILE;
	if( weaponCombo == "T_MULTI_LAUNCHER1" )
        return MachPhys::T_MULTI_LAUNCHER1;
	if( weaponCombo == "T_VORTEX" )
        return MachPhys::T_VORTEX;
	if( weaponCombo == "LR_MULTI_LAUNCHER2_X2" )
        return MachPhys::LR_MULTI_LAUNCHER2_X2;
	if( weaponCombo == "LR_HEAVY_BOLTER2_X2" )
        return MachPhys::LR_HEAVY_BOLTER2_X2;
	if( weaponCombo == "LR_PLASMA_CANNON1_X2" )
        return MachPhys::LR_PLASMA_CANNON1_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER3_X2" )
        return MachPhys::LR_MULTI_LAUNCHER3_X2;
	if( weaponCombo == "LR_PLASMA_CANNON2_X2" )
        return MachPhys::LR_PLASMA_CANNON2_X2;
	if( weaponCombo == "LRT_MULTI_LAUNCHER4_X3" )
        return MachPhys::LRT_MULTI_LAUNCHER4_X3;
	if( weaponCombo == "LRT_PLASMA_CANNON2_X3" )
        return MachPhys::LRT_PLASMA_CANNON2_X3;
	if( weaponCombo == "T_FLAME_THROWER1" )
        return MachPhys::T_FLAME_THROWER1;
	if( weaponCombo == "LR_MULTI_LAUNCHER5_X2" )
        return MachPhys::LR_MULTI_LAUNCHER5_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER6_X2" )
        return MachPhys::LR_MULTI_LAUNCHER6_X2;
	if( weaponCombo == "LR_LARGE_MISSILE_X2" )
        return MachPhys::LR_LARGE_MISSILE_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER7_X2" )
        return MachPhys::LR_MULTI_LAUNCHER7_X2;
	if( weaponCombo == "T_WASP_LIGHT_STING" )
        return MachPhys::T_WASP_LIGHT_STING;
	if( weaponCombo == "T_WASP_METAL_STING" )
        return MachPhys::T_WASP_METAL_STING;
	if( weaponCombo == "T_BEE_BOMB" )
        return MachPhys::T_BEE_BOMB;
    ASSERT_INFO( weaponCombo );
	ASSERT( false," Unknown Weapon Combo\n" );
	return MachPhys::L_BOLTER;
}

int PedScenarioFile::constructionSubType( const string& type ) const
{
	if ( type == "CIVILIAN" )
		return MachPhys::CIVILIAN;
	if ( type == "MILITARY" )
		return MachPhys::MILITARY;
	if ( type == "LAB_CIVILIAN" )
		return MachPhys::LAB_CIVILIAN;
	if ( type == "LAB_MILITARY" )
		return MachPhys::LAB_MILITARY;
	if ( type == "TURRET" )
		return MachPhys::TURRET;
	if ( type == "SENTRY" )
		return MachPhys::SENTRY;
	if ( type == "LAUNCHER" )
		return MachPhys::LAUNCHER;
	if ( type == "ICBM" )
		return MachPhys::ICBM;
	return MachPhys::ICBM;
}

MachPhys::MachineType PedScenarioFile::machineType( const string& type ) const
{
	//Check for machine types
	if( type == "ADMINISTRATOR" )
		return MachPhys::ADMINISTRATOR;
	if( type == "AGGRESSOR" )
		return MachPhys::AGGRESSOR;
	if( type == "APC" )
		return MachPhys::APC;
	if( type == "CONSTRUCTOR" )
		return MachPhys::CONSTRUCTOR;
	if( type == "GEO_LOCATOR" )
		return MachPhys::GEO_LOCATOR;
	if( type == "RESOURCE_CARRIER" )
		return MachPhys::RESOURCE_CARRIER;
	if( type == "SPY_LOCATOR" )
		return MachPhys::SPY_LOCATOR;
	if( type == "TECHNICIAN" )
		return MachPhys::TECHNICIAN;
	return MachPhys::TECHNICIAN;
}

int	PedScenarioFile::machineSubType( const string& type ) const
{
	if( type == "GRUNT" )
		return MachPhys::GRUNT;
	if( type == "ASSASSIN" )
		return MachPhys::ASSASSIN;
	if( type == "KNIGHT" )
		return MachPhys::KNIGHT;
	if( type == "BALLISTA" )
		return MachPhys::BALLISTA;
	if( type == "NINJA" )
		return MachPhys::NINJA;
	if( type == "LAB_TECH" )
		return MachPhys::LAB_TECH;
	if( type == "TECH_BOY" )
		return MachPhys::TECH_BOY;
	if( type == "BRAIN_BOX" )
		return MachPhys::BRAIN_BOX;
	if( type == "DOZER" )
		return MachPhys::DOZER;
	if( type == "BUILDER" )
		return MachPhys::BUILDER;
	if( type == "BEHEMOTH" )
		return MachPhys::BEHEMOTH;
	if( type == "BOSS" )
		return MachPhys::BOSS;
	if( type == "OVERSEER" )
		return MachPhys::OVERSEER;
	if( type == "COMMANDER" )
		return MachPhys::COMMANDER;

	return MachPhys::GRUNT;

}

uint PedScenarioFile::nRaces( void )
{
	// Returns the number of races in scenario file
	uint nRaces = 0;

	UtlLineTokeniser* pParser = _NEW( UtlLineTokeniser( fileName_ ) );

	while( not pParser->finished() )
	{
		if( pParser->tokens()[0] == raceToken_ )
		{
			++nRaces;
		}
		pParser->parseNextLine();
	}
	return nRaces;
}

void PedScenarioFile::errorBox( const string& errorMessage ) const
{
	SysWindowsAPI::messageBox( errorMessage.c_str(),"PlanetEditor v0.01");
	exit( 0 );
}

/* End SCENFILE.CPP *************************************************/
