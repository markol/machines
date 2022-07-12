/*
 * G X L A B E L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXLabel

    A brief description of the class should go in here
*/

#ifndef _GXLABEL_HPP
#define _GXLABEL_HPP
#include <string>

#include "base/base.hpp"
#include "ctl/vector.hpp"

#define LABELLENGTH 25
#define MAXLABEL    34
#define LABELLIST   \
  {                       \
  "",                     \
  "Objects",              \
  "QuadMesh",             \
  "TriMesh",              \
  "3DPointArray",         \
  "TriangleArray",        \
  "QuadrangleArray",      \
  "NormalArray",          \
  "Polygon",              \
  "PolygonVertex",        \
  "DirectionnalLight",    \
  "PointLight",           \
  "SpotLight",            \
  "Camera",               \
  "Hierarchy",            \
  "Child",                \
  "Transform",            \
  "Matrix4X4",            \
  "Matrix3X3",            \
  "ComponentTransform",   \
  "Textures",             \
  "BitmapTexture",        \
  "Animation",            \
  "AnimateParameter",     \
  "KeyFrame",             \
  "UserData",             \
  "UserDataNode",         \
  "AGIDArray",            \
  "ChangeVertex3D",       \
  "SkinMeshUserData", \
  "SkinBoneUserData", \
  "SkinLinkArray",    \
  "SkinMeshArray",    \
  "SkinVertexArray"}

typedef enum
{
  NOLABEL,
  OBJECTS,
  QUADMESH,
  TRIMESH,
  POINTARRAY,

  TRIANGLEARRAY,
  QUADRANGLEARRAY,
  NORMALARRAY,
  POLYGON,
  POLYGONVERTEX,

  DIRECTIONNALLIGHT,
  POINTLIGHT,
  SPOTLIGHT,
  CAMERA,
  HIERARCHY,

  CHILD,
  TRANSFORM,
  MATRIX4X4,
  MATRIX3X3,
  COMPONENTTRANSFORM,

  TEXTURES,
  BITMAPTEXTURE,
  ANIMATION,
  ANIMATEPARAMETER,
  KEYFRAME,

  USERDATA,
  USERDATANODE,
  AGIDARRAY,
  CHANGEVERTEX3D,
  SKINMESHUSERDATA,
  SKINBONEUSERDATA,

  SKINLINKARRAY,
  SKINMESHARRAY,
  SKINVERTEXARRAY
} LabelAGT;


class GXLabel
// Canonical form revoked
{
public:
    GXLabel();
    ~GXLabel();

    void CLASS_INVARIANT;

	const LabelAGT type() const {
	  return type_;
	}

	void nameAppend(char character);

    // Reinitialise label
    void init();

	// find the label type corrensponding to the std::string  name_
	// affects it to type_, returns it
    LabelAGT findLabelType();

    friend ostream& operator <<( ostream& o, const GXLabel& t );

private:
    GXLabel( const GXLabel& );
    GXLabel& operator =( const GXLabel& );
    bool operator ==( const GXLabel& );

    // Reinitialise label
    //void init();

    static std::string  labelList_[MAXLABEL];


    LabelAGT type_;
	std::string  name_;

};


#endif

/* End GXLABEL.HPP **************************************************/
