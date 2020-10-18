/*
 * G X V E C T 4 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxvect4.hpp"

GXVect4::GXVect4():
x_(0),
y_(0),
z_(0),
h_(0)
{

    TEST_INVARIANT;
}

GXVect4::~GXVect4()
{
    TEST_INVARIANT;

}

void GXVect4::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXVect4& t )
{

    o << t.x_ << "\t" << t.y_ << "\t" << t.z_ << "\t" << t.h_;
    return o;
}
GXVect4::GXVect4(const GXVect4& copy):
x_(copy.x_),
y_(copy.y_),
z_(copy.z_),
h_(copy.h_)
{

 POST(*this==copy);
 TEST_INVARIANT;
}

GXVect4& GXVect4::operator=(const GXVect4& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    x_=copy.x_;
	y_=copy.y_;
	z_=copy.z_;
	h_=copy.h_;
  }

  POST(*this==copy)
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXVect4 &point1, const GXVect4 &point2)
{
  bool result=true;

  if ((point1.x_!=point2.x_) ||
      (point1.y_!=point2.y_) ||
	  (point1.z_!=point2.z_) ||
	  (point1.h_!=point2.h_))
	  result=false;

  return result;
}


/* End GXVECT3.CPP **************************************************/
