/*
 * P E R M A C H . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "mathex/point2d.hpp"
#include "machlog/permach.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/apc.hpp"
#include "machlog/construc.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/spy.hpp"
#include "machlog/technici.hpp"
#include "machlog/races.hpp"

PER_DEFINE_PERSISTENT( MachLogMachinePersistence );

// static
MachLogMachinePersistence& MachLogMachinePersistence::instance()
{
    static MachLogMachinePersistence instance_;
    return instance_;
}

MachLogMachinePersistence::MachLogMachinePersistence()
{
	registerDerivedClasses();
    TEST_INVARIANT;
}

MachLogMachinePersistence::~MachLogMachinePersistence()
{
    TEST_INVARIANT;

}

void MachLogMachinePersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMachinePersistence& t )
{

    o << "MachLogMachinePersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogMachinePersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogMachinePersistence::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogAdministrator );
	PER_REGISTER_DERIVED_CLASS( MachLogAggressor );
	PER_REGISTER_DERIVED_CLASS( MachLogAPC );
	PER_REGISTER_DERIVED_CLASS( MachLogConstructor );
	PER_REGISTER_DERIVED_CLASS( MachLogGeoLocator );
	PER_REGISTER_DERIVED_CLASS( MachLogResourceCarrier );
	PER_REGISTER_DERIVED_CLASS( MachLogSpyLocator );
	PER_REGISTER_DERIVED_CLASS( MachLogTechnician );
}

void MachLogMachinePersistence::setUpLocalVectorFromViews() const
{
	//clear down collection of machines before we begin.
	MachLogMachinePersistence& nonConstPer = *( _CONST_CAST( MachLogMachinePersistence*, this ) );
	nonConstPer.machines_.erase( nonConstPer.machines_.begin(), nonConstPer.machines_.end() );
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin(); i != MachLogRaces::instance().objects().end(); ++i )
		if( (*i)->objectIsMachine() )
			nonConstPer.machines_.push_back( &(*i)->asMachine() );
}

void MachLogMachinePersistence::setUpViewsFromLocalVector()
{
	//put all the machines back into the collections.
	//this should update everything accordingly.
	for( Machines::iterator i = machines_.begin(); i != machines_.end(); ++i )
		MachLogRaces::instance().addToAllCollections( (*i) );
}

void perWrite( PerOstream& ostr, const MachLogMachinePersistence& per )
{
	per.setUpLocalVectorFromViews();
	ostr << per.machines_;
}

void perRead( PerIstream& istr, MachLogMachinePersistence& per )
{
	istr >> per.machines_;
	per.setUpViewsFromLocalVector();
}

/* End PERMACH.CPP **************************************************/
