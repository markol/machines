/*
 * G X C O L O R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxcolor.hpp"
#include "gxin/gxerror.hpp"

GXColor::GXColor():
red_(0),
green_(0),
blue_(0)
{

    TEST_INVARIANT;
}

GXColor::~GXColor()
{
    TEST_INVARIANT;

}

GXColor::GXColor(const GXColor&	copy):
red_(copy.red_),
green_(copy.green_),
blue_(copy.blue_)
{
  POST(*this==copy);
  TEST_INVARIANT;
}
GXColor& GXColor::operator=(const GXColor& copy)
{
  TEST_INVARIANT;
  if (this!=&copy)
  {
    red_=copy.red_;
	green_=copy.green_;
	blue_=copy.blue_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator<(const GXColor& color1, const GXColor& color2)
{
  bool result;

  // compare red
  if (color1.red_<color2.red_)
  {
    result = true;
	return result;
  }
  if (color1.red_>color2.red_)
  {
    result=false;
    return result;
  }

  // reds are equals
  // compare green
  if (color1.green_<color2.green_)
  {
    result = true;
	return result;
  }
  if (color1.green_>color2.green_)
  {
    result=false;
    return result;
  }

  // greens are equals
  // compare blue
  if (color1.blue_<color2.blue_)
  {
    result = true;
	return result;
  }
  if (color1.blue_>color2.blue_)
  {
    result=false;
    return result;
  }

  // reds equals green equals blue equals
  return false;
}

bool operator==(const GXColor &color1, const GXColor& color2)
{
  bool result=true;

  if ((color1.red_!=color2.red_) ||
      (color1.green_!=color2.green_) ||
	  (color1.blue_!=color2.blue_))
	  result=false;

  return result;
}

void GXColor::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXColor& t )
{

    o << "red " << (USHORT)t.red_ << " green " << (USHORT)t.green_ << " blue " << (USHORT)t.blue_;
    return o;
}

/* End GXCOLOR.CPP **************************************************/
