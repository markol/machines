/*
 * E N T Y I M P L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dEntityImpl

    Data implementation for W4dEntity
*/

#ifndef _WORLD4D_ENTYIMPL_HPP
#define _WORLD4D_ENTYIMPL_HPP

#include "base/base.hpp"
#include "base/tracker.hpp"
#include "base/persist.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/t3dkey.hpp"
#include "mathex/double.hpp"
#include "phys/phys.hpp"
#include "render/fwmatmap.hpp"
#include "render/render.hpp"
#include "world4d/world4d.hpp"
#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"
#include "world4d/internal/animdata.hpp"
#include "world4d/entity.hpp"
#include "stdlib/string.hpp"

class W4dEntity;
class RenColour;
class W4dLightData;

class W4dEntityImpl
// Canonical form revoked
{
public:
    ~W4dEntityImpl( void );

	// These methods are public, available to entities and composites.
    bool hasAnimationData() const;
	void applyAllAnimations( W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const;
	void applyLightAnimations( W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const;
	void applyNonLightAnimations( W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const;

    bool hasParent( void ) const;
    // True iff this object has a parent

    W4dEntity* pParent() const;
    // Returns pointer to parent entity
    // PRE( hasParent() );
    // POST( result != NULL );

    //Get/set the render pass id;
    void passId( ulong id );
    ulong passId( void ) const;

	bool hasMesh( void ) const;

	void doNotLight(bool lightIt);
	bool doNotLight() const;

	// Any non-global lights which illuminate this object.
	bool hasLightList() const;
	W4dLocalLightList& lightListForEdit();
	void lightList(W4dLocalLightList* pList);
	const W4dLocalLightList& lightList() const;

	// If this value is true, then the system turns off lighting for this
	// object.  For emissive objects the value probably ought to be true.  If
	// the object isn't emissive it will be rendered black.  It's the client's
	// responsibility to set this correctly.  The render library tries to guess
	// correct values by looking at an entity's materials during loading models.
	// However, it doesn't take material plans etc. into account, so the client
	// is ultimately responsible for getting this value correct.
	//void doNotLight(bool lightIt);
	//bool doNotLight() const;

    //Set the isComposite flag
    void isComposite( bool isIt );
    //true iff this is a composite
    bool isComposite() const;

    const W4dEntity::W4dEntities& children() const;
    // Exports list of child entities
    //Persistence

	// Generic data that can be used by the client for any purpose.
	void clientData( int );
	int clientData() const;

    PER_MEMBER_PERSISTENT( W4dEntityImpl );
    PER_FRIEND_READ_WRITE( W4dEntityImpl );

private:
    /////////////////////////////////////////////////////////////////
    //types
	struct MeshData
	{
        MeshData(): mesh(NULL) {}
        ~MeshData() {}

		RenMeshInstance*	mesh;
		W4dEntity::W4dDistance	distance;

        PER_MEMBER_PERSISTENT_DEFAULT_INLINE( MeshData );
        PER_FRIEND_READ_WRITE( MeshData );
	};
	PER_FRIEND_READ_WRITE( MeshData );

	typedef ctl_vector< MeshData >          Meshes;

    //Used to cache the last time at which entity attributes have been
    //updated from the entity plans
    struct PlanUpdateTimes
    {
        PhysAbsoluteTime transformTime;
        PhysAbsoluteTime visibilityTime;
        PhysAbsoluteTime scaleTime;
        PhysAbsoluteTime uvTime;
        PhysAbsoluteTime materialTime;

        PER_MEMBER_PERSISTENT_DEFAULT_INLINE( PlanUpdateTimes );
        PER_FRIEND_READ_WRITE( PlanUpdateTimes );
    };
    PER_FRIEND_READ_WRITE( PlanUpdateTimes );
    /////////////////////////////////////////////////////////////////

	friend class W4dEntity;

    W4dEntityImpl( W4dEntity* pParent, const W4dTransform3d& localTransform, W4dEntity::Solidity solid );
    W4dEntityImpl( const W4dEntityImpl& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform );
	W4dEntityImpl( void );
	W4dEntityImpl( W4dEntity* pParent, const W4dTransform3d& newLocalTransform, W4dEntity::Solidity, W4dEntity::NotAChild);

	void name(const string&);
	const string& name() const;

	typedef	CtlCountedPtr< W4dAnimationData > AnimationDataPtr;
	typedef ctl_vector< AnimationDataPtr > AnimationDataPtrs;

	void addAnimationData( W4dAnimationData* animData );
	void addLightAnimationData( W4dLightData* animData );

	void filterColour( const RenColour& colour );
	const RenColour& filterColour() const;
	bool filterColourIsSet() const;

	void updateHasMeshFlag();

    static const string& dummyName();

    void CLASS_INVARIANT;

    W4dEntityImpl& operator =( const W4dEntityImpl& );

	//data members
	W4dTransform3d			localTransform_;
	W4dTransform3d			globalTransform_;
    W4dEntity*          	pParent_;
    W4dEntity::W4dEntities*	childList_;
    Meshes*          		meshes_;

    W4dEntity::W4dDomains*	intersectingDomains_;

    W4dComposite*       pHolder_;    //Any composite holding this object

    W4dEntityScale*     pEntityScale_; //The object holding all scale info
	W4dEntityPlan*      pPlan_;// Pointer to a current W4dEntityPlan which may modify the
                               // entity's local transform, mesh display etc
    PlanUpdateTimes* pPlanUpdateTimes_; //Cache store for times plans last updated by attribute.
                                        //Exists if and only if pPlan_ defined.

    W4dSubject* pSubject_; //Optional logical owner of this entity

	// A number which uniquely identifies the last camera traversal to draw this entity.
	ulong				passId_;

    //The time when the cached global transform was last updated
    PhysAbsoluteTime timeGlobalTransformLastUpdated_;

    //Cached transform keys for the parent's global and local transform last
    //used to update the cached global transform
    MexTransform3dKey lastParentGlobalTransformKey_;
    MexTransform3dKey lastLocalTransformKey_;

    //An id used for various purposes in checking algorithms
    ulong checkId_;

    //This entity's id
    W4dId id_;

    //  A very approximate bounding volume of this ebtities meshes - it does not
    //  include the children of this entity.
    MexAlignedBox3d* boundingVolume_;

	// Warning: these variable are bitfields, thus it is not possible
	// to reference them using CB_DEPIMPL
	// W4dEntity has to access them using the indirection operator pImpl_->
    W4dEntity::Solidity solid_ : 2;
    bool                isDomain_ : 1;
    bool                ownedByParent_ : 1;
    bool            visible_ : 1;
    bool isStationary_ : 1; //True if the entity is considered as stationary.
                            //False implies likely to move.

    bool notScaled_ : 1; //True iff the lod 0 mesh has unity scale,
                         //and the entity has unity scale override.
    bool isComposite_ : 1; //True if this is a composite
    bool isGarbage_ : 1; //True if added to the garbage collection list
    bool isOwnedByCountedPtr_ : 1; //True if a counted ptr references the entity.
                                   //In this case it must only be deleted when the
                                   //last counted ptr goes out of scope - NOT as a child
                                   //of a deleting parent, or via garbage collection.
                                   //Set when referenced by a W4dSubject

	bool hasMesh_ : 1;				// True if the entity has at least one mesh

	//This entity's local light list
	W4dLocalLightList*	pLocalLightList_;

	string*				name_;
	AnimationDataPtrs*  pAnimationDataPtrs_;
	AnimationDataPtrs*  pAnimationLightDataPtrs_;
	RenColour*		    pFilterColour_;
    int                 nLinkedSounds_; //Count of sounds associated via W4dSoundManager
	bool				doNotLight_;
	int					clientData_;  // To be used as clients of this class see fit.

    OBJECT_TRACKER( W4dEntityImpl );

    /////////////////////////////////////////////////////////////////
};

PER_DECLARE_PERSISTENT( W4dEntityImpl );

#ifdef _INLINE
#include "internal\entityi.ipp"
#endif

#endif

/* End ENTYDATA.HPP *************************************************/
