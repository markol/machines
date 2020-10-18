/*
 * M A C H P E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/private/machper.hpp"

#include "machphys/administ.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/apc.hpp"
#include "machphys/construc.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/technici.hpp"

#include "machphys/ofactory.hpp"

PER_DEFINE_PERSISTENT( MachPhysMachinePersistence );

// static
MachPhysMachinePersistence& MachPhysMachinePersistence::instance()
{
    static MachPhysMachinePersistence instance_;
    return instance_;
}

MachPhysMachinePersistence::MachPhysMachinePersistence()
{

    TEST_INVARIANT;
}

MachPhysMachinePersistence::~MachPhysMachinePersistence()
{
    TEST_INVARIANT;

}

void MachPhysMachinePersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachinePersistence& t )
{

    o << "MachPhysMachinePersistence " << (void*)&t << " start" << std::endl;
    o << "MachPhysMachinePersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysMachinePersistence& )
{
    ostr << MachPhysAdministrator::factory();
    ostr << MachPhysAggressor::factory();
    ostr << MachPhysAPC::factory();
    ostr << MachPhysConstructor::factory();
    ostr << MachPhysGeoLocator::factory();
    ostr << MachPhysResourceCarrier::factory();
    ostr << MachPhysSpyLocator::factory();
    ostr << MachPhysTechnician::factory();
}

void perRead( PerIstream& istr, MachPhysMachinePersistence& )
{
    istr >> MachPhysAdministrator::factory();
    istr >> MachPhysAggressor::factory();
    istr >> MachPhysAPC::factory();
    istr >> MachPhysConstructor::factory();
    istr >> MachPhysGeoLocator::factory();
    istr >> MachPhysResourceCarrier::factory();
    istr >> MachPhysSpyLocator::factory();
    istr >> MachPhysTechnician::factory();
}

/* End MACHPER.CPP **************************************************/
