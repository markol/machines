/*
 * S P H E R E 3 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/sphere3d.hpp"
#include "mathex/sausage3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"

MexSphere3d::MexSphere3d():
 radius_(0)
{

    TEST_INVARIANT;
}

MexSphere3d::MexSphere3d( const MexPoint3d& center, MATHEX_SCALAR radius):
 center_(center),
 radius_(radius)
{

    TEST_INVARIANT;
}

MexSphere3d::~MexSphere3d()
{
    TEST_INVARIANT;

}

void MexSphere3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MexSphere3d::transform( const MexTransform3d& t )
{
  t.transform(&center_);
}

bool MexSphere3d::intersects( const MexSausage3d & otherShape ) const
{
  PRE(otherShape.length()>0);

  bool result;

  MATHEX_SCALAR axisDistance=MexLine3d::sqrEuclidianDistance(
                                   otherShape.startPoint(), otherShape.endPoint(), otherShape.length(),
                                   center());

  result=(axisDistance <= (radius()+otherShape.radius())*(radius()+otherShape.radius()));

  return result;
}

bool MexSphere3d::intersects( const MexSphere3d & otherShape ) const
{

  bool result;

  MATHEX_SCALAR axisDistance=center().sqrEuclidianDistance(otherShape.center());

  result=(axisDistance <= (radius()+otherShape.radius())*(radius()+otherShape.radius()));

  return result;

}

ostream& operator <<( ostream& o, const MexSphere3d& t )
{

	o << std::endl;
    o << "center_ " << t.center_ << std::endl;
    o << "radius_ " << t.radius_ << std::endl;
    return o;
}

/* End HASPHERE3D.CPP *************************************************/
