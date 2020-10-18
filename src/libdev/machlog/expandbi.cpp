/*
 * E X P A N D B I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machlog/internal/expandbi.hpp"

#include "sim/manager.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/actor.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"

MachLogExpandingBlastImpl::MachLogExpandingBlastImpl( MachActor* pOwner, 
													  MachLogRace* pLogRace, 
													  const MexPoint3d& startPosition, 
													  MATHEX_SCALAR blastRadius,
													  MachLogExpandingBlast::CanHitAirUnits canHitAirUnits )
:	pOwner_( pOwner ),
	startPosition_( startPosition ),
	pRace_( pLogRace ),
	finished_( false ),
	blastRadius_( blastRadius ),
	startedFirstWave_( false ),
	startedSecondWave_( false ),
	startedThirdWave_( false ),
	creationTime_( SimManager::instance().currentTime() ),
	nVictims_( 0 ),
	canHitAirUnits_( canHitAirUnits ), 
	pConstruction_( NULL )				// this represents the building the explosion is inside (if any)
{
	victims_.reserve( 50 );

    TEST_INVARIANT;
}

MachLogExpandingBlastImpl::~MachLogExpandingBlastImpl()
{
    TEST_INVARIANT;

}

void MachLogExpandingBlastImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogExpandingBlastImpl& t )
{

    o << "MachLogExpandingBlastImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogExpandingBlastImpl " << (void*)&t << " end" << std::endl;

    return o;
}


/* End EXPANDBI.CPP *************************************************/
