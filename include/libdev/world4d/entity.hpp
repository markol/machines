/*
 * E N T I T Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dEntity

    The base class for all entities in the render tree.
*/

#ifndef _REN_ENTITY_HPP
#define _REN_ENTITY_HPP

#include "base/base.hpp"
#include "base/tracker.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "render/fwmatmap.hpp"
#include "world4d/world4d.hpp"
#include "world4d/fwrdplan.hpp"
//#include <strfwd.hpp>
#include "stdlib/strfwd.hpp"

template <class T> class ctl_vector;
template <class T> class ctl_list;

class MexAlignedBox3d;
class MexLine3d;
class MexDouble;

class RenMeshInstance;
class RenScale;
class RenNonUniformScale;

class W4dRoot;
class W4dDomain;
class W4dCamera;
class W4dComposite;
class W4dLink;
class W4dPortal;
class W4dEntityPlan;
class W4dSubject;
class SysPathName;
class UtlLineTokeniser;
class W4dLocalLightList;
class W4dEntityScale;
class RenColour;
class W4dComplexity;

class W4dEntityImpl;

class W4dEntity
{
public:
    virtual ~W4dEntity();
    // Destructor also destroys all children and removes from parent's child list

    void attachTo( W4dEntity* pNewParent, const W4dTransform3d& newLocalTransform );
    // Changes parent to pNewParent, setting local transform to newLocalTransform. Updates old and
    // new parents' child lists. Cancels any previous W4dComposite holding relationship.
    // PRE( pNewParent != NULL );

    void attachTo( W4dEntity* pParent );
    // Changes parent to newParent. Updates old and new parents' child lists.
    // Sets local transform such that global position is unchanged.
    // Cancels any previous W4dComposite holding relationship.
    // PRE( newParent != NULL );

    bool hasParent( void ) const;
    // True iff this object has a parent

    W4dEntity* pParent() const;
    // Returns pointer to parent entity
    // PRE( hasParent() );
    // POST( result != NULL );

    typedef ctl_list< W4dEntity* >  W4dEntities;

    const W4dEntities& children() const;
    // Exports list of child entities

    // Returns reference to current local transform. This is the transform of 'this'
    // relative to its parent in the rendering tree.
    const W4dTransform3d& localTransform() const;

    // Updates local transform to newLocalTransform.
    // PRE( hasParent() );  // Cannot set the transform for the root entity
    void localTransform( const W4dTransform3d& newLocalTransform );

    const W4dTransform3d& globalTransform() const;
    // Returns reference to cached global transform. Ensures up-to-date at time of call.

    void globalTransform( const W4dTransform3d& newGlobalTransform );
    // Updates local transform to achieve global position defined by newGlobalTransform

    void relativeTransform( const W4dEntity& other, W4dTransform3d* pTransform ) const;
    // Returns position of 'this' relative to other in t

    void stationary( bool newFlag );
    bool stationary() const;
    // sets/returns the stationary flag

    enum Solidity { SOLID, NOT_SOLID };

    Solidity solid() const;
    // returns the solid flag

    //Set the solidity flag
    void solid( Solidity newSolidity );

    void intersects( W4dDomain* pDomain, bool intersects );
    // Establishes or removes an intersects relation with domain. This is only for use in
    // pathological cases. Normally the intersects relationships will be calculated automatically
    // by the rendering library.

    bool intersects( const W4dDomain& domain ) const;
    // returns true if an intersect relation exists between 'this' and domain

    //  This should be replaced by a map when we have a working map inplementation
    typedef ctl_list< W4dDomain* >  W4dDomains;

    const W4dDomains& intersectingDomains() const;
    // Exports a list of the domains having an intersect relation with 'this'

        // Establish an intersects relation with every domain in the given list.
        // Any existing intersecting domains are *replaced* by the new ones.
    void intersectingDomains(const W4dDomains&);

        bool hasIntersectingDomains( void ) const;
        void createIntersectingDomains( void );
        void eraseIntersectingDomains( void );
        void deleteIntersectingDomains( void );
        void addIntersectingDomain( W4dDomain* );

    W4dDomain* containingDomain( void ) const;
    // Returns the lowest domain in the tree that owns 'this'.
    // PRE( hasParent() );
    // POST( result != NULL );

    // True if this entity is attached to a domain. In this case returns the domain
    // via ppDomain (if ppDomain is NULL, this argument is just ignored).
    bool hasContainingDomain( W4dDomain** ppDomain=NULL ) const;

    bool isDomain( void ) const;
    // Returns the value stored in the domain flag

    W4dDomain* getAsDomain( void );
    // PRE( isDomain() );
    // POST( result != NULL );

    //true iff this is a composite
    bool isComposite() const;

    //Get as a composite
    W4dComposite& asComposite();
    const W4dComposite& asComposite() const;
    //PRE( isComposite );

    //This entity's id. Zero by default.
    W4dId id() const;

    //Set the id associated with this entity
    void id( W4dId id );

        // This number uniquely identifies the last camera that drew this entity.
        // It is used internally to avoid rendering the same entity twice.  It is
        // also needed externally for domain culling.
        ulong latestRenderPassId() const;

    //Generates a new globally unique checkid
    static ulong nextCheckId();

    //This entity's stored check id
    ulong checkId() const;

    //Updates the stored checkid
    void checkId( ulong id );

    //  Add the materials used by this entity (all LODs) to the given set
    virtual void    addMaterials( RenMaterialSet* pSet ) const;

    //  Remap the materials that are being used
    virtual void    materialMap( const RenMaterialMap& map );

        W4dRoot*       findRoot();
        const W4dRoot* findRoot() const;
        // Proceed up the tree from this entity until we reach it's ultimate parent.
        // Useful for debugging, i.e. asserting which tree something belongs to.

    //Clear all current plans attached to this entity
    void clearPlan( void );

        //The entity plan.
        const W4dEntityPlan& entityPlan() const;
    //PRE( hasPlan() )

        //Ensures has a W4dEntityPlan and returns it.
    //Also causes the entity to be updated for any existing plans. Hence this
    //method must only be called if the intention is to edit the W4dEntityPlan
    //by adding new plans. Otherwise efficiency could be severely affected.
    //NB using this method causes a W4dEntityPlan to be instantiated if one
    //does not already exist, so only use them if this is the desired behaviour.
        W4dEntityPlan& entityPlanForEdit();

    /////////////////////////////////////////////////////////////////
    //Scaling

    //Flag used to indicate whether scaling should be propogated through the
    //descendants of this entity.
    enum PropogateScaleFlag { PROPOGATE = 1, NOT_PROPOGATE = 0 };

    //Set the temporary scale override to scale. This supercedes and previous temporary
    //scale, and is not combined with such. propogate indicates whether any child
    //entities should be likewise scaled.
    //Default implementation is adequate for entities. Should only be overridden by
    //W4dComposite class.
    virtual void temporaryScale( const RenScale& scale, PropogateScaleFlag propogate );

    //Clear any temporary scale override.
    //propogate indicates whether any descendants should also be cleared.
    //Default implementation is adequate for entities. Should only be overridden by
    //W4dComposite class.
    virtual void clearTemporaryScale( PropogateScaleFlag propogate );

    //True iff has a temporary scale override for own meshes
    bool hasTemporaryScale() const;

    //The current temporary scale override for own meshes
    const RenScale& temporaryScale() const;
    //PRE( hasTemporaryScale() )

    //Apply planPtr to this entity ans all its descendants start at startTime, with
    //specified number of repetitions and animation id.
    //Default implementation is adequate for entities. Should only be overridden by
    //W4dComposite class.
    virtual void propogateScalePlan( const W4dScalePlanPtr& planPtr,
                                     const PhysAbsoluteTime& startTime,
                                     uint repetitions = 0, uint animId = 0 );
    /////////////////////////////////////////////////////////////////

        // For debugging: perhaps these should be compiled out of the release version.
        void name(const string&);
        const string& name() const;

//    const W4dD3DEntity& d3dEntity() const;

    // Replace the meshes that are curently in this W4dEntity
    // with those from the supplied W4dEntity
    void replaceMeshes( const W4dEntity& entity );

    //Remove all the meshes associated with this entity
    void clearMeshes();

    //The number of levels of detail specified.
    //NB Some LODs may have no mesh.
    uint nLODs() const;

    //True iff this entity has a mesh defined with LOD LODid
        bool hasMesh( W4dLOD LODid ) const;
        // true if the entity has a mesh at lod 0
        bool hasMesh( ) const;

    //returns the mesh for LOD LODid
    const RenMeshInstance& mesh( W4dLOD LODid = 0 ) const;
    //PRE( hasMesh( LODiD ) )

    bool visible( void ) const;
    void visible( bool vis );

    //The local mesh bounding volume (scaled if any scale applied)
    const MexAlignedBox3d& boundingVolume() const;

    //The local mesh bounding volume may be set directly to allow entities
        //such as generics to set a volume without having to come from a predefined mesh.
    void boundingVolume( const MexAlignedBox3d& );

    //Ensures the bounding volume is up-to-date
    void updateBoundingVolume();

    //Parameter used to define accuracy of various checks
    enum Accuracy { HIGH, MEDIUM, LOW };

    //True if this entity intersects line.
    //If so returns the distance from line.end1() to the intersection point in pDistance.
    //This method is virtual so that the derived class can implement variant behaviour,
    //using, for example, known special features of the geometry of the object.
    //The accuracy of the test is indicated by accuracy.
    //Appropriate default behaviour might be to call defaultIntersects().
    //NB could not call this method "intersects" because of overload conflict in subclasses
    //with the intersects( domain ) method.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const = 0;

    //Appropriate default behaviour for the intersects method.
    //LOW and MEDIUM accuracy simply check against the boundingVolume().
    //HIGH accuracy would then check the mesh on a hit (not implemented yet).
    bool defaultIntersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                Accuracy accuracy ) const;

    //True if this entity's bounding volume intersects line.
    //If so returns the distance from line.end1() to the intersection point in pDistance.
    bool intersectsBoundingVolume( const MexLine3d& line, MATHEX_SCALAR* pDistance ) const;

    ////////////////////////////////////////////////

    //True iff has an associated W4dSubject
        bool hasSubject() const;

    //The associated subject
        W4dSubject& subject();
        const W4dSubject& subject() const;
    //PRE( hasSubject() )

    ////////////////////////////////////////////////

    void debugOutput( ostream& ostr );
    // Write out the details of 'this' and all its children for debugging
    // purposes.

    void debugOutputChildList( ostream& ostr ) const;
    // Write out the childlist for debugging purposes

    void CLASS_INVARIANT;

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
        void doNotLight(bool lightIt);
        bool doNotLight() const;

        void filterColour( const RenColour& colour );
        const RenColour& filterColour() const;
        bool filterColourIsSet() const;

        // Generic data that can be used by the client for any purpose.
        void clientData( int );
        int clientData() const;
        typedef MexDouble  W4dDistance;
        enum NotAChild { DONT_ADD_TO_CHILD_LIST };

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dEntity );
    PER_FRIEND_READ_WRITE( W4dEntity );

protected:
    // Constructor attaches to parent, adds this to the parent's list of children,
    // stores copy of localTransform, sets solid flag.
    // Initialises child list to empty, activity priority to 0, and static flag to true.
    // PRE( parent != NULL );
    W4dEntity( W4dEntity* pParent, const W4dTransform3d& localTransform, Solidity solid );

    //  Make a copy of an entity.
    //  None of the entity's children will be copied
    //  Any motion plans owned by the entity will not be copied
    W4dEntity( const W4dEntity& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform );

    // Write out the details of 'this' for debugging purposes.
    virtual void doDebugOutput( ostream& ) {};

    // Read in a level of detail file and add all of the
    // referenced meshes to this.
    void readLODFile( const SysPathName& pathname );
    // PRE( pathname.existsAsFile() );

    // Adds mesh to the entity, linked to level of detail code LODid.
        // Beware: could violate the LSP for W4dComposites!
        // By convention 0 is the highest LOD.
    // Distance is the transition distance between this mesh and the
    // one with the next lower LOD (i.e. a higher W4dLOD number)
    //typedef MexDouble  W4dDistance;
    void add(RenMeshInstance* pMeshInstance, const W4dDistance& distance, W4dLOD id);

    // Set the transition distance for a mesh that has already been
    // added to this entity
    void setDistance( const W4dDistance& distance, W4dLOD id);
    // PRE( id < meshes_.size() );

    // Adds mesh to the entity, linked to level of detail code LODid.
        // Beware: could violate the LSP for W4dComposites!
        // By convention 0 is the highest LOD.
    void add( RenMeshInstance* pMeshInstance, W4dLOD LODid = 0 );

        // Returns the maximum range at which any mesh is visible for the meshes
        // which are directly owned by this entity.  It *does* account for any
        // children's meshes.  Beyond this range, the entity is invisble.  If the
        // entity always has a visible mesh, then infinity is returned.
        // POST(retval >= 0);
        W4dDistance maxRange() const;

    //True iff any plans are defined for this entity
    bool hasPlan() const;

    enum RenderType { RENDER_CHILDREN, DO_NOT_RENDER_CHILDREN };

    // Renders the entity. The complexity value is an aid to selecting the LOD to use in
    // rendering. It represents the distance from eye-point, field of view, and may be modified
    // to reflect current frame rate. The override perfroms some mapping from this value
    // to desired LOD id, depending on the number of variant LOD meshes that were added,
    // and makes calls to ::render (const W4dCamera& camera, W4dLOD LODid) accordingly. The
    // default implementation of this method simply calls the above function with LODid 0.
    // Returns a value indicating if children should be rendered.
    // The options are as specified in the W4dCamera::render methods.
    virtual RenderType render( const W4dCamera&, W4dComplexity& );

        W4dEntityImpl& entityImpl();
        const W4dEntityImpl& entityImpl() const;
        // PRE(pImpl_!=NULL);

private:
        friend class W4dCamera;
    friend class W4dMeshPlan;
    friend class W4dDomain;
    friend class W4dRoot;
    friend class W4dLink;
    friend class W4dPortal;
        friend class W4dSubjectImpl;
    friend class W4dComposite;
    friend class W4dCompositeImpl;
    friend class W4dGarbageCollector;
    friend class W4dEntityMaterialVecChanger;

    // Render the entity as though it were the given range away from the camera i.e. use
    // the appropriate level of detail for that range
    virtual void renderAtRange( const W4dCamera& camera, const W4dDistance& range, const W4dComplexity& complexity );

    //returns the mesh for LOD LODid
    RenMeshInstance& mesh( W4dLOD LODid = 0 );
    //PRE( hasMesh( LODiD ) )

//     // Set the camera flag.
//     // This call only for use by W4dCamera::W4dCamera
//     void isCamera( bool newCam );

    // Set the domain flag.
    // This call only for use by W4dDomain::W4dDomain
    void isDomain( bool newFlag );

    // Initialises child list to empty, activity priority to 0, and static flag to true.
    // This call only for use by W4dRoot::W4dRoot
    W4dEntity( void );

    //enum NotAChild { DONT_ADD_TO_CHILD_LIST };
    // Constructor attaches to parent, DOES NOT add this to the parent's list of children,
    // stores copy of localTransform, sets solid flag.
    // Initialises child list to empty, activity priority to 0, and static flag to true.
    // This is used by W4dEntities that do not want to be part of the standard
    // rendering tree.
    // PRE( parent != NULL );
    // This call only for use by W4dLink::W4dLink and W4dPortal::W4dPortal
    W4dEntity( W4dEntity* pParent, const W4dTransform3d& newLocalTransform, Solidity solid, NotAChild );

    //Ensures an entity plan is attached and returns it
    W4dEntityPlan& plan();

        //Updates the entity's local transform from pPlan_ if defined, and clears any
        //motion plans now complete. Returns true if anything updated.
        bool updateTransformFromPlan();

    //Updates the material overrides for all levels of detail at timeNow, except
    //the detail for lodId.
    //Returns the number of obsolete material plans encountered.
    uint updateLODMaterials( const PhysAbsoluteTime timeNow, W4dLOD exceptLodId );
    //PRE( pPlan_ != NULL and pPlan_->hasMaterialPlan() )

    //Update the uv transform override to uvTransformPtr for all meshes up to an including
    //level of detail maxLOD, except for exceptId.
    void updateLODUVTransforms( const Ren::UVTransformPtr& uvTransformPtr,
                                W4dLOD maxLOD, W4dLOD exceptId );

private:
    /////////////////////////////////////////////////////////////////

    //Set the associated W4dSubject - must only be called by the W4dSubject
        void subject( W4dSubject* theSubject );

    /////////////////////////////////////////////////////////////////

    void removeChild( const W4dEntity* pChild );
        int  computeDescendantCount() const;

    //Ensure this entity has a child list, allocating one if none already exists.
    //Only call this if you want to add a child.
    W4dEntities* childListForEdit();

    bool isHeld( void ) const;
    // true iff this object is being held by a composite object

    W4dComposite* pHolder( void ) const;
    // PRE( isHeld() );
    // POST( result != NULL );

    //Cancel any holding relationship
    void cancelIsHeld();

    //This object is being held by pComposite
    void beHeldBy( W4dComposite* pComposite );

    //Set the isComposite flag
    void isComposite( bool isIt );

        //deletes the owned W4dEntityPlan if all its plans are complete, returning true
        //if actually deletes it.
        bool clearPlanIfDone( void );

    void parseMesh( const SysPathName& directoryname, UtlLineTokeniser* pParser, W4dLOD id );

        void parseAnimationData( UtlLineTokeniser* pParser );

    void parseCycleTexture( UtlLineTokeniser* pParser );
    void parseUVTranslate( UtlLineTokeniser* pParser );
    void parseColourPulse( UtlLineTokeniser* pParser );
    void parseGenericLight( UtlLineTokeniser* pParser );

    //  Convert a range into a pointer to the mesh to be drawn at that range.
    //  If the return value is false, there is no mesh to be drawn at that range.
    //The level of detail id of the selected mesh is returned in pLodId.
    bool rangeToMesh( const W4dDistance& range, RenMeshInstance** ppMesh,
                                  W4dLOD* pLodId, const W4dComplexity& complexity ) const;

    //Get/set the render pass id;
    void passId( ulong id );
    ulong passId( void ) const;

    //Mark the global transform of this entity and all its descendants out of date.
    //Also works for composites and their links.
    void markGlobalTransformsDirty();

    /////////////////////////////////////////////////////////////////
    //Scaling

    //Update notScaled_ according to existence of scaling on highest lod mesh
    //and local scale attribute
    void updateNotScaledFlag();

    //The W4dEntityScale ownded by this. Constructs one if doesn't exist.
    W4dEntityScale& entityScale();

    //Recursively apply a non-uniform transform scale to this entity and its
    //descendants. The transform from the ancestor at which scale is applied
    //to this entity is useTransform. This transform is used to compose a
    //modified non-uniform scale.
    virtual void temporaryScale( const RenNonUniformScale& scale,
                                 const MexTransform3d& useTransform );

    //Set/clear scaling of local transform position using parent's temporary scale override
    void scaleTransformFromParent( bool doScale );

    //Return above flag
    bool scaleTransformFromParent() const;

    //Update the local transform position by scaling the unscaled position (saved in
    //the entity scale) from parent's mesh scale attribute,
    void scaleTransformPosition();

    //Recursively apply planPtr to this entity and all its descendants start at startTime, with
    //specified number of repetitions and animation id.
    //The scale plan is to be modified by the offsetTransform.
    //Default implementation is adequate for entities. Should only be overridden by
    //W4dComposite class.
    virtual void propogateScalePlan( const W4dScalePlanPtr& planPtr,
                                     const PhysAbsoluteTime& startTime,
                                     uint repetitions, uint animId,
                                     const MexTransform3d& offsetTransform );
    //PRE( planPtr->isNonUniform() )
    /////////////////////////////////////////////////////////////////

    //Used in conjunction with the garbage collector class to prevent deletion
    //of objects registered to be deleted by the garbage collector.
    void isGarbage( bool setIsGarbage );
    //PRE( not setIsGarbage  or  not isOwnedByCountedPtr() );

    bool isGarbage() const;

    //Set flag indicating owned by counted ptr system
    void nowOwnedByCountedPtr();

    //True iff owned by counted ptr system
    bool isOwnedByCountedPtr() const;

    /////////////////////////////////////////////////////////////////

    friend class W4dSoundManager; //Maintains the sound count. Interface is const because
                                  //sound manager has const pointers.
    void incrementLinkedSoundCount() const;
    void decrementLinkedSoundCount() const;

    /////////////////////////////////////////////////////////////////

    // Operation deliberately revoked
    W4dEntity( const W4dEntity& );
    const W4dEntity& operator =( const W4dEntity& );
    bool operator ==( const W4dEntity& ) const;

    /////////////////////////////////////////////////////////////////
    //Data members
        W4dEntityImpl*          pImpl_;

    OBJECT_TRACKER( W4dEntity );

    /////////////////////////////////////////////////////////////////
};

// Prints debugging info.
ostream& operator<<(ostream& s, const W4dEntity* entity);

PER_DECLARE_PERSISTENT( W4dEntity );

#pragma off(unreferenced);
//EXISTS( W4dEntity );
#pragma on(unreferenced);

#endif
