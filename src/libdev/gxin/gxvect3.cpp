/*
 * G X V E C T 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxvect3.hpp"

GXVect3::GXVect3():
x_(0),
y_(0),
z_(0)
{

    TEST_INVARIANT;
}

GXVect3::~GXVect3()
{
    TEST_INVARIANT;

}

void GXVect3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXVect3& t )
{

    o << t.x_ << " " << t.y_ << " " << t.z_;
    return o;
}
GXVect3::GXVect3(const GXVect3& copy):
x_(copy.x_),
y_(copy.y_),
z_(copy.z_)
{

 POST(*this==copy);
 TEST_INVARIANT;
}

GXVect3& GXVect3::operator=(const GXVect3& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    x_=copy.x_;
	y_=copy.y_;
	z_=copy.z_;
  }

  POST(*this==copy)
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXVect3 &point1, const GXVect3 &point2)
{
  bool result=true;

  if ((point1.x_!=point2.x_) ||
      (point1.y_!=point2.y_) ||
	  (point1.z_!=point2.z_))
	  result=false;

  return result;
}


/* End GXVECT3.CPP **************************************************/
