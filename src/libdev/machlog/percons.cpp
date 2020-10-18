/*
 * P E R C O N S . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "mathex/point2d.hpp"
#include "machlog/percons.hpp"
#include "machlog/races.hpp"
#include "machlog/constron.hpp"
#include "machlog/beacon.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/mine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/pod.hpp"
#include "machlog/smelter.hpp"

PER_DEFINE_PERSISTENT( MachLogConstructionPersistence );

// static
MachLogConstructionPersistence& MachLogConstructionPersistence::instance()
{
    static MachLogConstructionPersistence instance_;
    return instance_;
}

MachLogConstructionPersistence::MachLogConstructionPersistence()
{
	registerDerivedClasses();
    TEST_INVARIANT;
}

MachLogConstructionPersistence::~MachLogConstructionPersistence()
{
    TEST_INVARIANT;

}

void MachLogConstructionPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogConstructionPersistence& t )
{

    o << "MachLogConstructionPersistence " << (void*)&t << " start" << std::endl;
    o << "MachLogConstructionPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogConstructionPersistence::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogBeacon );
	PER_REGISTER_DERIVED_CLASS( MachLogFactory );
	PER_REGISTER_DERIVED_CLASS( MachLogGarrison );
	PER_REGISTER_DERIVED_CLASS( MachLogHardwareLab );
	PER_REGISTER_DERIVED_CLASS( MachLogMine );
	PER_REGISTER_DERIVED_CLASS( MachLogMissileEmplacement );
	PER_REGISTER_DERIVED_CLASS( MachLogPod );
	PER_REGISTER_DERIVED_CLASS( MachLogSmelter );
}

void MachLogConstructionPersistence::setUpLocalVectorFromViews() const
{
	//clear down collection of constructions before we begin.
	MachLogConstructionPersistence& nonConstPer = *( _CONST_CAST( MachLogConstructionPersistence*, this ) );
	nonConstPer.constructions_.erase( nonConstPer.constructions_.begin(), nonConstPer.constructions_.end() );
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin(); i != MachLogRaces::instance().objects().end(); ++i )
		if( (*i)->objectIsConstruction() )
			nonConstPer.constructions_.push_back( &(*i)->asConstruction() );
}

void MachLogConstructionPersistence::setUpViewsFromLocalVector()
{
	//put all the machines back into the collections.
	//this should update everything accordingly.
	for( Constructions::iterator i = constructions_.begin(); i != constructions_.end(); ++i )
		MachLogRaces::instance().addToAllCollections( (*i) );
}

void perWrite( PerOstream& ostr, const MachLogConstructionPersistence& per )
{
	per.setUpLocalVectorFromViews();
	ostr << per.constructions_;
}

void perRead( PerIstream& istr, MachLogConstructionPersistence& per )
{
	istr >> per.constructions_;
	per.setUpViewsFromLocalVector();
}


/* End PERCONS.CPP **************************************************/
