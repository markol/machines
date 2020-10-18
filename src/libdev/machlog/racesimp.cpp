/*
 * R A C E S I M P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machlog/internal/racesimp.hpp"
#include "machlog/score.hpp"

PER_DEFINE_PERSISTENT( MachLogRacesDataImpl );

MachLogRacesDataImpl::MachLogRacesDataImpl()
:  isUnloadingGame_( false )
{

    TEST_INVARIANT;
}

MachLogRacesDataImpl::~MachLogRacesDataImpl()
{
    TEST_INVARIANT;

}

void MachLogRacesDataImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogRacesDataImpl& t )
{

    o << "MachLogRacesDataImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogRacesDataImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogRacesDataImpl& impl )
{
//	ostr << impl.scores_;
	PER_WRITE_RAW_DATA( ostr, impl.virtualBeacon_, sizeof( MachLog::BeaconType ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.beaconType_, sizeof( MachLog::BeaconType ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.disposition_, sizeof( MachLogRaces::DispositionToRace ) * MachPhys::N_RACES * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.defaultDefCon_, sizeof( MachLog::DefCon ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.nBuildingMaterialUnits_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.nOre_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.nMaxBuildingMaterialUnits_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.nMaxOre_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.techniciansTreacheried_, sizeof( int ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.vortexHasBeenFired_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.firedAt_, sizeof( bool ) * MachPhys::N_RACES * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.hasWon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.hasLost_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.podCaptured_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.builtIonCannon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.builtNuclearWeapon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_WRITE_RAW_DATA( ostr, impl.raceInGame_, sizeof( bool ) * MachPhys::N_RACES );

    //The following line causes a crash. Don't know why, so just writing as an int
	//PER_WRITE_RAW_DATA( ostr, impl.gameType_, sizeof( MachLog::GameType ) );
    int gameType = impl.gameType_;
	PER_WRITE_RAW_OBJECT( ostr, gameType );


	for( MachPhys::Race i = MachPhys::RED ; i < MachPhys::N_RACES; ++((int&)i) )
	{
		const ctl_vector< MexPoint2d > tempPoints( impl.aggressorAssemblyPoints_[ i ] );
		PER_WRITE_RAW_OBJECT( ostr, tempPoints );
		const ctl_vector< MexPoint2d > tempPoints2( impl.administratorAssemblyPoints_[ i ] );
		PER_WRITE_RAW_OBJECT( ostr, tempPoints2 );
	}

	HAL_STREAM("writing out score objects...\n" );
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		HAL_STREAM(" for race " << i << " pointer is " << (void*)impl.scores_[i] << std::endl );
		MachLogScore& score = *( impl.scores_[i] );
		ostr << score;
//		PER_WRITE_RAW_OBJECT( ostr, score );
	}
}

void perRead( PerIstream& istr, MachLogRacesDataImpl& impl )
{
//	istr >> impl.scores_;
	PER_READ_RAW_DATA( istr, impl.virtualBeacon_, sizeof( MachLog::BeaconType ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.beaconType_, sizeof( MachLog::BeaconType ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.disposition_, sizeof( MachLogRaces::DispositionToRace ) * MachPhys::N_RACES * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.defaultDefCon_, sizeof( MachLog::DefCon ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.nBuildingMaterialUnits_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.nOre_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.nMaxBuildingMaterialUnits_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.nMaxOre_, sizeof( MachPhys::BuildingMaterialUnits ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.techniciansTreacheried_, sizeof( int ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.vortexHasBeenFired_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.firedAt_, sizeof( bool ) * MachPhys::N_RACES * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.hasWon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.hasLost_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.podCaptured_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.builtIonCannon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.builtNuclearWeapon_, sizeof( bool ) * MachPhys::N_RACES );
	PER_READ_RAW_DATA( istr, impl.raceInGame_, sizeof( bool ) * MachPhys::N_RACES );

	//PER_READ_RAW_DATA( istr, impl.gameType_, sizeof( MachLog::GameType ) );

    int gameType;
	PER_READ_RAW_OBJECT( istr, gameType );
    impl.gameType_ = (MachLog::GameType)gameType;

	for( MachPhys::Race i = MachPhys::RED ; i < MachPhys::N_RACES; ++((int&)i) )
	{
		MachLogRaces::AssemblyPoints tempPoints;
		PER_READ_RAW_OBJECT( istr, tempPoints );
		impl.aggressorAssemblyPoints_[ i ] = tempPoints;
		MachLogRaces::AssemblyPoints tempPoints2;
		PER_READ_RAW_OBJECT( istr, tempPoints2 );
		impl.administratorAssemblyPoints_[ i ] = tempPoints2;
	}

	HAL_STREAM("reading in score objects...\n" );
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		HAL_STREAM(" race " << i << " pointer is " << (void*)impl.scores_[i] << std::endl );
		MachLogScore& score = *( impl.scores_[i] );
		istr >> score;
	}
}

/* End RACESIMP.CPP *************************************************/
