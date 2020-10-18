/*
 * C O M P O S I T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    W4dComposite

    Provides support for modelling an entity which contains internal linkages
*/

#ifndef _REN_COMPOSITE_HPP
#define _REN_COMPOSITE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/fwmatmap.hpp"
#include "world4d/fwrdplan.hpp"
#include "world4d/entity.hpp"

template <class T> class ctl_vector;
template< class T > class ctl_pvector;

class SysPathName;
class MexTransform3d;
class W4dLink;
class W4dCompositePlan;
class W4dCompositeImpl;

class W4dComposite : public W4dEntity
{
public:
    //Used to store pointers to held objects
    typedef ctl_pvector< W4dEntity > HeldEntities;

    virtual ~W4dComposite( void );

    //Update this composite to use the animations defined in newPlan, with time offset
    //startTime.
    //The entire plan is to be repeated nRepetitions times.
    //The animation will asigned id animId.
    void plan( const W4dCompositePlan& newPlan, const PhysAbsoluteTime& startTime,
               uint nRepetitions = 0, uint animId = 0 );

    //Clear all motion/mesh plans etc for the composite and its links
    void clearAllPlans( void );

    /////////////////////////////////////////////////////////////////
    //Scaling

    //Set the temporary scale override to scale. This supercedes and previous temporary
    //scale, and is not combined with such. propogate indicates whether any child
    //entities should be likewise scaled.
    //Inherited from W4dEntity.
    //Default implementation is adequate for composites. Clients don't override.
    virtual void temporaryScale( const RenScale& scale, PropogateScaleFlag propogate );

    //Clear any temporary scale override.
    //propogate indicates whether any descendants should also be cleared.
    //Inherited from W4dEntity.
    //Default implementation is adequate for composites. Clients don't override.
    virtual void clearTemporaryScale( PropogateScaleFlag propogate );

    //Apply planPtr to this entity ans all its descendants start at startTime, with
    //specified number of repetitions and animation id.
    //Inherited from W4dEntity.
    //Default implementation is adequate for composites. Clients don't override.
    virtual void propogateScalePlan( const W4dScalePlanPtr& planPtr,
                                     const PhysAbsoluteTime& startTime,
                                     uint repetitions = 0, uint animId = 0 );

    /////////////////////////////////////////////////////////////////

    //Finish the plans with animation id animId. This sets the repetition count
    //so that each plan with this id will terminate after the current repetition.
    //Returns the time at which all animations will complete.
    //Does this for the composite an dall its links
    PhysAbsoluteTime finishAnimation( uint animId );

    //Clear the plans with animation id animId immediately.
    //Does this for the composite and all its links.
    void clearAnimation( uint animId );

    //  Return the equivalent link in this composite to pOriginalLink. It is assumed that
    //  pOriginalLink points to a link in a composite which is a copy of this.
    W4dLink* pEquivalentLink( const W4dLink* pOriginalLink ) const;

    //  Add the materials used by this composite to the given set
    virtual void    addMaterials( RenMaterialSet* pSet ) const;

    //  Remap the materials that are being used
    virtual void    materialMap( const RenMaterialMap& map );

    //The composite's boundary enclosing all links - only updated on request
    const MexAlignedBox3d& compositeBoundingVolume() const;

    //Updates the bounding volume including all links using current configuration
    void updateCompositeBoundingVolume();

    //Inherited from W4dEntity
    //virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
    //                         Accuracy accuracy ) const = 0;

    //Default implementation of intersects() above.
    //LOW accuracy just checks against compositeBoundingVolume().
    //MEDIUM accuracy on hit also checks composite and links using separate boundingVolume().
    //HIGH accuracy would then check against separate meshes on hit (not implemented yet).
    bool defaultCompositeIntersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                     Accuracy accuracy ) const;

    //True if the composite's bounding volume intersects the infinite line through line.
    //If so returns the distance from line.end1() to the intersection point in pDistance.
    bool intersectsCompositeBoundingVolume( const MexLine3d& line,
                                            MATHEX_SCALAR* pDistance) const;

    void CLASS_INVARIANT;

    //Moved from protected area
    bool findCompositePlan( const string& id, W4dCompositePlanPtr* pPlanPtr ) const;

    //Cycles through animation sequences
    bool cycleAnims(W4dCompositePlanPtr* pPlanPtr) const;

    //Returns a list of composite plan names
	//Not implemented yet!
    size_t listCompositePlans(ctl_list<string>* planList) const;

    //////////////////////////////////////////////////////
    //Links. WARNING these methods are public so that subclasses of W4dComposite can
    //delegate part of their implementation to nominated classes.
    //Clients must not abuse this privilege!

    //True if a link named id exists, in which case it's pointer is
    //returned in ppLink.
    bool findLink( const string& id, W4dLink** ppLink ) const;

    typedef ctl_vector< W4dLink* >    W4dLinks;

    //The collection of links
    const W4dLinks& links( void ) const;

    //returns in pTransform the transform from this composite to link
    void linkTransform( const W4dLink& link, MexTransform3d* pTransform ) const;
    //PRE( pLink is owned by this )

    //Attach object as a child of parent at position offset newLocalTransform.
    //However, the object will be rendered by the
    //default render method, even if the link is clipped for level of detail.
    //The holding relationship is cancelled automatically if object is attached
    //elsewhere.
    void hold( W4dEntity* object, W4dLink* parent, const W4dTransform3d& newLocalTransform );

    bool hasHeldEntities( void ) const;

	const HeldEntities& heldEntities( void );
	// PRE(hasHeldEntities);

    //Cancel any holding relationship with object.
    //This does not change the parent of object in any way.
    void stopHolding( W4dEntity* object );
    //////////////////////////////////////////////////////

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dComposite );
    PER_FRIEND_READ_WRITE( W4dComposite );

protected:
    W4dComposite( W4dEntity* pParent, const W4dTransform3d& localTransform, Solidity solid );
    // Constructor attaches to parent, and stores copy of localTransform, and solid flag.

    W4dComposite( const W4dComposite& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform );
    // Constructs a copy of a composite. All links are copied, however nothing
    // that the composite is holding will be copied

	enum CopyLights { COPY_LIGHTS, DONT_COPY_LIGHTS };
    W4dComposite( const W4dComposite& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform, CopyLights );
    // As above, but optionally don't copy any attached light animations.
	// Added for multi-launched projectiles which don't need a light each.

    W4dLink* addLink( const W4dTransform3d& localTransform, Solidity solid );
    W4dLink* addLink( W4dLink* parent, const W4dTransform3d& localTransform, Solidity solid );
    // Constructs and adds a new link to the W4dComposite, with initial transform.
    // Variant attaches link to parent link. The W4dLink is not part of the main render tree, in that it
    // is not added to the list of child W4dEntities owned by the w4dcomposite. The links form a
    // distinct subtree.

    void renderLinks( const W4dCamera& camera, MATHEX_SCALAR range, const W4dComplexity& complexity ) const;
    // Render every W4dLink owned by the composite as though it were being
    // displayed at the given range.
    // The options are as specified in the W4dCamera::render methods.

    //render all the objects held by this composite from the point of view
    //of camera. Must only be called from override of render().
    void renderHeldObjects( const W4dCamera& camera ) const;

    //Store the link transforms, and parent pointers
    void storeLinkTransforms( void ) const;

    //Restore the link transforms and parents as they were
    //on last storeLinkTransforms() call
    void restoreLinkTransforms( void );

    void readCompositeFile( const SysPathName& pathname );
    // Read in a composite file and set up the appropriate W4dLinks
    // for this composite
    // PRE( pathname.existsAsFile() );

    //True if successfully reads an animation called animationName from xanFileName ,
    //Updating pCompositePlan with corresponding animations
    //bool readXANAnimation( const SysPathName& xanFileName,
    //                       const string& animationName,
    //                       W4dCompositePlan* pCompositePlan ) const;

    bool readAnimation( const SysPathName& fileName,
                         const string& animationName,
                         W4dCompositePlan* pCompositePlan,
                         MATHEX_SCALAR framesPerSecond ) const;

    virtual void doDebugOutput( ostream& ostr );
    // Write out the details of 'this' for debugging purposes.

private:
	friend class W4dCompositeImpl;

    // Operation deliberately revoked
    W4dComposite( const W4dComposite& );

    // Operation deliberately revoked
    W4dComposite& operator =( const W4dComposite& );

    // Operation deliberately revoked
    bool operator ==( const W4dComposite& );

	W4dCompositeImpl& compositeImpl();
	const W4dCompositeImpl& compositeImpl() const;
	// PRE(pImpl_!=NULL);

//     void parseComposite( const SysPathName& directoryname, UtlLineTokeniser* pParser );
//     void parseObject( const SysPathName& directoryname, UtlLineTokeniser* pParser );
//     void parseShadow( const SysPathName& directoryname, UtlLineTokeniser* pParser );
//     void parseConnect( UtlLineTokeniser* pParser );
//     void parseTransform( const UtlLineTokeniser::Tokens& tokens, size_t tokenOffset, W4dTransform3d* pTransform );

//     bool readXANFile( istream& inputStream, const string& animationName,
//                       W4dCompositePlan* pCompositePlan ) const;

    //Override. Default action is to render all links with lod 0
    virtual RenderType render( const W4dCamera& camera,
                               W4dComplexity& complexity );

	void computeMaxRange();

	void sharedCopyCtor(const W4dComposite& copyMe, CopyLights);

    /////////////////////////////////////////////////////////////////
    //Scaling

    //Recursively apply a non-uniform transform scale to this entity and its
    //descendants. The transform from the ancestor at which scale is applied
    //to this entity is useTransform. This transform is used to compose a
    //modified non-uniform scale.
    //Inherited from W4dEntity. This implementation adequate for composites.
    //Should not be overridden.
    virtual void temporaryScale( const RenNonUniformScale& scale,
                                 const MexTransform3d& useTransform );

    //Recursively apply planPtr to this entity and all its descendants start at startTime, with
    //specified number of repetitions and animation id.
    //The scale plan is to be modified by the offsetTransform.
    //Inherited from W4dEntity. This implementation adequate for composites.
    //Should not be overridden.
    virtual void propogateScalePlan( const W4dScalePlanPtr& planPtr,
                                     const PhysAbsoluteTime& startTime,
                                     uint repetitions, uint animId,
                                     const MexTransform3d& offsetTransform );
    //PRE( planPtr->isNonUniform() )

    /////////////////////////////////////////////////////////////////
	//apply all the built-in animations to the links of the composite
	void applyAllAnimationData(CopyLights copy = COPY_LIGHTS);
	void applyOwnAnimationData();

	// implementation of the class
	W4dCompositeImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( W4dComposite );


#endif

/* End COMPOSIT.HPP *************************************************/
