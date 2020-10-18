/*
 * G X H I E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxhier.hpp"

GXIdPos GXHier::zeroId_;

GXHier::GXHier():
nChildren_(0)
{
	// Set the default transform matrix to identity
	// (no transformation supplied)
	transform_.xx(1.0);
	transform_.yy(1.0);
	transform_.zz(1.0);
    TEST_INVARIANT;
}

GXHier::~GXHier()
{
    TEST_INVARIANT;

	for (int i=0; i<children_.size(); ++i)
	{
	   _DELETE(children_[i]);
	}
}

void GXHier::CLASS_INVARIANT
{
	INVARIANT(nChildren()>=nChildrenSupplied());
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXHier& t )
{

    o << std::endl;
    o << "meshId_ {" << t.meshId_ << "}" << std::endl;
	o << "meshName_ " << t.meshName_ << std::endl;
	o << "parentId_ {" << t.parentId_ << "}" << std::endl;
	o << "transform_ {" << t.transform_ << "}" << std::endl;
	o << "translate_ {" << t.translate_ << "}" << std::endl;
	o << "nChildren_ " << t.nChildren_ << std::endl;
	o << "nChildrenSupplied() " << t.children_.size() << std::endl;
	for (int i=0; i<t.children_.size(); i++) {
	  o << "child_(" << i << ") {" << t.child(i) << "}" << std::endl;
	}

    return o;
}

void GXHier::nChildren(long newNChildren)
{
  TEST_INVARIANT;
  PRE(nChildrenSupplied()==0);

  nChildren_=newNChildren;
  children_.reserve(newNChildren);

  TEST_INVARIANT;
}

void GXHier::createChildren(long nbChildren)
{


  TEST_INVARIANT;
  PRE(nChildrenSupplied()==0);

  //reserve
  nChildren(nbChildren);

  for (int i=0; i<nbChildren; ++i)
  {
	 addChild(_NEW(GXHier));
  }

  POST(nChildrenSupplied()==nbChildren);
  POST(allChildrenSupplied());
  TEST_INVARIANT;
}


// Note: can not add more children than reserved by nChildren()
// if not TEST_INVARIANT will fail
void GXHier::addChild(GXHier* newChild)
{
  TEST_INVARIANT;
  PRE(newChild);

  PRE_DATA(int oldNChildrenSupplied=nChildrenSupplied());
  children_.push_back(newChild);

  POST(nChildrenSupplied()==(oldNChildrenSupplied+1));
  TEST_INVARIANT;
}

GXName GXHier::getMeshName(const GXIdPos& searchedMeshId, bool& nameFound)
{
	PRE_DATA(GXIdPos zero);
	PRE(!(searchedMeshId==zero));

    GXName result;
    GXHier*pMeshNode=getPointerNode(searchedMeshId);

    if (pMeshNode==NULL)
    {
      nameFound=false;
    }
    else
    {
      nameFound=true;
   	  result=pMeshNode->meshName();
    }

    POST(iff(nameFound,result.length()!=0));
    TEST_INVARIANT;

   return result;
}

// I hate recursive functions...
//  so don't yell if this one is not pretty!
GXHier* GXHier::getPointerNode(const GXIdPos& searchedId)
{

  PRE_DATA(GXIdPos zero);
  PRE(!(searchedId==zero));

  GXHier *result=NULL;
  int cIndex;

  TEST_INVARIANT;

  if (meshId_==searchedId)
  {
	 result=this;
  }
  else
  {
	cIndex=0;
	while ((result==NULL)&&(cIndex<nChildrenSupplied()))
	{
	  result=child(cIndex).getPointerNode(searchedId);
	  cIndex++;
	}
  }


  POST((result==NULL)||(result->meshId()==searchedId));
  TEST_INVARIANT;

  return result;

}

/* End GXHIER.CPP ***************************************************/
