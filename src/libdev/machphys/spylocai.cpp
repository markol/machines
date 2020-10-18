/*
 * S P Y L O C A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/spylocai.hpp"

PER_DEFINE_PERSISTENT( MachPhysSpyLocatorImpl );

MachPhysSpyLocatorImpl::MachPhysSpyLocatorImpl()
: isLocating_( false ),
  locatingPlanEndTime_( 0 )
{

    TEST_INVARIANT;
}

MachPhysSpyLocatorImpl::~MachPhysSpyLocatorImpl()
{
    TEST_INVARIANT;

}

void MachPhysSpyLocatorImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSpyLocatorImpl& t )
{

    o << "MachPhysSpyLocatorImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysSpyLocatorImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysSpyLocatorImpl::MachPhysSpyLocatorImpl( PerConstructor )
: isLocating_( false ),
  locatingPlanEndTime_( 0 )
{
}

void perWrite( PerOstream&, const MachPhysSpyLocatorImpl& )
{
}

void perRead( PerIstream&, MachPhysSpyLocatorImpl& )
{
}

/* End SPYLOCAI.CPP *************************************************/
