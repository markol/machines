/*
 * S Q U A D I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/squadi.hpp"
#include "machlog/administ.hpp"
#include "machlog/cntrl_ai.hpp"

PER_DEFINE_PERSISTENT( MachLogSquadronImpl )

MachLogSquadronImpl::MachLogSquadronImpl( int squadronId )
:	hasCommander_( false ),
	squadronId_( squadronId ),
	totalDesiredMachines_( 0 ),
	setDefCon_( false ),
	defCon_( MachLog::DEFCON_LOW )
{
	machines_.reserve( 20 );
    TEST_INVARIANT;
}

MachLogSquadronImpl::~MachLogSquadronImpl()
{
    TEST_INVARIANT;

}

void MachLogSquadronImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogSquadronImpl& t )
{

    o << "MachLogSquadronImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogSquadronImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogSquadronImpl& actorImpl )
{
	ostr << actorImpl.hasCommander_;
	if( actorImpl.hasCommander_ )
		ostr << actorImpl.pCommander_;
	ostr << actorImpl.machines_;
	ostr << actorImpl.squadronId_;
	ostr << actorImpl.desiredMachineList_;
	ostr << actorImpl.totalDesiredMachines_;
	ostr << actorImpl.setDefCon_;
	ostr << actorImpl.defCon_;
}

void perRead( PerIstream& istr, MachLogSquadronImpl& actorImpl )
{
	istr >> actorImpl.hasCommander_;
	if( actorImpl.hasCommander_ )
		istr >> actorImpl.pCommander_;
	else
		actorImpl.pCommander_ = NULL;
	istr >> actorImpl.machines_;
	istr >> actorImpl.squadronId_;
	istr >> actorImpl.desiredMachineList_;
	istr >> actorImpl.totalDesiredMachines_;
	istr >> actorImpl.setDefCon_;
	istr >> actorImpl.defCon_;
}

MachLogSquadronImpl::MachLogSquadronImpl( PerConstructor )
{
}

/* End SQUADI.CPP ***************************************************/
