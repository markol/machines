/*
 * G X M E S H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxmesh.hpp"

GXMesh::GXMesh():
hasName_(false),
name_("noname"),
hasTextures_(false)
{

    TEST_INVARIANT;
}

GXMesh::~GXMesh()
{
    TEST_INVARIANT;

}

void GXMesh::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXMesh& t )
{
	o << std::endl;
    o << "id_ { " << t.id_ << " }" << std::endl;
	o << "hasName_" << t.hasName_ << std::endl;
    if (t.hasName_)
      o << "name_ " << t.name_ << std::endl;
	o << "points_ {" << std::endl;
	for (int i=0; i<t.numPoints(); i++) {
	  o << t.points_[i] << std::endl;
	}
	o << "}" << std::endl;
	o << "normals_ {" << std::endl;
	for (int i=0; i<t.numNormals(); i++) {
	  o << t.normals_[i] << std::endl;
	}
	o << "}" << std::endl;
	o << "polygons_ {" << std::endl;
	for (int i=0; i<t.numPolygons(); i++) {
	  o << "{" << t.polygons_[i] << "}" << std::endl;
	}
	o << "}" << std::endl;
	o << "hasTextures_  " << t.hasTextures_ << std::endl;
	if (t.hasTextures_)
 	{
	  o << "textures_ { " << std::endl;
	  for (GXTextures::const_iterator it=t.textures_.begin();
		   it!=t.textures_.end(); ++it)	{
		o << "texture { " << (*it).first << "}: " << (*it).second << std::endl;
	  }
      o << "}" << std::endl;
	}
    return o;
}

void GXMesh::addPoint(const GXPoint3& newPoint)
{
  points_.push_back(newPoint);
}

int GXMesh::numPoints() const
{
  return points_.size();
}

void GXMesh::numPoints(int newNumPoint)
{
  // TODO: switch to resize as soon as
  // it is implemented
  points_.reserve(newNumPoint);
}

int GXMesh::numNormals() const
{
  return normals_.size();
}

void GXMesh::numNormals(int newNumNormals)
{
  // TODO: switch to resize as soon as
  // it is implemented
  normals_.reserve(newNumNormals);
}

void GXMesh::addNormal(const GXVect3& newNormal)
{
  normals_.push_back(newNormal);
}

int GXMesh::numPolygons() const
{
  return polygons_.size();
}

void GXMesh::numPolygons(int newNumPolygons)
{
  // TODO: switch to resize as soon as
  // it is implemented
  polygons_.reserve(newNumPolygons);
}

void GXMesh::addPolygon(const GXPolygon3& newPolygon)
{
  polygons_.push_back(newPolygon);
}

GXError GXMesh::setTextures(GXTextures &textures)
{
  GXError result=NO_ERROR_AGT;
  GXIdPos txId;
  GXIdPos zeroId;

  for (int polygonIndex=0;
       polygonIndex<numPolygons();
       ++polygonIndex)
  {
	txId=polygon(polygonIndex).mat().textureId();
	if (txId!=zeroId)
	{
	  if (textures_.find(txId)==textures_.end())
	  {
	    if (textures.find(txId)==textures.end())
	    {
	      result=NO_TEXTURE_AGT;
	    }
	    else
	    {
	      GXTextures::value_type new_val(txId, textures[txId]);
		  textures_.insert(new_val);
		  polygon(polygonIndex).hasValidTexture(true);
	    }
	  }
	  else
	  {
		polygon(polygonIndex).hasValidTexture(true);
	    ASSERT_DATA(bool noNewTex = (textures.find(txId)==textures.end()));
	    ASSERT((noNewTex || (textures[txId]==textures_[txId])),
	         runtime_error("Different texture names for polygon"));
	  }
	}
  }	// for (polygonIndex...

  if (result==NO_ERROR_AGT)
    hasTextures_=true;

  POST(
       ((result==NO_ERROR_AGT)&&hasTextures()) ||
       ((result==NO_TEXTURE_AGT)&&!hasTextures())
      );

  return result;

}

bool GXMesh::hasTextureWithId(const GXIdPos &id) const
{
  return !(textures_.find(id)==textures_.end());
}

const GXTexture& GXMesh::texture(const GXIdPos &id)
{
  PRE(hasTextureWithId(id));
  return textures_[id];
}

bool GXMesh::setName(GXHier& hierarchy)
{

  GXName hierName;
  bool result;

  // get the node corresponding to the mesh
  hierName=hierarchy.getMeshName(id_, result);
  if (result==true)
  {
    hasName_=result;
	name_=hierName;
  }
  return result;
}

/* End GXMESH.CPP ***************************************************/
