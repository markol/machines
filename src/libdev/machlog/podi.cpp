/*
 * P O D I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/podi.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachLogPodImpl );

MachLogPodImpl::MachLogPodImpl()
:	lastUpdateTime_( SimManager::instance().currentTime() ),						 
	hasIonCannon_( false ),
	ionCannonCharged_( false ),
	addedBMUStorageToRace_( false )
{
    TEST_INVARIANT;
}

MachLogPodImpl::~MachLogPodImpl()
{
    TEST_INVARIANT;

}

void MachLogPodImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPodImpl& t )
{

    o << "MachLogPodImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogPodImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPodImpl& actorImpl )
{
	ostr << actorImpl.lastUpdateTime_;
	ostr << actorImpl.hasIonCannon_;
	ostr << actorImpl.ionCannonCharged_;
	ostr << actorImpl.addedBMUStorageToRace_;
}

void perRead( PerIstream& istr, MachLogPodImpl& actorImpl )
{
 	istr >> actorImpl.lastUpdateTime_;
	istr >> actorImpl.hasIonCannon_;
	istr >> actorImpl.ionCannonCharged_;
	istr >> actorImpl.addedBMUStorageToRace_;
}

MachLogPodImpl::MachLogPodImpl( PerConstructor )
{
}
/* End PODI.CPP ***************************************************/
