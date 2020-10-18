/*
 * M I S S I L E I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/missilei.hpp"
#include "machphys/vaptrail.hpp"
#include "world4d/link.hpp"

PER_DEFINE_PERSISTENT( MachPhysMissileImpl );

MachPhysMissileImpl::MachPhysMissileImpl( size_t level )
: pVapourTrail_( NULL ),
  destructionTime_( 0.0 ),
  level_(level),
  pFlame_( NULL )
{

    TEST_INVARIANT;
}

MachPhysMissileImpl::~MachPhysMissileImpl()
{
    TEST_INVARIANT;

	_DELETE( pVapourTrail_ );

}

void MachPhysMissileImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMissileImpl& t )
{

    o << "MachPhysMissileImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysMissileImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysMissileImpl::MachPhysMissileImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysMissileImpl& missileImpl )
{
    ostr << missileImpl.pVapourTrail_;
    ostr << missileImpl.vapourTrailOffset_;
    ostr << missileImpl.destructionTime_;
    ostr << missileImpl.level_;
    ostr << missileImpl.pFlame_;
}

void perRead( PerIstream& istr, MachPhysMissileImpl& missileImpl )
{
    istr >> missileImpl.pVapourTrail_;
    istr >> missileImpl.vapourTrailOffset_;
    istr >> missileImpl.destructionTime_;
    istr >> missileImpl.level_;
    istr >> missileImpl.pFlame_;
}

/* End MISSILEI.CPP *************************************************/
