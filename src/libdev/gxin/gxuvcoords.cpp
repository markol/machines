/*
 * G X U V C O O R D S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxuvcoords.hpp"

GXUVCoords::GXUVCoords():
u_(0.0),
v_(0.0)
{

    TEST_INVARIANT;
}

GXUVCoords::~GXUVCoords()
{
    TEST_INVARIANT;

}

GXUVCoords::GXUVCoords(const GXUVCoords& copy):
u_(copy.u_),
v_(copy.v_)
{
  POST(*this==copy);
  TEST_INVARIANT;
}

GXUVCoords& GXUVCoords::operator=(const GXUVCoords& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    u_=copy.u_;
	v_=copy.v_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXUVCoords& uv1, const GXUVCoords& uv2)
{
  bool result=false;

  if ((uv1.u_==uv2.u_) &&
      (uv1.v_==uv2.v_))
	result=true;

  return result;
}

bool operator < (const GXUVCoords& uv1, const GXUVCoords& uv2)
{
  bool result=false;

  if (uv1.u_<uv2.u_)
  {
    result=true;
  }
  else if (uv1.u_==uv2.u_)
  {
    if (uv1.v_<uv2.v_)
    {
      result=true;
    }
  }

  return result;
}

void GXUVCoords::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXUVCoords& t )
{

    o << "u_ " << t.u_ << "\tv_" << t.v_ << std::endl;
    return o;
}

/* End GXUVCOORDS.CPP ***********************************************/
