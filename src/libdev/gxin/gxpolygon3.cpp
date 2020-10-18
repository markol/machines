/*
 * G X P O L Y G O N 3 . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxpolygon3.hpp"

GXPolygon3::GXPolygon3():
flag_(0),
flag2_(0)
{

	mat_.hasOneColor(true);

    TEST_INVARIANT;
}

GXPolygon3::~GXPolygon3()
{
    TEST_INVARIANT;

}

GXPolygon3::GXPolygon3(const GXPolygon3& copy):
vertices_(copy.vertices_),
flag_(copy.flag_),
flag2_(copy.flag2_),
mat_(copy.mat_)
{
  POST(*this==copy);
  TEST_INVARIANT;
}

GXPolygon3& GXPolygon3::operator=(const GXPolygon3& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    vertices_=copy.vertices_;
	flag_=copy.flag_;
	flag2_=copy.flag2_;
	mat_=copy.mat_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXPolygon3& polygon1, const GXPolygon3& polygon2)
{
  bool result=false;

  if (
      (polygon1.vertices_ == polygon2.vertices_) &&
      (polygon1.flag_ == polygon2.flag_) &&
      (polygon1.flag2_ == polygon2.flag2_) &&
	  (polygon1.mat_ == polygon2.mat_)
	 )
	  result=true;

  return result;
}

void GXPolygon3::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	// exclusive or
	INVARIANT( allVerticesColored() || noVertexColored());
	INVARIANT( !(allVerticesColored() && noVertexColored()));
}

bool GXPolygon3::allVerticesColored( void )	const
{
  bool result=false;

  if (vertices_.size()>0)
  {
	result=true;
	int i=0;
	while (result && i<vertices_.size())
	{
      result=vertices_[i].hasColor();
	  ++i;
	}
  }
  return result;
}

bool GXPolygon3::noVertexColored(void) const
{
  bool result=true;
  for (int i=0; i<vertices_.size(); ++i)
  {
    if (vertices_[i].hasColor()==true)
	{
	  result=false;
	}
  }
  return result;
}

ostream& operator <<( ostream& o, const GXPolygon3& t )
{
	o << std::endl;
    o << "vertices_ { " << std::endl;
    for (int i=0; i<t.numVertices(); i++)
      o << t.vertices_[i] << std::endl;
	o << "}" << std::endl;
	o << "flag_ " << (USHORT)t.flag_ << std::endl;;
	o << "mat_ { "<< t.mat_ << "}";
	o << std::endl;
    return o;
}

int GXPolygon3::numVertices() const
{
  return vertices_.size();
}

void GXPolygon3::numVertices(int newNumVertices)
{
  // TODO: switch to resize as soon as
  // it is implemented
  vertices_.reserve(newNumVertices);
}

void GXPolygon3::addVertex(const GXPolyVert3& newVertex)
{
  TEST_INVARIANT;
  if (vertices_.size()==0)
  {
	// Note the first recorded vertex set the status of the polygon
	// which laterly recorded vertices have to be consistent with
    vertices_.push_back(newVertex);
    mat_.hasOneColor(!newVertex.hasColor());
  }
  else
  {
     ASSERT(iff(newVertex.hasColor(),allVerticesColored()), runtime_error("New vertex non consistent without previously recorded vertices"));
	 vertices_.push_back(newVertex);
  }

  TEST_INVARIANT;
}

const GXColor& GXPolygon3::diffuseColor() const
{
  PRE(hasOneColor());
  return mat_.diffuseColor();
}

void GXPolygon3::setColorStatus()
{
  TEST_INVARIANT;

  // if noVertexColored() is true,
  // the color status has been set yet (hasOneColor is true)


  if (!noVertexColored())
  {
	// the vertices are colored

    bool oneColorFound=true;
    for (int i=0; (i<vertices_.size() && oneColorFound); ++i)
    {
      if (!(vertices_[i].color()==vertices_[0].color()))
	  {
	    oneColorFound=false;
	  }
    }

    if (oneColorFound==true)
    {
      // change the color status
      mat_.diffuseColor(vertices_[0].color());
	  for (int i=0; i<vertices_.size(); i++)
	  {
	      vertices_[i].hasColor(false);
	  }
	  ASSERT(noVertexColored(),"");
    }
  }

  // Ensure that the material is in a consistant status with this
  // instance
  mat_.hasOneColor(noVertexColored());

  TEST_INVARIANT;
}


/* End GXPOLYGON3.CPP ***********************************************/
