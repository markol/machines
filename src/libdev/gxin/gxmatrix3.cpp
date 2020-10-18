/*
 * G X M A T R I X 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxmatrix3.hpp"

GXMatrix3::GXMatrix3()
{

    TEST_INVARIANT;
}

GXMatrix3::~GXMatrix3()
{
    TEST_INVARIANT;

}

void GXMatrix3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXMatrix3& t )
{

    o << std::endl;
	o << t.a_ << std::endl;
	o << t.b_ << std::endl;
	o << t.c_ << std::endl;

    return o;
}

GXMatrix3::GXMatrix3(const GXMatrix3& copy):
a_(copy.a_),
b_(copy.b_),
c_(copy.c_)
{

 POST(*this==copy);
 TEST_INVARIANT;
}

GXMatrix3& GXMatrix3::operator=(const GXMatrix3& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    a_=copy.a_;
	b_=copy.b_;
	c_=copy.c_;
  }

  POST(*this==copy)
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXMatrix3 &mx1, const GXMatrix3 &mx2)
{
  bool result=false;

  if ((mx1.a_==mx2.a_) &&
      (mx1.b_==mx2.b_) &&
	  (mx1.c_==mx2.c_))
	  result=true;

  return result;
}

/* End GXMATRIX3.CPP ************************************************/
