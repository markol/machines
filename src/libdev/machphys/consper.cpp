/*
 * C O N S P E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/private/consper.hpp"

#include "machphys/beacon.hpp"
#include "machphys/factory.hpp"
#include "machphys/garrison.hpp"
#include "machphys/mine.hpp"
#include "machphys/missemp.hpp"
#include "machphys/pod.hpp"
#include "machphys/smelter.hpp"
#include "machphys/hwlab.hpp"
#include "machphys/missile.hpp"

PER_DEFINE_PERSISTENT( MachPhysConstructionPersistence );

// static
MachPhysConstructionPersistence& MachPhysConstructionPersistence::instance()
{
    static MachPhysConstructionPersistence instance_;
    return instance_;
}

MachPhysConstructionPersistence::MachPhysConstructionPersistence()
{

    TEST_INVARIANT;
}

MachPhysConstructionPersistence::~MachPhysConstructionPersistence()
{
    TEST_INVARIANT;

}

void MachPhysConstructionPersistence::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysConstructionPersistence& t )
{

    o << "MachPhysConstructionPersistence " << (void*)&t << " start" << std::endl;
    o << "MachPhysConstructionPersistence " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysConstructionPersistence& )
{
    ostr << MachPhysBeacon::factory();
    ostr << MachPhysFactory::factory();
    ostr << MachPhysGarrison::factory();
    ostr << MachPhysMine::factory();
    ostr << MachPhysMissileEmplacement::factory();
    ostr << MachPhysPod::factory();
    ostr << MachPhysSmelter::factory();
    ostr << MachPhysHardwareLab::factory();
    ostr << MachPhysMissile::factory();
}

void perRead( PerIstream& istr, MachPhysConstructionPersistence& )
{
    istr >> MachPhysBeacon::factory();
    istr >> MachPhysFactory::factory();
    istr >> MachPhysGarrison::factory();
    istr >> MachPhysMine::factory();
    istr >> MachPhysMissileEmplacement::factory();
    istr >> MachPhysPod::factory();
    istr >> MachPhysSmelter::factory();
    istr >> MachPhysHardwareLab::factory();
    istr >> MachPhysMissile::factory();
}

/* End CONSPER.CPP **************************************************/
