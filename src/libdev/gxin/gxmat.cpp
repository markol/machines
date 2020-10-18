/*
 * G X M A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxmat.hpp"

GXMat::GXMat():
hasOneColor_(false),
transparancy_(0),
hasValidTexture_(false)
{

    TEST_INVARIANT;
}

GXMat::~GXMat()
{
    TEST_INVARIANT;

}

GXMat::GXMat(const GXMat& copy):
hasValidTexture_(copy.hasValidTexture_),
textureId_(copy.textureId_),
transparancy_(copy.transparancy_),
hasOneColor_(copy.hasOneColor_),
diffuseColor_(copy.diffuseColor_)
{
  POST(*this==copy);
  TEST_INVARIANT;
}

GXMat& GXMat::operator=(const GXMat& copy)
{
  TEST_INVARIANT;
  if (this !=&copy)
  {
	hasValidTexture_=copy.hasValidTexture_;
    textureId_=copy.textureId_;
    transparancy_=copy.transparancy_;
	hasOneColor_=copy.hasOneColor_;
	diffuseColor_=copy.diffuseColor_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;

}

bool operator<(const GXMat& mat1, const GXMat& mat2)
{
  bool result=false;

  // compare colors
  if (mat1.diffuseColor_<mat2.diffuseColor_)
  {
    result=true;
  }
  else if ((mat1.diffuseColor_==mat2.diffuseColor_) &&
       (mat1.transparancy_<mat2.transparancy_))
  {
    result=true;
  }
  else if ((mat1.diffuseColor_==mat2.diffuseColor_) &&
       (mat1.transparancy_==mat2.transparancy_) &&
	   (mat1.textureId_<mat2.textureId_))
  {
    result=true;
  }

  return result;
}

bool operator==(const GXMat& mat1, const GXMat& mat2)
{
  bool result=false;

  if ((mat1.hasOneColor_==mat2.hasOneColor_) &&
	  (mat1.diffuseColor_==mat2.diffuseColor_) &&
	  (mat1.transparancy_==mat2.transparancy_) &&
	  (mat1.textureId_==mat2.textureId_))
	  result=true;

  return result;
}

void GXMat::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXMat& t )
{
	o << std::endl;
    o << "transparancy " << t.transparancy_ << std::endl;
    o << "hasOneColor_ " << t.hasOneColor_ << std::endl;
    if (t.hasOneColor_)
      o << "diffuseColor_ " << t.diffuseColor_ << std::endl;
    o << "hasValidTexture_ " << t.hasValidTexture_ << std::endl;
    o << "textureId_ " << t.textureId_ << std::endl;
    return o;
}

/* End GXMAT.CPP ****************************************************/
