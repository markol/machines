/*
 * G X L A B E L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxlabel.hpp"

// static
std::string  GXLabel::labelList_[MAXLABEL]=LABELLIST;

GXLabel::GXLabel():
type_(NOLABEL)
{

    TEST_INVARIANT;
}

GXLabel::~GXLabel()
{
    TEST_INVARIANT;

}

void GXLabel::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXLabel& t )
{
	o << "{" << std::endl;
	o << " name_ " << t.name_ << std::endl;
	o << " type_ " << t.type_ << std::endl;
	o << "}" << std::endl;

    return o;
}

void GXLabel::init( void )
{
  type_=NOLABEL;
  name_="";
}

void GXLabel::nameAppend(char character)
{
	name_=name_+character;
}

LabelAGT GXLabel::findLabelType()
{
  int  i              = 1;
  int       identical = 1;
  int       length    = 0;

  // do not use NOLABEL result for compare always true
  for(i = 1; (i < MAXLABEL) && (identical != 0); i++)
  {
    if (name_==labelList_[i])
      identical=0;
  }
  if(identical != 0)
    type_=NOLABEL;  // NOLABEL because we didn't find any match
  else
  {
   switch (i)
   {
   case 2:
    type_=OBJECTS;
	break;
   case 3:
   	type_=QUADMESH;
    break;
   case 4:
  	type_=TRIMESH;
	break;
   case 5:
  	type_=POINTARRAY;
	break;
   case 6:
    type_=TRIANGLEARRAY;
	break;
   case 7:
    type_=QUADRANGLEARRAY;
	break;
   case 8:
    type_=NORMALARRAY;
	break;
   case 9:
    type_=POLYGON;
	break;
   case 10:
    type_=POLYGONVERTEX;
	break;
   case 11:
    type_=DIRECTIONNALLIGHT;
	break;
   case 12:
    type_=POINTLIGHT;
	break;
   case 13:
    type_=SPOTLIGHT;
	break;
   case 14:
    type_=CAMERA;
	break;
   case 15:
    type_=HIERARCHY;
	break;
   case 16:
    type_=CHILD;
	break;
   case 17:
    type_=TRANSFORM;
	break;
   case 18:
    type_=MATRIX4X4;
	break;
   case 19:
    type_=MATRIX3X3;
	break;
   case 20:
    type_=COMPONENTTRANSFORM;
	break;
   case 21:
    type_=TEXTURES;
	break;
   case 22:
    type_=BITMAPTEXTURE;
	break;
   case 23:
    type_=ANIMATION;
	break;
   case 24:
    type_=ANIMATEPARAMETER;
	break;
   case 25:
    type_=KEYFRAME;
	break;
   case 26:
    type_=USERDATA;
	break;
   case 27:
    type_=USERDATANODE;
	break;
   case 28:
    type_=AGIDARRAY;
	break;
   case 29:
    type_=CHANGEVERTEX3D;
	break;
   case 30:
    type_=SKINMESHUSERDATA;
	break;
   case 31:
    type_=SKINBONEUSERDATA;
	break;
   case 32:
    type_=SKINLINKARRAY;
	break;
   case 33:
   type_=SKINMESHARRAY;
	break;
   case 34:
    type_=SKINVERTEXARRAY;
	break;
   }
  }

  return type_;
}

/* End GXLABEL.CPP **************************************************/
