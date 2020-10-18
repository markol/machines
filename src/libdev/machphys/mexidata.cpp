/*
 * M E X I D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/mexidata.hpp"
#include "machphys/mexpdata.hpp"
//#include "world4d/link.hpp"

#ifndef _INLINE
    #include "machphys/internal/mexidata.ipp"
#endif

PER_DEFINE_PERSISTENT( MachPhysIMachineExplosionData );

MachPhysIMachineExplosionData::MachPhysIMachineExplosionData()
: maxSize_( 0.0 ),
  minToShootOff_( 0 ),
  maxToShootOff_( MachPhysMachineExplosionData::ALL_LINKS )
{

    TEST_INVARIANT;
}

MachPhysIMachineExplosionData::MachPhysIMachineExplosionData( const MachPhysIMachineExplosionData& copyMe )
: links_( copyMe.links_ ),
  maxSize_( copyMe.maxSize_ ),
  minToShootOff_( copyMe.minToShootOff_ ),
  maxToShootOff_( copyMe.maxToShootOff_ ),
  explosionCenter_( copyMe.explosionCenter_ )
{

    TEST_INVARIANT;
}

MachPhysIMachineExplosionData::MachPhysIMachineExplosionData( PerConstructor )
: maxToShootOff_( MachPhysMachineExplosionData::ALL_LINKS )
{
}

MachPhysIMachineExplosionData::~MachPhysIMachineExplosionData()
{
    TEST_INVARIANT;

}

void MachPhysIMachineExplosionData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysIMachineExplosionData& t )
{

    o << "MachPhysIMachineExplosionData " << (void*)&t << " start" << std::endl;
    o << "MachPhysIMachineExplosionData " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysIMachineExplosionData& data )
{
    ostr << data.links_;
    ostr << data.maxSize_;
    ostr << data.minToShootOff_;
    ostr << data.maxToShootOff_;
    ostr << data.explosionCenter_;
}

void perRead( PerIstream& istr, MachPhysIMachineExplosionData& data )
{
    istr >> data.links_;
    istr >> data.maxSize_;
    istr >> data.minToShootOff_;
    istr >> data.maxToShootOff_;
    istr >> data.explosionCenter_;
}
void perWrite( PerOstream& ostr, const MachPhysMachineExplosionData::LinkData& data )
{
    ostr << data.initialised_;
    ostr << data.linkId_;
    ostr << data.size_;
    ostr << data.discreteSize_;
    ostr << data.shootOffProbability_;
    ostr << data.minExplosionTime_;
    ostr << data.maxExplosionTime_;
}

void perRead( PerIstream& istr, MachPhysMachineExplosionData::LinkData& data )
{
    istr >> data.initialised_;
    istr >> data.linkId_;
    istr >> data.size_;
    istr >> data.discreteSize_;
    istr >> data.shootOffProbability_;
    istr >> data.minExplosionTime_;
    istr >> data.maxExplosionTime_;
}

/* End MEXIDATA.CPP *************************************************/
