#include "machlog/internal/scorei.hpp"

PER_DEFINE_PERSISTENT( MachLogScoreImpl );

MachLogScoreImpl::MachLogScoreImpl()
{
	HAL_STREAM("MachLogScoreImpl::MachLogScoreImpl " << (void*)this << std::endl );
}
MachLogScoreImpl::~MachLogScoreImpl()
{
	HAL_STREAM("MachLogScoreImpl::~MachLogScoreImpl " << (void*)this << std::endl );
}


void perWrite( PerOstream& ostr, const MachLogScoreImpl& impl )
{
	HAL_STREAM("perWrite MachLogScoreImpl\n" );
	PER_WRITE_RAW_OBJECT( ostr, impl.grossScore_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.machinesBuilt_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.militaryMachinesBuilt_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.constructionsBuilt_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.myMachinesDestroyed_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.otherMachinesDestroyed_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.myConstructionsDestroyed_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.otherConstructionsDestroyed_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.itemsResearched_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.totalResearchCost_ );
	PER_WRITE_RAW_OBJECT( ostr, impl.BMUsMined_ );

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		PER_WRITE_RAW_OBJECT( ostr, impl.raceMachinesDestroyed_[ ridx ] );
		PER_WRITE_RAW_OBJECT( ostr, impl.raceConstructionsDestroyed_[ ridx ] );
		PER_WRITE_RAW_OBJECT( ostr, impl.raceMyMachinesDestroyed_[ ridx ] );
		PER_WRITE_RAW_OBJECT( ostr, impl.raceMyConstructionsDestroyed_[ ridx ] );
	}
}

void perRead( PerIstream& istr, MachLogScoreImpl& impl )
{
	HAL_STREAM("perRead MachLogScoreImpl\n" );
	PER_READ_RAW_OBJECT( istr, impl.grossScore_ );
	PER_READ_RAW_OBJECT( istr, impl.machinesBuilt_ );
	PER_READ_RAW_OBJECT( istr, impl.militaryMachinesBuilt_ );
	PER_READ_RAW_OBJECT( istr, impl.constructionsBuilt_ );
	PER_READ_RAW_OBJECT( istr, impl.myMachinesDestroyed_ );
	PER_READ_RAW_OBJECT( istr, impl.otherMachinesDestroyed_ );
	PER_READ_RAW_OBJECT( istr, impl.myConstructionsDestroyed_ );
	PER_READ_RAW_OBJECT( istr, impl.otherConstructionsDestroyed_ );
	PER_READ_RAW_OBJECT( istr, impl.itemsResearched_ );
	PER_READ_RAW_OBJECT( istr, impl.totalResearchCost_ );
	PER_READ_RAW_OBJECT( istr, impl.BMUsMined_ );

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		PER_READ_RAW_OBJECT( istr, impl.raceMachinesDestroyed_[ ridx ] );
		PER_READ_RAW_OBJECT( istr, impl.raceConstructionsDestroyed_[ ridx ] );
		PER_READ_RAW_OBJECT( istr, impl.raceMyMachinesDestroyed_[ ridx ] );
		PER_READ_RAW_OBJECT( istr, impl.raceMyConstructionsDestroyed_[ ridx ] );
	}
}

