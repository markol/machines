/*
 * M A T M G R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MATMGR_HPP
#define _RENDER_MATMGR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

template <class T>
class ctl_vector;
class RenIMatBody;
class RenColour;
class RenDevice;
class RenMaterialTransform;
class RenMaterial;

// Maintains a list of all materials in existence and ensures that duplicate
// materials are not created.
class RenIMatManager
{
public:
    //  Singleton class
    static RenIMatManager& instance( void );
    ~RenIMatManager( void );

	// Update the Direct3D parts of all materials in the system.
	void forceUpdateAll();

	// This forces all materials to obtain handles for the given device.
	void useDevice(const RenDevice&);

    //Delete the unused material bodies
    void clearUnusedMatBodyList();

	// This firstly keeps track of the maximum and minimum coplanar values.
	// It also asserts out if they differ by more than 16.
	void checkCoplanarValue(short coplanarPriority);
	short minCoplanarValue() const		{ return 14; }
	short maxCoplanarValue() const		{ return 30; }

	const RenMaterialTransform* globalMaterialTransform() const;
	void globalMaterialTransform(const RenMaterialTransform*);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenIMatManager& t );

	typedef ctl_vector<RenIMatBody*> Bodies;

private:
	// Internal methods used by RenMaterial.
	friend class RenMaterial;
    friend void perRead( PerIstream&, RenMaterial& );

	RenIMatBody* defaultBody();
	RenIMatBody* createBody(const RenColour& dif);

	// The material class has created what may be a new body.  Check to see if
	// it is unique and return either the given body or an identical copy.
	RenIMatBody* addPossiblyNewBody(RenIMatBody* body);

	// PRE(body) POST(ret != body) POST(!ret || *ret == *body);
	// PRE(body->sharable());
	RenIMatBody* findDuplicate(const RenIMatBody* body) const;

    //Add an unused body to a collection to be deleted on the next render frame.
    //These are typically bodies read in via persistence for which a duplicate already exists.
    void addUnusedMatBody( RenIMatBody* pBody );
    //PRE( pBody->sharable() );

	// Internal methods used by RenIMatBody.
	friend class RenIMatBody;
	void killMe(RenIMatBody*);

	// Internal truly private stuff.
	void addBody(RenIMatBody*);

	Bodies*				bodies_;
	RenIMatBody*		defaultBody_;
	ulong				nullSlots_;
	Bodies*				unusedBodies_; //Temporary cache of unused bodies
	short				minPriority_, maxPriority_;

	// If this is set, it will be applied to all materials before rendering.
	const RenMaterialTransform*		globalMaterialXform_;

    // Operations deliberately revoked
    RenIMatManager();
    RenIMatManager( const RenIMatManager& );
    RenIMatManager& operator =( const RenIMatManager& );
    bool operator ==( const RenIMatManager& );
};

#endif

/* End MATMGR.HPP ***************************************************/
