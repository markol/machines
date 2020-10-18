/*
 * G X I D P O S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxidpos.hpp"

GXIdPos::GXIdPos():
pos_(0),
gxid_(0)
{

    TEST_INVARIANT;
}

GXIdPos::~GXIdPos()
{
    TEST_INVARIANT;

}

GXIdPos::GXIdPos(const GXIdPos& copy):
pos_(copy.pos_),
gxid_(copy.gxid_)
{
  POST(*this==copy);
  TEST_INVARIANT;
}

GXIdPos& GXIdPos::operator =(const GXIdPos& copy)
{
  TEST_INVARIANT;

  if (this!=&copy)
  {
    pos_=copy.pos_;
	gxid_=copy.gxid_;
  }

  POST(*this==copy);
  TEST_INVARIANT;

  return *this;
}

bool operator==(const GXIdPos& id1, const GXIdPos& id2)
{
  bool result=true;

  if ((id1.pos_!=id2.pos_) ||
      (id1.gxid_!=id2.gxid_))
	result=false;

  return result;
}

bool operator!=(const GXIdPos& id1, const GXIdPos& id2)
{
    return !(id1 == id2);
}

bool operator<(const GXIdPos& id1, const GXIdPos& id2)
{
  bool result=false;

  if (id1.gxid_<id2.gxid_)
  {
    result=true;
  } else if ((id1.gxid_==id2.gxid_) && (id1.pos_<id2.pos_))
  {
    result=true;
  }

  return result;
}

void GXIdPos::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXIdPos& t )
{

	o << "gxid_ " << t.gxid_ << " pos_ " << t.pos_;

    return o;
}

/* End GXIDPOS.CPP **************************************************/
