/*
 * G X T E X T U R E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxtexture.hpp"

GXTexture::GXTexture():
transparancy_(0),
isPaletted_(false),
palette_(0)
{

    TEST_INVARIANT;
}

GXTexture::~GXTexture()
{
    TEST_INVARIANT;

}

GXTexture::GXTexture( const GXTexture& copy):
name_(copy.name_),
transparancy_(copy.transparancy_),
isPaletted_(copy.isPaletted_),
palette_(copy.palette_)
{

    TEST_INVARIANT;
}

GXTexture& GXTexture::operator=(const GXTexture& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    name_=copy.name_;
	transparancy_=copy.transparancy_;
	isPaletted_=copy.isPaletted_;
	palette_=copy.palette_;
  }
  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

void GXTexture::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool operator==(const GXTexture& tex1, const GXTexture& tex2)
{
  bool result=false;

  if (
	  (tex1.name_==tex2.name_) &&
	  (tex1.transparancy_==tex2.transparancy_) &&
	  (tex1.isPaletted_==tex2.isPaletted_) &&
	  (tex1.palette_==tex2.palette_)
	 )
	  result=true;

  return result;
}

bool operator < (const GXTexture& tex1, const GXTexture& tex2)
{
  bool result=false;
  // TODO:If necessary, should complete this method so as
  // to compare other informations than names
  if (tex1.name_<tex2.name_)
    result =true;

  return result;
}

ostream& operator <<( ostream& o, const GXTexture& t )
{

    o << "name_ " << t.name_;

    return o;
}

/* End GXTEXTURE.CPP ************************************************/
