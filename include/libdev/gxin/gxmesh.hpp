/*
 * G X M E S H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _GXMESH_HPP
#define _GXMESH_HPP

#include <string>
#include <iostream>

#include "base/base.hpp"
#include "ctl/vector.hpp"

#include "gx.hpp"
#include "gxin/gxidpos.hpp"
#include "gxin/gxpoint3.hpp"
#include "gxin/gxvect3.hpp"
#include "gxpolygon3.hpp"
#include "gxtextures.hpp"
#include "gxhier.hpp"

// Class derived from Gamut-X data structure

class GXMesh {
// Canonical form revoked
public:
    GXMesh();
    ~GXMesh();

    friend ostream& operator <<( ostream& o, const GXMesh& t );

    void CLASS_INVARIANT;

	bool hasName() const { return hasName_; }
    const GXName& name() const { return name_; }
    void name(const GXName& newName) { name_=newName; }

    const GXIdPos& id() const {
      return id_;
    }
    void id(const GXIdPos& newId) {
      id_=newId;
    }
// points_ access methods

    // Get the size of the points array
    int numPoints() const;

	// Resize the points array
	void numPoints(int newNumPoint);

    // Set and get methods for GXPoint3
    // Warning: This set method is useless up
    // until the resize method on vectors has
    // been implemented
    GXPoint3& point(int i) {
      return points_[i];
	}
	const GXPoint3& point(int i) const {
	  return points_[i];
	}

	// Add a point to the array,
	void addPoint(const GXPoint3& newPoint);

// normals_ access methods

    // Get the size of the normals array
    int numNormals() const;

	// Resize the normals array
	void numNormals(int newNumNormals);

    // Set and get methods for GXPolygon3
    // Warning: This set method is useless up
    // until the resize method on vectors has
    // been implemented
    GXVect3& normal(int i) { return normals_[i]; }
	const GXVect3& normal(int i) const { return normals_[i];}
	// Add a normal to the array
	void addNormal(const GXVect3& newNormal);

// polygons_ access methods

    // Get the size of the polygons array
    int numPolygons() const;

	// Resize the normals array
	void numPolygons(int newNumPolygons);

    // Set and get methods for GXPolygon3
    // Warning: This set method is useless up
    // until the resize method on vectors has
    // been implemented
    GXPolygon3& polygon(int i) { return polygons_[i]; }
	const GXPolygon3& polygon(int i) const { return polygons_[i]; }
	// Add a polygon to the array
	void addPolygon(const GXPolygon3& newPolygon);

    bool hasTextures() { return hasTextures_;	}
	//const GXTexture& texture(const GXIdPos &id) const;
	const GXTexture& texture(const GXIdPos &id);
	// PRE(hasTextureWithId(id));

	// true if id is a key of the textures_ map
	bool hasTextureWithId(const GXIdPos& id) const;

    // set the textures names of all the polygons
	//GXError setTextures(const GXTextures& );
	GXError setTextures( GXTextures& );
	// POST(iff(hasTextures(),(result==NO_ERROR_AGT));

	bool hasName() { return hasName_; }
	// set the names of the mesh using the hierarchy
	bool setName(GXHier& );

private:
   GXMesh( const GXMesh& );
   GXMesh& operator =( const GXMesh& );
   bool operator ==( const GXMesh& );

   // true if the mesh has been found in the hierarchy
   // structure, and the name set
   bool hasName_;

   // Apparently not implemented in the AGT file format.
   // Get the information from @Hierarchy
   GXName name_;

  // internal used to match the hierarchies nodes with the
  // mesh with affecting meshes names
  GXIdPos id_;

  ctl_vector<GXPoint3> points_;
  // Get it from @3DPointArray

  ctl_vector<GXVect3>  normals_;
  // Get the normals from @NormalArray

  ctl_vector<GXPolygon3> polygons_;
  // Get the polygons from @Polygon

  // True when setTextures has been called
  bool hasTextures_;
  GXTextures textures_;

//  GXColor diffuseColor_;
  // Not implemented in AGT file format
  // TODO, either get it from the vertices colors, or @UserData

//  GXColor emissiveColor_;
  // Not implemented in AGT file format, get it from @UserData

//  GXColor specularColor_;
  // Not implemented in file format, probably useless

};

#endif
