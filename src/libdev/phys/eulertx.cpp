/*
 * E U L E R T X . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "phys/eulertx.hpp"

PER_DEFINE_PERSISTENT( PhysEulerTransform3d );

PhysEulerTransform3d::PhysEulerTransform3d()
: angles_( 0.0, 0.0, 0.0 ),
  position_( 0.0, 0.0, 0.0 )
{

    TEST_INVARIANT;
}

PhysEulerTransform3d::PhysEulerTransform3d(
    const MexEulerAngles& angles )
: angles_( angles ),
  position_( 0.0, 0.0, 0.0 )
{
}

PhysEulerTransform3d::PhysEulerTransform3d(
    const MexEulerAngles& angles,
    const MexPoint3d& position )
: angles_( angles ),
  position_( position )
{
}

PhysEulerTransform3d::PhysEulerTransform3d( const PhysEulerTransform3d& copyMe )
: angles_( copyMe.angles_ ),
  position_( copyMe.position_ )
{
}

PhysEulerTransform3d::PhysEulerTransform3d( const MexTransform3d& copyMe )
: position_( copyMe.position() )
{
    copyMe.rotation( &angles_ );
}

PhysEulerTransform3d::~PhysEulerTransform3d()
{
    TEST_INVARIANT;

}

const MexEulerAngles& PhysEulerTransform3d::rotation( void ) const
{
    return angles_;
}

const MexPoint3d& PhysEulerTransform3d::position( void ) const
{
    return position_;
}

PhysEulerTransform3d& PhysEulerTransform3d::operator =( const PhysEulerTransform3d& copyMe )
{
    angles_ = copyMe.angles_;
    position_ = copyMe.position_;
    
    return *this;
}    

void PhysEulerTransform3d::transform( MexTransform3d* pTransform ) const
{
    pTransform->rotation( angles_ );
    pTransform->position( position_ );
}

void PhysEulerTransform3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysEulerTransform3d& t )
{

    o << t.angles_ << "  " << t.position_;

    return o;
}

void perWrite( PerOstream& ostr, const PhysEulerTransform3d& tx )
{
    ostr << tx.angles_;
    ostr << tx.position_;
}

void perRead( PerIstream& istr, PhysEulerTransform3d& tx )
{
    istr >> tx.angles_;
    istr >> tx.position_;
}

/* End EULERTX.CPP **************************************************/
