/*
 * G X P O I N T 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxpoint3.hpp"

GXPoint3::GXPoint3():
x_(0),
y_(0),
z_(0)
{

    TEST_INVARIANT;
}

GXPoint3::~GXPoint3()
{
    TEST_INVARIANT;

}

void GXPoint3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXPoint3& t )
{

    o << t.x_ << "\t" << t.y_ << "\t" << t.z_;
    return o;
}

GXPoint3::GXPoint3(const GXPoint3& copy):
x_(copy.x_),
y_(copy.y_),
z_(copy.z_)
{

 POST(*this==copy);
 TEST_INVARIANT;
}

GXPoint3& GXPoint3::operator=(const GXPoint3& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    x_=copy.x_;
	y_=copy.y_;
	z_=copy.z_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXPoint3 &point1, const GXPoint3& point2)
{
  bool result=true;

  if ((point1.x_!=point2.x_) ||
      (point1.y_!=point2.y_) ||
	  (point1.z_!=point2.z_))
	  result=false;

  return result;
}

/* End GXPOINT3.CPP *************************************************/
