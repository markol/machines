/*
 * G X H I E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXHier

    A brief description of the class should go in here
*/

#ifndef _GXHIER_HPP
#define _GXHIER_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"

#include "gxin/gxidpos.hpp"
#include "gxin/gxname.hpp"
#include "gxin/gxmatrix3.hpp"

class GXHier
// Canonical form revoked
{
public:
    GXHier();
    ~GXHier();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXHier& t );

    void createChildren( long );
	// PRE(nChildrenSupplied==0);
	// POST(allChildrenSupplied());

	GXName getMeshName(const GXIdPos&, bool&);
	// PRE(not searchedId==zeroId_);

	// The node has an object (namely a mesh) attached to it if the meshId
	// is not null
	bool hasObject() const { return not (meshId_==zeroId_); }

	void meshId(const GXIdPos& newMeshId) { meshId_=newMeshId; }
	const GXIdPos& meshId() const { return meshId_; }

	void parentId(const GXIdPos& newParentId) { parentId_=newParentId; }
	const GXIdPos& ParentId() const { return parentId_; }

	void meshName(const GXName & newMeshName) { meshName_=newMeshName; }
    const GXName& meshName(void) const { return meshName_; }

	void transform(const GXMatrix3& newTransform) { transform_=newTransform; }
	const GXMatrix3& transform() const { return transform_; }

	void translate(const GXVect3& newTranslate) { translate_=newTranslate; }
	const GXVect3& translate() const { return translate_; }

	long nChildrenSupplied(void) const { return children_.size(); }

	long nChildren(void) const { return nChildren_; }

	// Set the total number of children
	// and call the reserve method for children_
	// can be called only if no child has been 
	// provided yet
	void nChildren(long newNChildren);
	// PRE(nChildrenSupplied()==0);

	bool allChildrenSupplied(void) const {
	  return nChildren()==nChildrenSupplied();
	}

    GXHier& child(int i) {
	  PRE(i<nChildrenSupplied());
	  PRE(children_[i]!=NULL);
	  return *(children_[i]);
	}
	const GXHier& child(int i) const {
	  return *(children_[i]);
	}
private:
    GXHier( const GXHier& );
    GXHier& operator =( const GXHier& );
    bool operator ==( const GXHier& );

	void addChild(GXHier* );
    // PRE(newChild);
	// POST(nChildrenSupplied()==old(nChildrenSupplied())+1);

	// Get, in the hierarchy structure, the node corresponding having the meshid
	GXHier* getPointerNode(const GXIdPos& meshId);
    // POST((result==NULL)||(result->meshId()==searchedId));

	static GXIdPos zeroId_;

	GXIdPos meshId_;
	GXIdPos parentId_;
	GXName meshName_;
	GXMatrix3 transform_;
	GXVect3 translate_;
	long nChildren_;
	ctl_vector<GXHier*> children_;
};


#endif

/* End GXHIER.HPP ***************************************************/
