/*
 * H W L A B I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/hwlabi.hpp"
#include "sim/manager.hpp"
#include "machlog/resitem.hpp"

PER_DEFINE_PERSISTENT( MachLogHardwareLabImpl );

MachLogHardwareLabImpl::MachLogHardwareLabImpl( MachPhys::HardwareLabSubType subType )
:	subType_( subType ),
	lastUpdateTime_( SimManager::instance().currentTime() )
{

    TEST_INVARIANT;
}

MachLogHardwareLabImpl::~MachLogHardwareLabImpl()
{
    TEST_INVARIANT;

}

void MachLogHardwareLabImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogHardwareLabImpl& t )
{

    o << "MachLogHardwareLabImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogHardwareLabImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogHardwareLabImpl& actorImpl )
{
	ostr << actorImpl.availableResearchItems_;
	ostr << actorImpl.currentResearchQueue_;
	ostr << actorImpl.lastUpdateTime_;
	ostr << actorImpl.subType_;	
}

void perRead( PerIstream& istr, MachLogHardwareLabImpl& actorImpl )
{
 	istr >> actorImpl.availableResearchItems_;
	istr >> actorImpl.currentResearchQueue_;
	istr >> actorImpl.lastUpdateTime_;
	istr >> actorImpl.subType_;
}

MachLogHardwareLabImpl::MachLogHardwareLabImpl( PerConstructor )
{
}
/* End HWLABI.CPP ***************************************************/
