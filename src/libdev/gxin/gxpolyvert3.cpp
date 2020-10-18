/*
 * G X P O L Y V E R T 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>
#include "gxin/gxpolyvert3.hpp"

GXPolyVert3::GXPolyVert3():
pointIndex_(0),
normalIndex_(0),
color_(NULL)
{

    TEST_INVARIANT;
}

GXPolyVert3::~GXPolyVert3()
{
    TEST_INVARIANT;
	hasColor(false);

}

GXPolyVert3::GXPolyVert3( const GXPolyVert3& copy):
pointIndex_(copy.pointIndex_),
normalIndex_(copy.normalIndex_),
color_(NULL),
uv_(copy.uv_)
{

  hasColor(copy.hasColor());
  if (hasColor())
  {
    color(copy.color());
  }

  POST(*this==copy);
  TEST_INVARIANT;
}

GXPolyVert3& GXPolyVert3::operator =(const GXPolyVert3& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    pointIndex_=copy.pointIndex_;
	normalIndex_=copy.normalIndex_;
	hasColor(copy.hasColor());
	if (hasColor())
	{
	 color(copy.color());
	}
	uv_=copy.uv_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXPolyVert3& vertex1, const GXPolyVert3& vertex2)
{
  bool result=false;

  if (
      (vertex1.pointIndex_==vertex2.pointIndex_) &&
      (vertex1.normalIndex_==vertex2.normalIndex_) &&
      (vertex1.uv_==vertex2.uv_) &&
      (vertex1.hasColor()==vertex2.hasColor()) &&
	  ((! vertex1.hasColor()) || (vertex1.color()==vertex2.color()))
	 )
	  result=true;

  return result;
}

bool operator < (const GXPolyVert3& vtx1, const GXPolyVert3& vtx2)
{
  bool result=false;

  if (vtx1.pointIndex_<vtx2.pointIndex_)
  {
    result=true;
  }
  else if (vtx1.pointIndex_==vtx2.pointIndex_)
  {
    if (vtx1.normalIndex_<vtx2.normalIndex_)
    {
       result=true;
    }
    else if (vtx1.normalIndex_==vtx2.normalIndex_)
    {
      if (vtx1.uv()<vtx2.uv())
      {
        result=true;
      }
      else if (vtx1.uv()==vtx2.uv())
      {
        if (vtx1.hasColor()<vtx2.hasColor())
        {
          result=true;
        }
        else if (vtx1.hasColor()==vtx2.hasColor())
        {
          if (vtx1.hasColor()&& (vtx1.color()<vtx2.color()))
          {
            result =true;
          }
        }
      }
    }
  }
  return result;
}

void GXPolyVert3::hasColor(bool newHasColor)
{
  if ((newHasColor==false) && (hasColor()==true))
  {
   _DELETE(color_);
   color_=NULL;
  }
  else if ((newHasColor==true) && (hasColor()==false))
  {
	  color_=_NEW(GXColor);
  }

  POST(hasColor()==newHasColor);
}

void GXPolyVert3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXPolyVert3& t )
{

    o << "pointIndex_ " << t.pointIndex_ << " normalIndex_ " << t.normalIndex_;
	if (t.hasColor())
	{
     o << " color_ { " << *(t.color_) << "}";
	}

    return o;
}

/* End GXPOLYVERT3.CPP **********************************************/
