/*
 * E N T I T Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/entity.hpp"

#pragma GCC diagnostic ignored "-Wunused-variable"
EXISTS( W4dEntity );

#include "stdlib/string.hpp"
#include "world4d/root.hpp"
#include "world4d/domain.hpp"
#include "world4d/camera.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/entscale.hpp"
#include "world4d/lllist.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/internal/animdata.hpp"
#include "world4d/internal/texdata.hpp"
#include "world4d/internal/multitex.hpp"
#include "world4d/internal/uvdata.hpp"
#include "world4d/coldata.hpp"
#include "world4d/internal/multicol.hpp"
#include "world4d/internal/lightdat.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/scalplan.hpp"
#include "world4d/internal/nolight.hpp"
#include "world4d/internal/entityi.hpp"
#include "world4d/internal/subjecti.hpp"
#include "world4d/internal/complexi.hpp"

#include "render/meshinst.hpp"
#include "render/mesh.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"
#include "render/device.hpp"
#include "render/uvanim.hpp"

#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "utility/linetok.hpp"

#include "mathex/line3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/vec2.hpp"

#define CB_W4dEntity_DEPIMPL() \
        CB_DEPIMPL( W4dTransform3d,     localTransform_ ); \
        CB_DEPIMPL( W4dTransform3d,     globalTransform_ ); \
        CB_DEPIMPL( W4dEntity*, pParent_ ); \
        CB_DEPIMPL( W4dEntities*, childList_ ); \
        CB_DEPIMPL( W4dEntityImpl::Meshes*, meshes_ ); \
        CB_DEPIMPL( W4dDomains*, intersectingDomains_ ); \
        CB_DEPIMPL( W4dComposite*, pHolder_ ); \
        CB_DEPIMPL( W4dEntityScale*, pEntityScale_ ); \
        CB_DEPIMPL( W4dEntityPlan*, pPlan_ ); \
        CB_DEPIMPL( W4dEntityImpl::PlanUpdateTimes*, pPlanUpdateTimes_ ); \
        CB_DEPIMPL( W4dSubject*, pSubject_ ); \
        CB_DEPIMPL( ulong, passId_); \
        CB_DEPIMPL( PhysAbsoluteTime, timeGlobalTransformLastUpdated_ ); \
        CB_DEPIMPL( MexTransform3dKey, lastParentGlobalTransformKey_ ); \
        CB_DEPIMPL( MexTransform3dKey, lastLocalTransformKey_ ); \
        CB_DEPIMPL( ulong, checkId_ ); \
        CB_DEPIMPL( W4dId, id_ ); \
        CB_DEPIMPL( MexAlignedBox3d*, boundingVolume_ ); \
        CB_DEPIMPL( W4dLocalLightList*, pLocalLightList_ ); \
        CB_DEPIMPL( string*, name_ ); \
        CB_DEPIMPL( RenColour*, pFilterColour_ ); \
        CB_DEPIMPL( int, nLinkedSounds_ );

PER_DEFINE_PERSISTENT_ABSTRACT( W4dEntity );



W4dEntity::W4dEntity( W4dEntity* pParent, const W4dTransform3d& newLocalTransform, Solidity solid )
: pImpl_(_NEW( W4dEntityImpl( pParent, newLocalTransform, solid ) ) )
{
    PRE( pParent != NULL );
    LOG_CONSTRUCTION;

    pParent->childListForEdit()->push_back( this );

        // Increment the entity count in this tree's root.
        findRoot()->adjustDescendantCount(1);

	pImpl_->updateHasMeshFlag();

    TEST_INVARIANT;
}

W4dEntity::W4dEntity(
    const W4dEntity& copyMe,
    W4dEntity* pParent,
    const W4dTransform3d& newLocalTransform )
:  pImpl_(_NEW( W4dEntityImpl( copyMe.entityImpl(), pParent, newLocalTransform ) ) )
{
        CB_W4dEntity_DEPIMPL();

    PRE( pParent != NULL );
    LOG_CONSTRUCTION;

    //  Does not copy any motion plan

    //  Does not copy the entity's children

        // cannot take a refenrence on a bitfield thus we have to use the pimple to access
        // ownedByParent_
    if( pImpl_->ownedByParent_ )
        pParent->childListForEdit()->push_back( this );

        // Increment the entity count in this tree's root.
        findRoot()->adjustDescendantCount(1);

    //  Make sure all of the meshes are properly set up
    meshes_->reserve( copyMe.entityImpl().meshes_->size() );

    for( size_t id = 0; id < copyMe.entityImpl().meshes_->size(); ++id )
    {
                W4dEntityImpl::MeshData& meshData = (*copyMe.entityImpl().meshes_)[id];

        if (meshData.mesh)
            add( _NEW( RenMeshInstance(*meshData.mesh) ), sqrt( meshData.distance ), id );
        else
            add( NULL,  sqrt( meshData.distance ), id );
    }

	pImpl_->updateHasMeshFlag();

    //Need current flag indcating whether this entity has any scaling
    updateNotScaledFlag();

	// This is only possible because composite is a data attribute of the
	// impl.  If, say, isComposite were virtual, this wouldn't work here.
	ASSERT(implies(!copyMe.isComposite(), !isComposite()), "Didn't copy W4dEntity::composite attribute");
	ASSERT(implies( copyMe.isComposite(),  isComposite()), "Didn't copy W4dEntity::composite attribute");

//    if( hasMesh() )
	// If this entity is a composite, then the composite ctor is responsible for
	// applying the animations (cause they're done differently).
	if (!isComposite() && pImpl_->hasAnimationData())
	{
		pImpl_->applyAllAnimations(this, W4dManager::instance().time());
	}

	IAIN_STREAM((doNotLight()? "Dis": "En") << "abled lighting in copy ctor of " << this << "\n");

    TEST_INVARIANT;
}

// This constructor only used for constructing W4dRoots

W4dEntity::W4dEntity()
: pImpl_( _NEW( W4dEntityImpl() ) )
{
	CB_W4dEntity_DEPIMPL();

    LOG_CONSTRUCTION;
    meshes_->reserve( 1 );

	// Increment the entity count in this tree's root.
	findRoot()->adjustDescendantCount(1);

	pImpl_->updateHasMeshFlag();
}

W4dEntity::W4dEntity( W4dEntity* pParent, const W4dTransform3d& newLocalTransform, Solidity solid, NotAChild notAChild)
: pImpl_( _NEW( W4dEntityImpl( pParent, newLocalTransform, solid, notAChild ) ) )
{
	CB_W4dEntity_DEPIMPL();
    LOG_CONSTRUCTION;
    meshes_->reserve( 4 );

	pImpl_->updateHasMeshFlag();
}

W4dEntity::~W4dEntity()
{
        CB_W4dEntity_DEPIMPL();

    TEST_INVARIANT;

        if( isGarbage() )
                W4dGarbageCollector::instance().remove( this );

    //Disassociate any linked sounds
    if( nLinkedSounds_ != 0 )
        W4dSoundManager::instance().stop( this );

        //Disassociate any lights
        if( pLocalLightList_ )
                pLocalLightList_->clearAllLights( this );

        // Recursively delete all of this entity's descendants.
    //However, children owned by a counted pointer must be attached to a hidden root,
    //to be deleted naturally when the last counted ptr goes out of scope.
    //Otherwise double deletion will occur.
    while( childList_  and  childList_->size() != 0 )
    {
        W4dEntity* pChild = childList_->back();
        if( pChild->isOwnedByCountedPtr() )
        {
            pChild->attachTo( &W4dRoot::hiddenRoot(), pChild->localTransform() );
        }
        else
        {
            _DELETE( pChild );
        }
    }

    //Adjust the ancestor root descendant count, but only if the 'owned by parent'
    //flag is true
        if( pImpl_->ownedByParent_ )
            findRoot()->adjustDescendantCount(-1);

    //Cancel any holding relationship
    cancelIsHeld();

    //Remove from parent's child list if appropriate
    if( pImpl_->ownedByParent_  and  hasParent() )
    {
        ASSERT( pParent()->pImpl_->childList_ != NULL, "Parent has no child list" );
        pParent()->pImpl_->childList_->remove( this );
    }

    //Remove any intersecting domains relations
    while( intersectingDomains_ != NULL  and  intersectingDomains_->size() != 0 )
        intersects( intersectingDomains_->back(), false );

        _DELETE( pImpl_ );

    LOG_DESTRUCTION;
}


void W4dEntity::attachTo( W4dEntity* pNewParent, const W4dTransform3d& newLocalTransform )
{

    PRE( hasParent() );
    PRE( pNewParent != NULL );
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    //Ensure the local transform is updated from any motion plan, then clear any
    //absolute motion plan (can't be valid after change of owner)
    localTransform();
    if( pPlan_ != NULL )
        pPlan_->clearAbsoluteMotionPlans();

    //Remove from parent's child list if storing in child lists
        W4dEntity* oldParent = pParent();
    if( pImpl_->ownedByParent_ )
        oldParent->removeChild( this );

    //Add to new parent's child list if required
    pParent_ = pNewParent;
    if( pImpl_->ownedByParent_ )
        pNewParent->childListForEdit()->push_back( this );

    //Store the new local transform
    localTransform( newLocalTransform );

    //Cancel any holding relationship with a previous parent W4dLink
    cancelIsHeld();

        // Adjust the root's descendant count if necessary.
        W4dRoot* oldRoot = oldParent->findRoot();
        W4dRoot* newRoot =  pParent()->findRoot();
        if (oldRoot != newRoot)
        {
                const int count = computeDescendantCount();
                oldRoot->adjustDescendantCount(-count);
                newRoot->adjustDescendantCount( count);
        }

    //If associated with a logical subject, update any relevant observers
    //if resulted in domain change.
    if( pImpl_->ownedByParent_  and  pSubject_ != NULL )
    {
        pSubject_->impl().updateDomainObservers( *oldParent );
    }
}

void W4dEntity::attachTo( W4dEntity* pNewParent )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();

    //Compute the required local transform to maintain the same global position
    W4dTransform3d newLocalTransform;
    relativeTransform( *pNewParent, &newLocalTransform );

    //Switch to new parent using new transform
    attachTo( pNewParent, newLocalTransform );

    TEST_INVARIANT;
}

bool W4dEntity::hasParent() const
{
    TEST_INVARIANT;
    return pImpl_->hasParent();
}

W4dEntity* W4dEntity::pParent() const
{
    TEST_INVARIANT;
    return pImpl_->pParent();
}

void W4dEntity::removeChild( const W4dEntity* pChild )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( childList_ != NULL );

    // PRE( hasChild( pChild ) );

    W4dEntities::iterator   childPosition;

    childPosition = find( childList_->begin(), childList_->end(), pChild );

    ASSERT( childPosition != childList_->end(), "Child not in list" );

    childList_->erase( childPosition );

    TEST_INVARIANT;
}

W4dDomain* W4dEntity::containingDomain() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( hasParent() );


    W4dEntity*  pNode = pParent();
    W4dDomain*  result = NULL;

    bool finished = false;

    while( not finished )
    {
        if( pNode->isDomain() )
        {
            result = pNode->getAsDomain();
            finished = true;
        }
        else if( pNode->hasParent() )
        {
            pNode = pNode->pParent();
        }
        else
            finished = true;
    }

    POST( result != NULL );

    TEST_INVARIANT;

    return result;
}

bool W4dEntity::isDomain() const
{
    TEST_INVARIANT;

	CB_W4dEntity_DEPIMPL();

    return pImpl_->isDomain_;
}

void W4dEntity::isDomain( bool newFlag )
// This function is only for use by the W4dDomain constructor.
{
    TEST_INVARIANT;

	CB_W4dEntity_DEPIMPL();

    pImpl_->isDomain_ = newFlag;
}

W4dDomain* W4dEntity::getAsDomain()
{
    TEST_INVARIANT;

    PRE( isDomain() );

    W4dDomain* result = _STATIC_CAST( W4dDomain*, this );

    POST( result != NULL );

    return result;
}

void W4dEntity::intersects( W4dDomain* pDomain, bool intersects )
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    PRE( pDomain != NULL );
    PRE( intersects or intersectingDomains_ != NULL );

    //If adding an intersection, ensure we have an intersecting domains collection
    if( intersectingDomains_ == NULL )
        intersectingDomains_ = _NEW( W4dDomains );

    W4dDomains::iterator   domainPosition;

    domainPosition = find( intersectingDomains_->begin(), intersectingDomains_->end(), pDomain );

    if( intersects )
    {
        ASSERT( domainPosition == intersectingDomains_->end(), "Domain is already in intersecting domains list" );

        intersectingDomains_->push_back( pDomain );
    }
    else
    {
        ASSERT( domainPosition != intersectingDomains_->end(), "Domain is not in intersecting domains list" );

        intersectingDomains_->erase( domainPosition );
    }

    pDomain->intersects( this, intersects );
}

bool W4dEntity::intersects( const W4dDomain& domain ) const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    bool result = intersectingDomains_ != NULL;
    if( result )
    {
        W4dDomains::const_iterator   domainPosition;

        domainPosition = find(
          intersectingDomains_->begin(),
          intersectingDomains_->end(),
          &domain );

        result = domainPosition != intersectingDomains_->end();
    }

    return result;
}

const W4dEntity::W4dDomains& W4dEntity::intersectingDomains() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    //Used to return empty domain list when required
    static W4dDomains dummyIntersectingDomains;

    return ( intersectingDomains_ ? *intersectingDomains_ : dummyIntersectingDomains );
}

void W4dEntity::intersectingDomains(const W4dDomains& newDoms)
{
        CB_W4dEntity_DEPIMPL();

        const bool needNonZeroDomains = newDoms.size() > 0;

    if (!intersectingDomains_)
    {
        if (needNonZeroDomains)
                intersectingDomains_ = _NEW( W4dDomains );
    }
        else
        {
                // There were previous intersections.  Iterate thru the list of
                // domains and clear the back pointers from the domains to this object.
                W4dDomains::iterator it = intersectingDomains_->begin();
                while (it != intersectingDomains_->end())
                {
                    (*it)->intersects(this, false);
                        ++it;
                }
        }

        if (needNonZeroDomains)
        {
                ASSERT(intersectingDomains_, "Intersecting domains should have been alloc'ed");
                *intersectingDomains_ = newDoms;

                // Set up the back pointers from the domains.
                W4dDomains::iterator it = intersectingDomains_->begin();
                while (it != intersectingDomains_->end())
                {
                    (*it)->intersects(this, true);
                        ++it;
                }
        }
        else
        {
                _DELETE(intersectingDomains_);
                intersectingDomains_ = NULL;
        }
}

bool W4dEntity::hasIntersectingDomains() const
{
	CB_W4dEntity_DEPIMPL();
	return intersectingDomains_!=NULL;
}

void W4dEntity::createIntersectingDomains()
{
	CB_W4dEntity_DEPIMPL();
	PRE(not hasIntersectingDomains());
	intersectingDomains_ = _NEW( W4dDomains );
}

void W4dEntity::addIntersectingDomain( W4dDomain* domain )
{
	CB_W4dEntity_DEPIMPL();
	PRE(hasIntersectingDomains());
	PRE(not intersects(*domain));

	intersectingDomains_->push_back( domain );
	domain->intersects( this, true );
}

void W4dEntity::eraseIntersectingDomains()
{
	CB_W4dEntity_DEPIMPL();
	PRE(hasIntersectingDomains());
	W4dDomains::iterator it = intersectingDomains_->begin();
	while (it != intersectingDomains_->end())
	{
	    (*it)->intersects(this, false);
		++it;
	}
	intersectingDomains_->erase(intersectingDomains_->begin(), intersectingDomains_->end() );
	POST(hasIntersectingDomains());
}

void W4dEntity::deleteIntersectingDomains()
{
	CB_W4dEntity_DEPIMPL();
	PRE(hasIntersectingDomains());
	W4dDomains::iterator it = intersectingDomains_->begin();
	while (it != intersectingDomains_->end())
	{
	    (*it)->intersects(this, false);
		++it;
	}
	_DELETE( intersectingDomains_ );
	intersectingDomains_=NULL;

	POST(not hasIntersectingDomains());
}

void W4dEntity::localTransform( const W4dTransform3d& newLocalTransform )
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        PRE( pParent() );

    localTransform_ = newLocalTransform;

    //Ensure the global transforms get updated
    markGlobalTransformsDirty();

    //If transform is being scaled, apply the scale
    if( pEntityScale_ != NULL  and  scaleTransformFromParent() )
    {
        //Store the unscaled transform position
        pEntityScale_->unscaledTransformPosition( localTransform_.position() );

        //Scale it
        scaleTransformPosition();
    }
}

const W4dTransform3d& W4dEntity::localTransform() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    //Check for special conditions under which we might need to lazily evaluate
    bool hasTransformPlan = pPlan_  and  pPlan_->hasMotionPlan();
    bool useParentScale = scaleTransformFromParent();

    //In either case do some updating
    if( hasTransformPlan or useParentScale )
    {
        //get current time, and check for up to date
        PhysAbsoluteTime timeNow = W4dManager::instance().time();
        if( pPlanUpdateTimes_ == NULL  or  pPlanUpdateTimes_->transformTime != timeNow )
        {
            //Update the transform from any motion plan
            if( hasTransformPlan )
            {
                if( _CONST_CAST( W4dEntity*, this )->updateTransformFromPlan() )
                {
                    //Update any stored unscaled transform position
                    if( useParentScale )
                        pEntityScale_->unscaledTransformPosition( localTransform_.position() );
                }
            }

            //Update the transform position from parent scale if required
            if( useParentScale )
                _CONST_CAST( W4dEntity*, this )->scaleTransformPosition();
        }
    }

    return localTransform_;
}

bool W4dEntity::isHeld() const
// true iff this object is being held by a composite object
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();

    return pHolder_ != NULL;
}

W4dComposite* W4dEntity::pHolder() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( isHeld() );

    return pHolder_;
}

void W4dEntity::beHeldBy( W4dComposite* pHolder )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();

    //cancel any existing holding relationship
    cancelIsHeld();

    //Store the new holder
    pHolder_ = pHolder;
}

void W4dEntity::cancelIsHeld()
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    //Clear any existing holder.
    W4dComposite* pHolder = pHolder_;
    if( pHolder != NULL )
    {
        //The call to stopHolding will generate a messy recursive call to this function, unless
        //pHolder_ is NULL. Hence the order of these 2 lines, and use of temp variable.
        pHolder_ = NULL;
        pHolder->stopHolding( this );
    }
}

const W4dTransform3d& W4dEntity::globalTransform() const
{
    TEST_INVARIANT;
    CB_W4dEntity_DEPIMPL();

    const W4dTransform3d* pResult = &globalTransform_;

    //Check has a parent
    if( pParent_ != NULL )
    {
        //Check for up to date
        PhysAbsoluteTime now = W4dManager::instance().time();

        if( now != timeGlobalTransformLastUpdated_ )
        {
            //Will now be updated
            //W4dEntity* nonConstThis = _CONST_CAST( W4dEntity*, this );
            const W4dEntity* nonConstThis =  this;
            nonConstThis->pImpl_->timeGlobalTransformLastUpdated_ = now;

            //Get the parent's global transform and this entity's current local transform
            const W4dTransform3d& parentGlobalTransform = pParent_->globalTransform();
            const W4dTransform3d& myTransform = localTransform();

            //See if the transform keys still match. If so no need to recompute.
            if( parentGlobalTransform.key() != lastParentGlobalTransformKey_   or
                myTransform.key() != lastLocalTransformKey_ )
            {
                //cache the keys
                nonConstThis->pImpl_->lastParentGlobalTransformKey_ = parentGlobalTransform.key();
                nonConstThis->pImpl_->lastLocalTransformKey_ = myTransform.key();

                //Compute the new transform
                parentGlobalTransform.transform( myTransform, &nonConstThis->pImpl_->globalTransform_ );
            }
        }
    }
    else
    {
        //No parent, so return local transform
        pResult = &(localTransform());
    }

    return *pResult;
}

void W4dEntity::globalTransform( const W4dTransform3d& newGlobalTransform )
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        PRE(hasParent());               // Not allowed to transform the root.

    // Calculate the local transform necessary to place the entity
    // at the given global transform.

    W4dTransform3d  parentGlobalTransform = pParent()->globalTransform();
    parentGlobalTransform.invert();
    parentGlobalTransform.transform( newGlobalTransform, &localTransform_ );

    //Ensure the global transforms get updated
    markGlobalTransformsDirty();
}

void W4dEntity::debugOutput( ostream& ostr )
// Write out the details of 'this' and all its children for debugging
// purposes.
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    ostr << "Start W4dEntity " << (void*)this << std::endl;
    doDebugOutput( ostr );

    ostr << "Entity has " << (childList_ ? childList_->size() : 0) << " children" << std::endl;
    if( meshes_ )
    {
        ostr << "Entity has " << meshes_->size() << " meshes" << std::endl;

        for( size_t i = 0; i < meshes_->size(); ++i )
        {
            const W4dEntityImpl::MeshData& data = (*meshes_)[ i ];
            ostr << "Mesh " << i << " distance " << data.distance << std::endl;
            if( data.mesh )
            {
                ostr << *data.mesh;
            }
            else
                ostr << "NULL mesh" << std::endl;
        }
    }

    if( hasParent() )
        ostr << "Entity has parent " << (void*)pParent() << std::endl;
    else
        ostr << "Entity has no parent" << std::endl;

    ostr << "Local transform:" << std::endl;
    ostr << localTransform() << std::endl;

    ostr << "Global transform:" << std::endl;
    ostr << globalTransform() << std::endl;

    if( childList_ )
    {
        for( W4dEntities::iterator i = childList_->begin();
         i != childList_->end();
          ++i )
        {
            (*i)->debugOutput( ostr );
        }
    }

    ostr << "End   W4dEntity " << (void*)this << std::endl;
}

void W4dEntity::debugOutputChildList( ostream& ostr ) const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    if( childList_ )
    {
        for( W4dEntities::const_iterator i = childList_->begin();
         i != childList_->end();
          ++i )
        {
            ostr << (void*)(*i) << " ";
        }
    }
    ostr << "END";
}

void W4dEntity::add(RenMeshInstance* pMesh, const W4dDistance& distance, W4dLOD id)
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        // If there's an existing mesh for this id, remove it.
     if( meshes_->size() > id )
                _DELETE((*meshes_)[id].mesh);

        // Ensure that the vector has at least id+1 elements.
        int newElements = id + 1 - meshes_->size();

        for (int i=0; i < newElements; ++i)
                meshes_->push_back(W4dEntityImpl::MeshData());

        (*meshes_)[id].mesh     = pMesh;
        (*meshes_)[id].distance = distance * distance;  // squared!

	pImpl_->updateHasMeshFlag();
    if( id == 0 and pMesh != NULL )
    {
        //Need current flag indicating whether this entity has any scaling
        updateNotScaledFlag();
    }
}

void W4dEntity::add(RenMeshInstance* pMesh, W4dLOD id)
{
    TEST_INVARIANT;
    add( pMesh, MexDouble::plusInfinity(), id );
}

void W4dEntity::setDistance( const W4dDistance& distance, W4dLOD id)
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    PRE_INFO( id );
    PRE_INFO( meshes_->size() );

    PRE( id < meshes_->size() );

        (*meshes_)[id].distance = distance * distance;  // squared!
}

// virtual
W4dEntity::RenderType W4dEntity::render
(
        const W4dCamera& camera,
        W4dComplexity& complexity
)
{
	    TEST_INVARIANT;
		PRE_INFO( name() );
		PRE(  hasMesh() );

        // Compute the square of the camera to eye distance and search the mesh
	    // list for the appropriate mesh.
        const MexPoint3d eyePos( camera.globalTransform().position() );
	    const MexPoint3d  myPos( globalTransform().position() );
	    const W4dDistance effectiveRangeSqr = eyePos.sqrEuclidianDistance(myPos);

	        // Enable any colour filter attached to this entity.
        const RenColour& filter = (filterColourIsSet())? filterColour(): RenColour::white();
        RenDevice::Filter colourfilter(RenDevice::current(), filter);

		// use the lowest rendering priority for an entity
		if( complexity.enabled() )
		{
	       	complexity.usePriority( 0 );
	       	complexity.range( effectiveRangeSqr );
		}
	    renderAtRange( camera, effectiveRangeSqr, complexity );

        return RENDER_CHILDREN;
}

// virtual
void W4dEntity::renderAtRange
(
        const W4dCamera& camera,
        const W4dDistance& range,
        const W4dComplexity& complexity
)
{
    TEST_INVARIANT;

	CB_W4dEntity_DEPIMPL();

    RenMeshInstance*    pMesh;
    W4dLOD lodId;

    if( rangeToMesh( range, &pMesh, &lodId, complexity ) )
    {
        ASSERT( pMesh != NULL, "" );

       	//  Deal with any material plans that we might have
       	Ren::MaterialVecPtr oldMaterialVecPtr;
   		bool materialOverridden = false;
        if( pPlan_ )
        {
		    W4dManager& manager = W4dManager::instance();
    		PhysAbsoluteTime timeNow = manager.time();

            //We must clear any obsolete plans before checking for the current plan, otherwise
            //we'll get the tail end of the last expired plan. This is because all other plans
            //leave the entity with a permanent state change, which material plans do not.
            pPlan_->clearMaterialPlansAtTime( timeNow );

            if( pPlan_->hasMaterialPlan() and pPlanUpdateTimes_->materialTime != timeNow )
            {
                //See if a material vector override is defined for this level of detail,
                //and if so apply it.
                uint nObsoletePlans = 0;
                Ren::MaterialVecPtr materialVecPtr;

                if( pPlan_->materialVec( timeNow, lodId, &materialVecPtr, &nObsoletePlans ) ==
                    W4dEntityPlan::DEFINED )
                {
                    //Save the old material
                    materialOverridden = true;
                    oldMaterialVecPtr = pMesh->materialVec();

                    //Set the override
                    pMesh->materialVec( materialVecPtr );
                }

                //Since material overrides restored after render call application
                //time doesn't apply
                //pPlanUpdateTimes_->materialTime = timeNow;

                //Clear any obsolete material plans. We must ensure that any other levels
                //of detail (besides the one we just did) are updated first.
                if( nObsoletePlans != 0 )
                {
                    //Update all levels of detail if clearing final plan
                    //!!!Material plans now regarded as temporary, so don't do this
                    //if( nObsoletePlans == pPlan_->nMaterialPlans() )
                    //    updateLODMaterials( timeNow, lodId );

                    //Clear obsolete plans
                    if( not manager.hasArtificialTime() )
                        pPlan_->clearMaterialPlans( nObsoletePlans );
                }
            }

	        //Deal with any texture coordinate plans
            if( pPlan_->hasUVPlan() and pPlanUpdateTimes_->uvTime != timeNow )
            {
                uint nObsoletePlans = 0;
                Ren::UVTransformPtr uvTransformPtr;
                W4dLOD maxLOD;
                if( pPlan_->uvTransform( timeNow, lodId, &uvTransformPtr, &maxLOD, &nObsoletePlans)
                    == W4dEntityPlan::DEFINED )
                {
                    //Set the mesh texture transform
                    pMesh->animateTextureCoords( uvTransformPtr );

                    //Clear any obsolete uv plans
                    if( nObsoletePlans != 0  and  not manager.hasArtificialTime() )
                    {
                        updateLODUVTransforms( uvTransformPtr, maxLOD, lodId );
                        pPlan_->clearUVPlans( nObsoletePlans );
                    }
                }

                pPlanUpdateTimes_->uvTime = timeNow;
            }

			if( pPlan_->hasMeshPlan() )
			{
				W4dEntity* nonConstThis = (W4dEntity*)this;
        		uint nObsoletePlans = 0;
        		pPlan_->renderMesh( camera, range, nonConstThis, timeNow, &nObsoletePlans, complexity );

        		//Clear any obsolete mesh plans
       			if( nObsoletePlans != 0  and  not manager.hasArtificialTime() )
            		pPlan_->clearMeshPlans( nObsoletePlans );
			}
		}

    	//Check for scale override
    	if( hasTemporaryScale() )
    		pMesh->render( globalTransform(), temporaryScale() );
    	else
       		pMesh->render( globalTransform() );

    	//Restore any old material if overridden
    	if( materialOverridden )
       		pMesh->materialVec( oldMaterialVecPtr );
	}

}

bool W4dEntity::rangeToMesh
(
    const W4dDistance& rangeConst, RenMeshInstance** ppMesh, W4dLOD* pLodId, const W4dComplexity& complexity
) const
{
    TEST_INVARIANT;

	CB_W4dEntity_DEPIMPL();

	W4dDistance range = 100; // Lower values are causing some troubles, why?
   	if( complexity.enabledLOD()) range = rangeConst;
   	bool drawMesh = false;

	if( complexity.enabled() )
	{
		W4dDistance modifiedRange = range;
		if( complexity.enabledLOD()) modifiedRange = complexity.modifiedRange();
		// so far modifiedRange implements a linear function

		// We compare the lod distance to the modifier range (range*complexity)
		// except for the last lod which we compare     to the range

	    if( meshes_->size() == 0 )
    	{
        	drawMesh = false;
    	}
    	else
    	{
	        bool foundMesh = false;
	        W4dLOD lodId = 0;
	        for( W4dEntityImpl::Meshes::const_iterator it = meshes_->begin(); it != meshes_->end() && not foundMesh; ++it )
	        {
				// If this mesh is within range, draw it.
				if( it != (meshes_->end()-1) )
	            {
	            	if( modifiedRange < (*it).distance )
					{
	                    foundMesh = true;
	                    *ppMesh = (*it).mesh;
	                    *pLodId = lodId;
						if( *ppMesh != NULL )
	                        drawMesh = true;
					}
				}
				else // the last lod distance is compared to the range
				{
					if( range < (*it).distance )
					{
						foundMesh = true;
						*ppMesh = (*it).mesh;
	                    *pLodId = lodId;
	                    if( *ppMesh != NULL )
	                        drawMesh = true;
					}
				}
	            //Advance level of detail indicator
	            ++lodId;
	        }
		}
    }
	else // not autoAdjustFrameRate()
	{
	    if( meshes_->size() == 0 )
    	{
        	drawMesh = false;
    	}
    	else
    	{
	        bool foundMesh = false;
	        W4dLOD lodId = 0;
	        for( W4dEntityImpl::Meshes::const_iterator it = meshes_->begin(); it != meshes_->end() && not foundMesh; ++it )
	        {
				if( range < (*it).distance )
				{
					foundMesh = true;
					*ppMesh = (*it).mesh;
                    *pLodId = lodId;
                    if( *ppMesh != NULL )
                        drawMesh = true;
				}
	            //Advance level of detail indicator
	            ++lodId;
	        }
		}
	}

    POST( implies( drawMesh, *ppMesh != NULL ) );
        return drawMesh;
}

W4dEntity::W4dDistance W4dEntity::maxRange() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        W4dDistance retval = 0;

        for( W4dEntityImpl::Meshes::const_iterator it = meshes_->begin(); it != meshes_->end(); ++it )
        {
                const RenMeshInstance* mi = (*it).mesh;
                if (mi)
        {
                        retval = std::max(retval, (*it).distance);
        }
        }

        // Having accounted for this entity's meshes, recursively search children.
    for (W4dEntities::const_iterator i = children().begin(); i != children().end(); ++i)
    {
        W4dEntity* kid = *i;
                if (kid)
                        retval = std::max(retval, kid->maxRange());
    }

        POST(retval >= W4dDistance(0) );
        return retval;
}

const W4dEntity::W4dEntities& W4dEntity::children() const
{
    TEST_INVARIANT;
	return pImpl_->children();
}

W4dRoot* W4dEntity::findRoot()
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        W4dEntity* retval = this;

        while (retval->hasParent())
    {

                retval = retval->pParent();
        }

        // We assume that because retval is at the top of the tree, it must
        // be an object of type W4dRoot.  This could possibly be a dynamic_cast
        // if the compiler supported it and we wanted the RTTI overhead.
        ASSERT(!retval->hasParent(), logic_error());
        return _STATIC_CAST(W4dRoot*, retval);
}

const W4dRoot* W4dEntity::findRoot() const
{
    TEST_INVARIANT;
        const W4dEntity* retval = this;
        while (retval->hasParent())
                retval = retval->pParent();

        // We assume that because retval is at the top of the tree, it must
        // be an object of type W4dRoot.  This could possibly be a dynamic_cast
        // if the compiler supported it and we wanted the RTTI overhead.
        ASSERT(!retval->hasParent(), logic_error());
        return _STATIC_CAST(const W4dRoot*, retval);
}

int W4dEntity::computeDescendantCount() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        int totalForThisNode = 1;       // self

    if( childList_ )
    {
        for(W4dEntities::const_iterator i = childList_->begin(); i != childList_->end(); ++i)
                totalForThisNode += (*i)->computeDescendantCount();
    }

        return totalForThisNode;
}

W4dEntityPlan& W4dEntity::plan()
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    //Construct the entity plan
        if (!pPlan_)
                pPlan_ = _NEW( W4dEntityPlan );

    //Construct the structure used to cache last update times for the plans
    if( pPlanUpdateTimes_ == NULL )
    {
        PhysAbsoluteTime initTime = 0;
        pPlanUpdateTimes_ = _NEW( W4dEntityImpl::PlanUpdateTimes );
        pPlanUpdateTimes_->transformTime = initTime;
        pPlanUpdateTimes_->visibilityTime = initTime;
        pPlanUpdateTimes_->scaleTime = initTime;
        pPlanUpdateTimes_->uvTime = initTime;
        pPlanUpdateTimes_->materialTime = initTime;
    }

        return *pPlan_;
}

const W4dEntityPlan& W4dEntity::entityPlan() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    PRE( hasPlan () );

    return *pPlan_;
}

bool    W4dEntity::hasPlan() const
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    return pPlan_ != NULL;
}


bool W4dEntity::clearPlanIfDone()
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

        bool deleted = false;

        //Check there is a plan
        if( pPlan_ )
        {
                //Get current simulation time
                PhysAbsoluteTime time = W4dManager::instance().time();

                //Check for complete
                if( pPlan_->isDone( time )  and  not W4dManager::instance().hasArtificialTime() )
                {
                        _DELETE( pPlan_ );
                        pPlan_ = NULL;
                        deleted = true;

            //Also delete the structure used to cache last update times
            _DELETE( pPlanUpdateTimes_ );
            pPlanUpdateTimes_ = NULL;
                }
        }

        return deleted;
}

bool W4dEntity::updateTransformFromPlan()
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();


    //LINEAR_TRAVEL_WHERE;
    //LINEAR_TRAVEL_INSPECT( localTransform_ );
    const MexVec3 oldZBasis = localTransform_.zBasis();

        bool updated = false;

    //Get current simulation time
    W4dManager& manager = W4dManager::instance();
        PhysAbsoluteTime time = manager.time();

        //Check there is a plan
        if( pPlan_  and  pPlan_->hasMotionPlan() )
        {
                //Update the local transform
        W4dEntityPlan::PlanState state;
        uint nObsoletePlans = 0;

                state = pPlan_->transform( time, &localTransform_, &nObsoletePlans );
                updated = state == W4dEntityPlan::DEFINED;

                //Clear any completed plans
        if( nObsoletePlans != 0  and  not manager.hasArtificialTime() )
                pPlan_->clearMotionPlans( nObsoletePlans );
        }

    //Set the last update time
    pPlanUpdateTimes_->transformTime = time;

    //LINEAR_TRAVEL_WHERE;
    //LINEAR_TRAVEL_INSPECT( localTransform_ );
    MexDegrees angle = localTransform_.zBasis().angleBetween( oldZBasis );
    //LINEAR_TRAVEL_INSPECT( oldZBasis );
    //LINEAR_TRAVEL_INSPECT( localTransform_.zBasis() );
    //LINEAR_TRAVEL_INSPECT( angle.asScalar() );
    //if( angle.asScalar() > 10.0 )
    //{
    //    LINEAR_TRAVEL_STREAM( "  **************** JUMP ****************" << std::endl );
    //}

        return updated;
}

void W4dEntity::readLODFile( const SysPathName& pathname )
// Read in a level of detail file and add all of the
// referenced meshes to this.
{
    TEST_INVARIANT;

    CB_W4dEntity_DEPIMPL();

    PRE_INFO( pathname.pathname() );
    PRE( pathname.existsAsFile() );
    PRE(pImpl_);

    const SysPathName& directoryname = pathname.directory();

    UtlLineTokeniser   parser( pathname );

    ASSERT( not parser.finished(), "LOD file empty" );

    W4dLOD  id = 0;

    while( not parser.finished() )
    {
        if( parser.tokens()[ 0 ] == "MESH" or parser.tokens()[ 0 ] == "EMPTY_MESH" )
        {
	        parseMesh( directoryname, &parser, id++ );
        }
        else if( parser.tokens()[ 0 ] == "ANIMATION_DATA" )
        {
			parseAnimationData( &parser );
        }
    }

    name( pathname.filename() );

    //Need current flag indcating whether this entity has any scaling
    updateNotScaledFlag();

	if (pImpl_->hasAnimationData())
	{
    	pImpl_->applyAllAnimations(this, W4dManager::instance().time());
	}

    // Check to see if this entity is entirely emissive.
    if (!w4dNeedsLighting(this))
    {
        doNotLight(true);
        IAIN_STREAM("Disabled lighting for " << pathname << "\n");
    }
}

void W4dEntity::parseMesh( const SysPathName& directoryname, UtlLineTokeniser* pParser, W4dLOD id )
{
    TEST_INVARIANT;

        CB_W4dEntity_DEPIMPL();

    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "MESH" or
            pParser->tokens()[ 0 ] == "EMPTY_MESH", "" );

    RenMeshInstance* pMesh = NULL;

    if( pParser->tokens()[ 0 ] == "MESH" )
    {
        ASSERT( pParser->tokens().size() == 3, "" );

        SysPathName meshPathname( directoryname );
        meshPathname.combine( SysPathName( pParser->tokens()[ 1 ] ) );

        ASSERT_INFO( meshPathname );

        const string& meshName = pParser->tokens()[ 2 ];

        pMesh = RenMeshInstance::createShared(meshPathname, meshName);

                if (pMesh)
                //*name_ = meshName;
                        name ( meshName );
    }
    else if( pParser->tokens()[ 0 ] == "EMPTY_MESH" )
    {
        ASSERT( pParser->tokens().size() == 1, "" );
        pMesh = NULL;
    }

    pParser->parseNextLine();

    if( pParser->finished() or pParser->tokens()[ 0 ] != "DISTANCE" )
        add( pMesh, HUGE_VAL, id );             // HUGE_VAL => effectively on until infinity.

    else if( pParser->tokens()[ 0 ] == "DISTANCE" )
    {
        ASSERT_INFO( pParser->tokens()[ 0 ] );
        //ASSERT( pParser->tokens()[ 0 ] == "DISTANCE", "" );
        ASSERT( pParser->tokens().size() == 2, "" );

        const double distance = atof( pParser->tokens()[ 1 ].c_str() );
        add( pMesh, distance, id );

        pParser->parseNextLine();

                // If the last keyword is "DISTANCE", as opposed to "MESH", then assume
                // that an empty mesh follows.
                if (pParser->finished())
                        add(NULL, id+1);
    }
}

bool W4dEntity::hasMesh( ) const
{
    TEST_INVARIANT;
    return pImpl_->hasMesh();
}

bool W4dEntity::hasMesh( W4dLOD id ) const
{
    TEST_INVARIANT;
	CB_W4dEntity_DEPIMPL();
    return meshes_->size() >id   and   (*meshes_)[id].mesh;
}

const RenMeshInstance& W4dEntity::mesh( W4dLOD id ) const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( hasMesh( id ) );
    return *((*meshes_)[id].mesh);
}

ulong W4dEntity::latestRenderPassId() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
        return passId_;
}

void W4dEntity::name(const string& n)
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
        //*name_ = n;
        pImpl_->name( n );
}

const string& W4dEntity::name() const
{
    TEST_INVARIANT;
        return pImpl_->name();
}

void W4dEntity::passId( ulong id )
{
    pImpl_->passId( id );
}

ulong W4dEntity::passId() const
{
    TEST_INVARIANT;
    return pImpl_->passId();
}

void W4dEntity::clearPlan()
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    //Delete any plan
    if( pPlan_ )
    {
        _DELETE( pPlan_ );
        pPlan_ = NULL;
        _DELETE( pPlanUpdateTimes_ );
        pPlanUpdateTimes_ = NULL;
    }
}

void W4dEntity::replaceMeshes( const W4dEntity& entity )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    meshes_->erase( meshes_->begin(), meshes_->end() );

    ASSERT( meshes_->size() == 0, "" );

    meshes_->reserve( entity.entityImpl().meshes_->size() );

    size_t id = 0;
    for( W4dEntityImpl::Meshes::const_iterator i = entity.entityImpl().meshes_->begin(); i != entity.entityImpl().meshes_->end(); ++i )
    {
                const W4dEntityImpl::MeshData& meshData = *i;

        if( meshData.mesh )
            add( _NEW( RenMeshInstance( *(meshData.mesh )) ), sqrt( meshData.distance ), id );
        else
            add( NULL, sqrt( meshData.distance ), id );

        ++id;
    }

    //Need current flag indcating whether this entity has any scaling
	pImpl_->updateHasMeshFlag();
    updateNotScaledFlag();
}

bool W4dEntity::visible() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();

    //Check for a visibility plan
    if( pPlan_ != NULL  and  pPlan_->hasVisibilityPlan() )
    {
        //Only update once at any given time
        W4dManager& manager = W4dManager::instance();
        PhysAbsoluteTime timeNow = manager.time();
        if( pPlanUpdateTimes_->visibilityTime != timeNow )
        {
            //Update local visibility flag from plan, caching update time
            bool visibleNow;
            uint nObsoletePlans;
            if( pPlan_->visible( timeNow, &visibleNow, &nObsoletePlans ) == W4dEntityPlan::DEFINED )
                pImpl_->visible_ = visibleNow;

            pPlanUpdateTimes_->visibilityTime = timeNow;

            //Expunge any obsolete visibility plans
            if( nObsoletePlans != 0  and  not manager.hasArtificialTime() )
                pPlan_->clearVisibilityPlans( nObsoletePlans );
        }
    }

    return pImpl_->visible_;
}

void W4dEntity::visible( bool vis )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    pImpl_->visible_ = vis;
}

void    W4dEntity::addMaterials( RenMaterialSet* pSet ) const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    for( W4dEntityImpl::Meshes::const_iterator i = meshes_->begin(); i != meshes_->end(); ++i )
    {
        if( (*i).mesh )
            (*i).mesh->addMaterials( pSet );
    }
}

// virtual
void    W4dEntity::materialMap( const RenMaterialMap& map )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    for( W4dEntityImpl::Meshes::const_iterator i = meshes_->begin(); i != meshes_->end(); ++i )
    {
        if( (*i).mesh )
            (*i).mesh->materialMap( map );
    }
}

void W4dEntity::boundingVolume( const MexAlignedBox3d& newBoundingVolume )
{
        CB_W4dEntity_DEPIMPL();
        PRE( boundingVolume_ == NULL );
        boundingVolume_ = _NEW( MexAlignedBox3d( newBoundingVolume ) );
}

const MexAlignedBox3d& W4dEntity::boundingVolume() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();

    //Only entities with a mesh have any scale
    const MexAlignedBox3d* pResult;

    if( boundingVolume_ == NULL  and  pImpl_->notScaled_  and  hasMesh() )
    {
        //Use the mesh's own local scale
        pResult = &(mesh().mesh()->boundingVolume());
    }
    else
    {
        //Ensure we have a local copy of the scaled BV
        if( boundingVolume_ == NULL )
            _CONST_CAST( W4dEntity*, this )->updateBoundingVolume();

        pResult = boundingVolume_;
        ASSERT( boundingVolume_ != NULL, "" )
    }

    return *pResult;
}

void W4dEntity::CLASS_INVARIANT
{
        CB_W4dEntity_DEPIMPL();
    INVARIANT_THIS_EXISTS;
        INVARIANT(pImpl_);
        INVARIANT(meshes_);
        INVARIANT( (pPlan_ == NULL and pPlanUpdateTimes_ == NULL) or
                   (pPlan_ != NULL and pPlanUpdateTimes_ != NULL));
		INVARIANT( hasMesh() == hasMesh( 0 ) );
}

bool W4dEntity::hasSubject() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    return pSubject_ != NULL;
}

W4dSubject& W4dEntity::subject()
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( hasSubject() );
    return *pSubject_;
}

const W4dSubject& W4dEntity::subject() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( hasSubject() );
    return *pSubject_;
}

void W4dEntity::subject( W4dSubject* theSubject )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    pSubject_ = theSubject;
}

void W4dEntity::relativeTransform( const W4dEntity& other, W4dTransform3d* pTransform ) const
{
    TEST_INVARIANT;
    //Get the global transform of other and invert it
    W4dTransform3d otherInverseGlobalTransform( other.globalTransform() );
    otherInverseGlobalTransform.invert();

    //Hence compute the relative position
    otherInverseGlobalTransform.transform( globalTransform(), pTransform );
}

bool W4dEntity::defaultIntersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                   Accuracy  ) const
{
    TEST_INVARIANT;
    return intersectsBoundingVolume( line, pDistance );
}

bool W4dEntity::intersectsBoundingVolume( const MexLine3d& line, MATHEX_SCALAR* pDistance ) const
{
    TEST_INVARIANT;

    //Get the global transform, and invert it so that we can produce a copy
    //of the line transformed into the local coordinate system this entity.
    MexTransform3d inverseGlobal;
    globalTransform().invert( &inverseGlobal );
    MexLine3d localLine( line );
    localLine.transform( inverseGlobal );

    //Intersect with the bounding volume
    MATHEX_SCALAR entryDistance, exitDistance;
    bool result = boundingVolume().intersects( localLine, &entryDistance, &exitDistance );

    if( result )
    {
        //If intersects after the line start point, use the entry distance, otherwise return
        //zero indicating that the start point is inside the volume
        *pDistance = (entryDistance > 0.0 ? entryDistance : 0.0);
    }

    return result;
}

//static
ulong W4dEntity::nextCheckId()
{
    static ulong nextId = 0;
    return ++nextId;
}

ulong W4dEntity::checkId() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    return checkId_;
}

void W4dEntity::checkId( ulong id )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    checkId_ = id;
}

W4dId W4dEntity::id() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    return id_;
}

void W4dEntity::id( W4dId id )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    id_ = id;
}

void W4dEntity::stationary( bool newFlag )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    pImpl_->isStationary_ = newFlag;
}

bool W4dEntity::stationary() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    return pImpl_->isStationary_;
}

W4dEntity::Solidity W4dEntity::solid() const
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    return pImpl_->solid_;
}

uint W4dEntity::updateLODMaterials( const PhysAbsoluteTime timeNow, W4dLOD exceptLodId )
{
        CB_W4dEntity_DEPIMPL();
    PRE( pPlan_ != NULL and pPlan_->hasMaterialPlan() );

    //Material plans now regarded as temporary, so we never want to do this
    ASSERT( false, "" );

    //Iterate through the levels of detail
    W4dLOD lodId = 0;
    uint nObsoletePlans;
    Ren::MaterialVecPtr materialVecPtr;

        for( W4dEntityImpl::Meshes::const_iterator it = meshes_->begin(); it != meshes_->end(); ++it, ++lodId )
    {
        //Skip excepted lod
        if( lodId != exceptLodId )
        {
            RenMeshInstance* pMesh = (*it).mesh;

            if( pPlan_->materialVec( timeNow, lodId, &materialVecPtr, &nObsoletePlans ) ==
                W4dEntityPlan::DEFINED )
            {
                pMesh->materialVec( materialVecPtr );
            }
        }
    }

    return nObsoletePlans;
}

void W4dEntity::updateLODUVTransforms
(
    const Ren::UVTransformPtr& uvTransformPtr, W4dLOD maxLOD, W4dLOD exceptId
)
{
        CB_W4dEntity_DEPIMPL();
    //Iterate through the levels of detail
    W4dLOD lodId = 0;

        for( W4dEntityImpl::Meshes::const_iterator it = meshes_->begin();
             it != meshes_->end()  and  lodId <= maxLOD; ++it, ++lodId )
    {
        if( lodId != exceptId )
        {
            RenMeshInstance* pMesh = (*it).mesh;
            pMesh->animateTextureCoords( uvTransformPtr );
        }
    }
}

W4dEntityPlan& W4dEntity::entityPlanForEdit()
{
        CB_W4dEntity_DEPIMPL();
    //If we have any existing plans, update them causing obsolete items to be cleared
    if( pPlan_ != NULL )
    {
        //Only clear if we don't have an artificial time
        W4dManager& manager = W4dManager::instance();
        if( not manager.hasArtificialTime() )
        {
            //get current time
            PhysAbsoluteTime timeNow = manager.time();

            //Transform plans
            localTransform();

            //Visibility plans
            visible();

            //Scale plans
            if( hasTemporaryScale() )
                temporaryScale();

            //material plans
            if( pPlan_->hasMaterialPlan() )
            {
                //Material plans now regarded as temporary, so we don't permanently update the
                //meshes
                //W4dLOD dummyLodId = -1;
                //uint nObsoletePlans = updateLODMaterials( timeNow, dummyLodId );
                //if( nObsoletePlans != 0 )
                //    pPlan_->clearMaterialPlans( nObsoletePlans );
                pPlan_->clearMaterialPlansAtTime( timeNow );
            }

            //uv plans
            if( pPlan_->hasUVPlan() )
            {
                uint nObsoletePlans = 0;
                Ren::UVTransformPtr uvTransformPtr;
                W4dLOD maxLOD;
                if( pPlan_->uvTransform( timeNow, W4dLOD( 0 ), &uvTransformPtr, &maxLOD, &nObsoletePlans)
                    == W4dEntityPlan::DEFINED )
                {
                    updateLODUVTransforms( uvTransformPtr, maxLOD, W4dLOD( 0 ) );
                }

                if( nObsoletePlans != 0 )
                   pPlan_->clearUVPlans( nObsoletePlans );
            }

            //Mesh plans
            if( pPlan_->hasMeshPlan() )
                pPlan_->clearMeshPlansAtTime( timeNow );
        }
    }

    //Construct (if necessary) and return the entity plan object
    return plan();
}

void W4dEntity::lightList(W4dLocalLightList* pList)
{
        pImpl_->lightList( pList );
}

void W4dEntity::updateNotScaledFlag()
{
        CB_W4dEntity_DEPIMPL();
    //Check for a scaled mesh or a local mesh scale attribute
    pImpl_->notScaled_ = (not hasMesh()  or  mesh().scale().isUnity()) and
                 (not pEntityScale_  or  not pEntityScale_->hasMeshScale());
}

//virtual
void W4dEntity::temporaryScale( const RenScale& scale, PropogateScaleFlag propogate )
{
        CB_W4dEntity_DEPIMPL();
    //Check scale type
    if( scale.isUnity() or (scale.isUniform() and scale.asUniform().factor() == 1.0) )
    {
        //Setting scale to untiy, which is same as clearing
        clearTemporaryScale( propogate );
    }
    else
    {
        //Set local mesh scale for this entity.
        entityScale().meshScale( scale );
		updateNotScaledFlag();

        //Deal with any propogation through descendants
        if( propogate and childList_ != NULL )
        {
            const RenNonUniformScale* pNonUniformScale =
                ( scale.isNonUniform() ? &scale.asNonUniform() : NULL );

            for( W4dEntities::iterator it = childList_->begin(); it != childList_->end(); ++it )
            {
                W4dEntity& child = *(*it);

                //Make child inherit transform position scale from this entity
                child.scaleTransformFromParent( true );

                //Apply this scale recursively
                if( pNonUniformScale )
                {
                    //Use special recursive function to propogate non-uniform scaling
                    child.temporaryScale( *pNonUniformScale, child.localTransform() );
                }
                else
                {
                    child.temporaryScale( scale, propogate );
                }
            }
        }
    }
}

//virtual
void W4dEntity::temporaryScale
(
    const RenNonUniformScale& scale, const MexTransform3d& useTransform
)
{
        CB_W4dEntity_DEPIMPL();
    //Produce a modified scale from the transform
    RenNonUniformScale myScale( scale, useTransform );

    //Set local mesh scale for this entity.
    entityScale().meshScale( myScale );
	updateNotScaledFlag();

    //Visit all children
    if( childList_ != NULL )
    {
        for( W4dEntities::iterator it = childList_->begin(); it != childList_->end(); ++it )
        {
            W4dEntity& child = *(*it);

            //Make child inherit transform position scale from this entity
            child.scaleTransformFromParent( true );

            //Combine the supplied transform with the local transform of this child
            MexTransform3d childUseTransform;
            useTransform.transform( child.localTransform(), &childUseTransform );

            //Make recursive call
            child.temporaryScale( scale, childUseTransform );
        }
    }
}

W4dEntityScale& W4dEntity::entityScale()
{
        CB_W4dEntity_DEPIMPL();
    //Ensure constructed
    if( pEntityScale_ == NULL )
        pEntityScale_ = _NEW( W4dEntityScale() );

    return * pEntityScale_;
}

//virtual
void W4dEntity::clearTemporaryScale( PropogateScaleFlag propogate )
{
        CB_W4dEntity_DEPIMPL();
    //Check have a scale
    if( pEntityScale_ )
    {
        //Update the local transform from unscaled version if applicable.
        if( pEntityScale_->scaleTransformFromParent() )
            localTransform_.position( pEntityScale_->unscaledTransformPosition() );

        //Ensure the global transforms get updated
        markGlobalTransformsDirty();

        //Delete the scale data
        _DELETE( pEntityScale_ );
        pEntityScale_ = NULL;

        //Update the not scaled flag
        updateNotScaledFlag();
    }

    //Clear any children if propogating
    if( propogate  and  childList_ != NULL )
    {
        for( W4dEntities::iterator it = childList_->begin(); it != childList_->end(); ++it )
            (*it)->clearTemporaryScale( propogate );
    }
}

bool W4dEntity::hasTemporaryScale() const
{
        CB_W4dEntity_DEPIMPL();
    return (pEntityScale_ and pEntityScale_->hasMeshScale()) or
           (pPlan_ != NULL and pPlan_->hasScalePlan());
}

const RenScale& W4dEntity::temporaryScale() const
{
        CB_W4dEntity_DEPIMPL();
    PRE( hasTemporaryScale() );

    //Ensure we have the entityScale allocated.
    W4dEntityScale& myEntityScale = _CONST_CAST( W4dEntity* , this )->entityScale();

    //Check for a scale plan
    if( pPlan_ != NULL  and  pPlan_->hasScalePlan() )
    {
        //Only update once at any given time
        W4dManager& manager = W4dManager::instance();
        PhysAbsoluteTime timeNow = manager.time();
        if( pPlanUpdateTimes_->scaleTime != timeNow )
        {
            //Update entity scale from plan, caching update time
            uint nObsoletePlans;
            pPlanUpdateTimes_->scaleTime = timeNow;
            if( pPlan_->scale( timeNow, &myEntityScale, &nObsoletePlans ) !=
                W4dEntityPlan::DEFINED )
            {
                //Ensure we have a scale defined. If none, allocate a unity scale.
                if( not myEntityScale.hasMeshScale() )
                    myEntityScale.meshScale( RenUniformScale( 1.0 ) );
            }

            //Expunge any obsolete scale plans
            if( nObsoletePlans != 0  and  not manager.hasArtificialTime() )
                pPlan_->clearScalePlans( nObsoletePlans );
        }
    }

    return pEntityScale_->meshScale();
}

void W4dEntity::scaleTransformFromParent( bool doScale )
{
        CB_W4dEntity_DEPIMPL();
    if( doScale  or  pEntityScale_ != NULL )
    {
        //Get the entity scale object, and checking if we're already scaling transform
        W4dEntityScale& myEntityScale = entityScale();
        bool wasScalingFromParent = myEntityScale.scaleTransformFromParent();

        //If removing scaling, restore local transform
        if( not doScale and wasScalingFromParent )
            localTransform_.position( myEntityScale.unscaledTransformPosition() );

        //Set the flag
        myEntityScale.scaleTransformFromParent( doScale );

        //If scaling for first time, ensure current transform position is saved
        if( doScale  and  not wasScalingFromParent )
            pEntityScale_->unscaledTransformPosition( localTransform_.position() );
    }
}

bool W4dEntity::scaleTransformFromParent() const
{
        CB_W4dEntity_DEPIMPL();
    return pEntityScale_ != NULL and pEntityScale_->scaleTransformFromParent();
}

void W4dEntity::scaleTransformPosition()
{
        CB_W4dEntity_DEPIMPL();
    PRE( scaleTransformFromParent() );

    //Get the parent's mesh scale
    W4dEntity& myParent = *pParent();
    if( myParent.hasTemporaryScale() )
    {
        //Get the unscaled position
        MexPoint3d p = pEntityScale_->unscaledTransformPosition();

        //Scale it
        myParent.temporaryScale().apply( &p );

        //Save it
        localTransform_.position( p );
    }
}

//virtual
void W4dEntity::propogateScalePlan( const W4dScalePlanPtr& planPtr,
                                     const PhysAbsoluteTime& startTime,
                                     uint repetitions, uint animId )
{
        CB_W4dEntity_DEPIMPL();
    //Set plan for this entity.
    entityPlanForEdit().scalePlan( planPtr, startTime, repetitions, animId );

    //Deal with any propogation through descendants
    bool isNonUniform = planPtr->isNonUniform();

    if( childList_ != NULL )
    {
        for( W4dEntities::iterator it = childList_->begin(); it != childList_->end(); ++it )
        {
            W4dEntity& child = *(*it);

            //Make child inherit transform position scale from this entity
            child.scaleTransformFromParent( true );

            //Apply the plan recursively
            if( isNonUniform )
            {
                //Use special recursive function to propogate non-uniform scaling
                child.propogateScalePlan( planPtr, startTime, repetitions, animId,
                                          child.localTransform() );
            }
            else
            {
                child.propogateScalePlan( planPtr, startTime, repetitions, animId );
            }
        }
    }
}

//virtual
void W4dEntity::propogateScalePlan
(
    const W4dScalePlanPtr& planPtr, const PhysAbsoluteTime& startTime,
    uint repetitions, uint animId, const MexTransform3d& offsetTransform
)
{
        CB_W4dEntity_DEPIMPL();
    PRE( planPtr->isNonUniform() );

    //Construct new plan for this entity and set it.
    W4dScalePlanPtr myPlanPtr( planPtr->transformClone( offsetTransform ) );
    entityPlanForEdit().scalePlan( myPlanPtr, startTime, repetitions, animId );

    //Deal with propogation through descendants
    if( childList_ != NULL )
    {
        for( W4dEntities::iterator it = childList_->begin(); it != childList_->end(); ++it )
        {
            W4dEntity& child = *(*it);

            //Combine the supplied transform with the local transform of this child
            child.scaleTransformFromParent( false );
            MexTransform3d childUseTransform;
            offsetTransform.transform( child.localTransform(), &childUseTransform );

            //Make child inherit transform position scale from this entity
            child.scaleTransformFromParent( true );

            //Use special recursive function to propogate non-uniform scaling
            child.propogateScalePlan( planPtr, startTime, repetitions, animId, childUseTransform );
        }
    }
}

void W4dEntity::updateBoundingVolume()
{
        CB_W4dEntity_DEPIMPL();
    if( hasMesh() )
    {
        //Check for any scaling applied to top level mesh
        if( pImpl_->notScaled_ )
        {
            //Ensure no local copy since no scaling
            if( boundingVolume_ != NULL )
            {
                _DELETE( boundingVolume_ );
                boundingVolume_ = NULL;
            }
        }
        else
        {
            //Get the mesh's unscaled boundary
            const MexAlignedBox3d& meshUnscaledBoundary = mesh().mesh()->boundingVolume();

            //Get the unscaled corners
            MexPoint3d minCorner = meshUnscaledBoundary.minCorner();
            MexPoint3d maxCorner = meshUnscaledBoundary.maxCorner();

            //Apply any mesh instance scale
            const RenScale& meshInstScale = mesh().scale();
            meshInstScale.apply( &minCorner );
            meshInstScale.apply( &maxCorner );

            //Apply any temporary scale override
            if( hasTemporaryScale() )
            {
                const RenScale& tempScale = temporaryScale();
                tempScale.apply( &minCorner );
                tempScale.apply( &maxCorner );
            }

            //Use the scaled corners to set the boundary
            if( boundingVolume_ == NULL )
                boundingVolume_ = _NEW( MexAlignedBox3d( minCorner, maxCorner ) );
            else
                boundingVolume_->corners( minCorner, maxCorner );

        }
    }
    else
    {
        //Ensure we have a dummy bounding volume
        if( boundingVolume_ == NULL )
            boundingVolume_ = _NEW( MexAlignedBox3d( MexPoint3d(0,0,0), MexPoint3d(0,0,0) ) );
        else
            boundingVolume_->corners( MexPoint3d(0,0,0), MexPoint3d(0,0,0) );
    }
}

void W4dEntity::clearMeshes()
{
        CB_W4dEntity_DEPIMPL();
    while( meshes_->size() != 0 )
    {
        _DELETE( meshes_->back().mesh );
        meshes_->pop_back();
    }
	pImpl_->updateHasMeshFlag();
}

bool W4dEntity::isComposite() const
{
    return pImpl_->isComposite();
}

W4dComposite& W4dEntity::asComposite()
{
    PRE( isComposite() );
    return *(_STATIC_CAST( W4dComposite*, this ));
}

const W4dComposite& W4dEntity::asComposite() const
{
    PRE( isComposite() );
    //return *(_STATIC_CAST( W4dComposite*, this ));
    return  *(_STATIC_CAST( const W4dComposite*, this ));
}

void W4dEntity::isComposite( bool isIt )
{
    pImpl_->isComposite( isIt );
}

void W4dEntity::isGarbage( bool setIsGarbage )
{
    PRE( not setIsGarbage  or  not isOwnedByCountedPtr() );

    CB_W4dEntity_DEPIMPL();
    pImpl_->isGarbage_ = setIsGarbage;
}

bool W4dEntity::isGarbage() const
{
        CB_W4dEntity_DEPIMPL();
    return pImpl_->isGarbage_;
}

bool W4dEntity::hasContainingDomain( W4dDomain** ppDomain ) const
{
    TEST_INVARIANT;

    W4dEntity*  pNode = _CONST_CAST( W4dEntity*, this);
    bool found = false;

    while( not found  and  pNode->hasParent() )
    {
        pNode = pNode->pParent();

        if( pNode->isDomain() )
        {
            *ppDomain = pNode->getAsDomain();
            found = true;
        }
    }

    return found;
}

uint W4dEntity::nLODs() const
{
        CB_W4dEntity_DEPIMPL();
    return (meshes_ ? meshes_->size() : 0);
}

RenMeshInstance& W4dEntity::mesh( W4dLOD id )
{
    TEST_INVARIANT;
        CB_W4dEntity_DEPIMPL();
    PRE( hasMesh( id ) );
    return *((*meshes_)[id].mesh);
}

void perWrite( PerOstream& ostr, const W4dEntity& entity )
{
    ostr << entity.pImpl_;
}

void perRead( PerIstream& istr, W4dEntity& entity )
{
    //_DELETE( entity.pImpl_ ); //> no idea why this deleted area is referenced on models load
    istr >> entity.pImpl_;

    //  This call must be made here to make sure that the entity plan
    //  update times are properly set up
    //JLG commented this out 22/2/99. It is exceedingly wasteful of memory, since many
    //objects never have plans associated with them, and this call forces a W4dEntityPlan
    //and a PlanUpdateTimes to be created unnecessarily. I cannot see why the call
    //is needed, but if it causes problems DO NOT comment the lines back in. Talk to me
    //and we'll find another solution.
    //entity.plan();

    //Ensure any intrinsic animations are applied
	if (!entity.isComposite() && entity.pImpl_->hasAnimationData())
	{
	    entity.pImpl_->applyAllAnimations(&entity, W4dManager::instance().time());
	}
}

W4dEntity::W4dEntity( PerConstructor )
: pImpl_( NULL )
{
}

bool W4dEntity::hasLightList() const
{
    return pImpl_->hasLightList();
}

const W4dLocalLightList& W4dEntity::lightList() const
{
        return pImpl_->lightList();
}

W4dLocalLightList& W4dEntity::lightListForEdit()
{
		return pImpl_->lightListForEdit();
}

void W4dEntity::markGlobalTransformsDirty()
{
        CB_W4dEntity_DEPIMPL();
    //reset the cached transform time
    timeGlobalTransformLastUpdated_ = -1.0;

    //Call recursively for all children
    if( childList_ != NULL )
    {
        W4dEntities& kids = *childList_;
        for( W4dEntities::iterator it = kids.begin(); it != kids.end(); ++it)
        {
            (*it)->markGlobalTransformsDirty();
        }
    }

    //If this is a composite, do special processing
    if( isComposite() )
    {
        //Get the links
        const W4dComposite::W4dLinks& links = asComposite().links();

        //Mark them all dirty
        for( W4dComposite::W4dLinks::const_iterator it = links.begin(); it != links.end(); ++it )
        {
            (*it)->markGlobalTransformsDirty();
        }
    }
}

W4dEntity::W4dEntities* W4dEntity::childListForEdit()
{
        CB_W4dEntity_DEPIMPL();
    if( childList_ == NULL )
        childList_ = _NEW( W4dEntities );

    return childList_;
}

void W4dEntity::solid( Solidity newSolidity )
{
        CB_W4dEntity_DEPIMPL();
    pImpl_->solid_ = newSolidity;
}

void W4dEntity::parseCycleTexture(UtlLineTokeniser* pParser )
{
    TEST_INVARIANT;
    CB_W4dEntity_DEPIMPL();
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "CYCLE_TEXTURE", "" );
    ASSERT( pParser->tokens().size() == 2, "" );

    size_t nAnimTexture = atoi( pParser->tokens()[ 1 ].c_str() );

    W4dCycleMultiTextureData* pMultiTextureData = _NEW(W4dCycleMultiTextureData( nAnimTexture ) );

    pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        ASSERT( pParser->tokens().size() == 7, "" );
        ASSERT( pParser->tokens()[ 3 ] == "FRAMES", "" );
        ASSERT( pParser->tokens()[ 5 ] == "MAX_LOD", "" );

        string textureName  = pParser->tokens()[ 0 ];
        size_t startTexture = atoi( pParser->tokens()[ 1 ].c_str() );
        size_t endTexture   = atoi( pParser->tokens()[ 2 ].c_str() );
        size_t nrepetations = atoi( pParser->tokens()[ 4 ].c_str() );
        W4dLOD maxLod       = atoi( pParser->tokens()[ 6 ].c_str() );

        W4dCycleTextureData* pCycleTextureData = _NEW(W4dCycleTextureData(textureName,
                                                                       startTexture,
                                                                       endTexture,
                                                                       nrepetations) );

        pMultiTextureData->add( pCycleTextureData );
        pParser->parseNextLine();
    }

    pImpl_->addAnimationData( pMultiTextureData );
    pParser->parseNextLine();
}

void W4dEntity::parseUVTranslate( UtlLineTokeniser* pParser )
{
    TEST_INVARIANT;
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "UV_TRANSLATE", "" );
    ASSERT( pParser->tokens()[ 4 ] == "MAX_LOD", "" );
    ASSERT( pParser->tokens().size() == 6, "" );

    string textureName = pParser->tokens()[ 1 ];
        MATHEX_SCALAR xSpeed = atof( pParser->tokens()[ 2 ].c_str() );
        MATHEX_SCALAR ySpeed = atof( pParser->tokens()[ 3 ].c_str() );
    W4dLOD maxLod        = atoi( pParser->tokens()[ 5 ].c_str() );

    MexVec2 speed(xSpeed, ySpeed );

    W4dUVTranslateData* pUVTranslateData = _NEW( W4dUVTranslateData(textureName, speed, maxLod ) );

    pImpl_->addAnimationData( pUVTranslateData );

    pParser->parseNextLine();
}

void W4dEntity::parseColourPulse( UtlLineTokeniser* pParser )
{
    TEST_INVARIANT;
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "COLOUR_PULSE", "" );
    ASSERT( pParser->tokens().size() == 2, "" );

    size_t nAnimTexture = atoi( pParser->tokens()[ 1 ].c_str() );

        W4dMultiColourPulseData* pMultiColourPulseData = _NEW(W4dMultiColourPulseData( nAnimTexture ) );

    pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
        ASSERT( pParser->tokens()[ 1 ] == "FROM", "" );
        ASSERT( pParser->tokens()[ 5 ] == "TO", "" );
        ASSERT( pParser->tokens()[ 9 ] == "EVERY", "" );
        ASSERT( pParser->tokens()[ 11 ] == "MAX_LOD", "" );
        ASSERT( pParser->tokens().size() == 13, "" );

        string textureName = pParser->tokens()[ 0 ];

        MATHEX_SCALAR rFrom = atof( pParser->tokens()[ 2 ].c_str() );
        MATHEX_SCALAR gFrom = atof( pParser->tokens()[ 3 ].c_str() );
        MATHEX_SCALAR bFrom = atof( pParser->tokens()[ 4 ].c_str() );

        MATHEX_SCALAR rTo = atof( pParser->tokens()[ 6 ].c_str() );
        MATHEX_SCALAR gTo = atof( pParser->tokens()[ 7 ].c_str() );
        MATHEX_SCALAR bTo = atof( pParser->tokens()[ 8 ].c_str() );

        MATHEX_SCALAR duration = atof( pParser->tokens()[ 10 ].c_str() );
        W4dLOD maxLod          = atoi( pParser->tokens()[ 12 ].c_str() );

        RenColour from(rFrom/255.0, gFrom/255.0, bFrom/255.0);
        RenColour to(rTo/255.0, gTo/255.0, bTo/255.0);

        W4dColourPulseData* pColourPulseData = _NEW( W4dColourPulseData(textureName, from, to, duration ) );

        pMultiColourPulseData->add( pColourPulseData );
        pParser->parseNextLine();
    }

    pImpl_->addAnimationData( pMultiColourPulseData );
    pParser->parseNextLine();
}

void W4dEntity::parseAnimationData( UtlLineTokeniser* pParser )
{
        ASSERT( pParser->tokens()[ 0 ] == "ANIMATION_DATA", "");

    pParser->parseNextLine();

    while( pParser->tokens()[ 0 ] != "END" )
    {
                if( pParser->tokens()[ 0 ] == "CYCLE_TEXTURE" )
                {
                        parseCycleTexture(pParser );
                }
                else if( pParser->tokens()[ 0 ] == "UV_TRANSLATE" )
                {
                        parseUVTranslate( pParser );
                }

                else if ( pParser->tokens()[ 0 ] == "COLOUR_PULSE" )
                {
                        parseColourPulse( pParser );
                }
                else if ( pParser->tokens()[ 0 ] == "GENERIC_LIGHT" )
                {
                        parseGenericLight( pParser );
                }
                else
                {
                    pParser->parseNextLine();
                }
        }

    pParser->parseNextLine();
}

W4dEntityImpl& W4dEntity::entityImpl()
{
        CB_W4dEntity_DEPIMPL();
        PRE( pImpl_ != NULL )

        return *pImpl_;
}

const W4dEntityImpl& W4dEntity::entityImpl() const
{
        CB_W4dEntity_DEPIMPL();
        PRE( pImpl_ != NULL )

        return *pImpl_;
}

void W4dEntity::filterColour( const RenColour& colour )
{
        CB_W4dEntity_DEPIMPL();
        pImpl_->filterColour( colour );

        POST( filterColourIsSet() );
}

const RenColour& W4dEntity::filterColour() const
{
        CB_W4dEntity_DEPIMPL();
        PRE( filterColourIsSet() );

        return pImpl_->filterColour();
}

bool W4dEntity::filterColourIsSet() const
{
        CB_W4dEntity_DEPIMPL();
        return pImpl_->filterColourIsSet();
}

void W4dEntity::incrementLinkedSoundCount() const
{
        CB_W4dEntity_DEPIMPL();
    ++pImpl_->nLinkedSounds_;
}

void W4dEntity::decrementLinkedSoundCount() const
{
	--pImpl_->nLinkedSounds_;
}

void W4dEntity::parseGenericLight( UtlLineTokeniser* pParser )
{
    TEST_INVARIANT;
    ASSERT_INFO( pParser->tokens()[ 0 ] );
    ASSERT( pParser->tokens()[ 0 ] == "GENERIC_LIGHT", "" );
    pParser->parseNextLine();

    W4dLightData* pLightData = _NEW( W4dLightData("",  0 ) );

    bool type, scope, dummyMesh, position, colour, range, attenuations, direction, uniform, point;

    bool timesDefined = false;
    bool intsDefined = false;

    size_t nTimes, nIntensities;

    while( pParser->tokens()[ 0 ] != "END" )
    {
                if( pParser->tokens()[ 0 ] == "DUMMY_MESH" )
                {
                    ASSERT( this->isComposite(), "only a composite can have light dummyMesh" );
                        dummyMesh = true;

                    ASSERT( pParser->tokens().size() == 2, "" );
                        string dummyMeshName = pParser->tokens()[ 1 ];

                        pLightData->name( dummyMeshName );
                        pLightData->composite( &(this->asComposite()) );
                }
                else if( pParser->tokens()[ 0 ] == "POSITION" )
                {
                        position = true;

                    ASSERT( pParser->tokens().size() == 4, "" );
                        MATHEX_SCALAR x = atof( pParser->tokens()[ 1 ].c_str() );
                        MATHEX_SCALAR y = atof( pParser->tokens()[ 2 ].c_str() );
                        MATHEX_SCALAR z = atof( pParser->tokens()[ 3 ].c_str() );

                        pLightData->position( MexPoint3d(x, y, z) );
                }

                else if ( pParser->tokens()[ 0 ] == "TYPE" )
                {
                        type = true;

                    ASSERT( pParser->tokens().size() == 2, "" );

                        string lightType = pParser->tokens()[ 1 ];
                        ASSERT( lightType == "DIRECTIONAL" or lightType == "POINT" or lightType == "UNIFORM", "illigal type specified");

                        if( lightType == "DIRECTIONAL" )
                                pLightData->type( W4dLightData::DIRECTIONAL);

                        if( lightType == "POINT" )
                        {
                                point = true;
                                pLightData->type( W4dLightData::POINT);
                        }

                        if( lightType == "UNIFORM" )
                        {
                                uniform = true;
                                pLightData->type( W4dLightData::UNIFORM);
                        }
                }
                else if ( pParser->tokens()[ 0 ] == "SCOPE" )
                {
                        scope = true;

                    ASSERT( pParser->tokens().size() == 2, "" );

                        string lightScope = pParser->tokens()[ 1 ];
                        ASSERT( lightScope == "LOCAL_LIGHT" or
                                lightScope == "GLOBAL_LIGHT" or
                                lightScope == "DOMAIN_LIGHT" or
                                lightScope == "DOMAIN_COMPOSITE" or
                                lightScope == "DYNAMIC_LIGHT", "illigal light scope specified");

                        if( lightScope == "LOCAL_LIGHT" )
                                pLightData->scope( W4dLightData::LOCAL_LIGHT );
                        else if( lightScope == "GLOBAL_LIGHT" )
                                pLightData->scope( W4dLightData::GLOBAL_LIGHT );
                        else if( lightScope == "DOMAIN_LIGHT" )
                                pLightData->scope( W4dLightData::DOMAIN_LIGHT );
                        else if( lightScope == "DOMAIN_COMPOSITE" )
                                pLightData->scope( W4dLightData::DOMAIN_COMPOSITE );
                        else if( lightScope == "DYNAMIC_LIGHT" )
                                pLightData->scope( W4dLightData::DYNAMIC_LIGHT );
                }

                else if( pParser->tokens()[ 0 ] == "ATTENUATIONS" )
                {
                        attenuations = true;

                    ASSERT( pParser->tokens().size() == 4, "" );
                        MATHEX_SCALAR a1 = atof( pParser->tokens()[ 1 ].c_str() );
                        MATHEX_SCALAR a2 = atof( pParser->tokens()[ 2 ].c_str() );
                        MATHEX_SCALAR a3 = atof( pParser->tokens()[ 3 ].c_str() );

                        pLightData->attenuations( a1, a2, a3 );
                }

                else if( pParser->tokens()[ 0 ] == "COLOUR" )
                {
                        colour = true;

                    ASSERT( pParser->tokens().size() == 4, "" );
                        MATHEX_SCALAR r = atof( pParser->tokens()[ 1 ].c_str() );
                        MATHEX_SCALAR g = atof( pParser->tokens()[ 2 ].c_str() );
                        MATHEX_SCALAR b = atof( pParser->tokens()[ 3 ].c_str() );

                        pLightData->colour( RenColour(r, g, b) );
                }
                else if( pParser->tokens()[ 0 ] == "RANGE" )
                {
                        range = true;

                    ASSERT( pParser->tokens().size() == 2, "" );
                        MATHEX_SCALAR maxRange = atof( pParser->tokens()[ 1 ].c_str() );

                        pLightData->maxRange( maxRange );
                }
                else if( pParser->tokens()[ 0 ] == "DIRECTION" )
                {
                        direction = true;

                    ASSERT( pParser->tokens().size() == 4, "" );
                        MATHEX_SCALAR x = atof( pParser->tokens()[ 1 ].c_str() );
                        MATHEX_SCALAR y = atof( pParser->tokens()[ 2 ].c_str() );
                        MATHEX_SCALAR z = atof( pParser->tokens()[ 3 ].c_str() );

                        pLightData->direction( MexVec3(x, y, z) );
                }
                else if( pParser->tokens()[ 0 ] == "TIMES" )
                {
                        timesDefined = true;
                        nTimes = pParser->tokens().size();
                        nTimes -= 1;

                        ctl_vector< MATHEX_SCALAR > intervals;
                        intervals.reserve( nTimes );

                        for( size_t i=0; i<nTimes; ++i)
                        {
                                intervals.push_back( atof( pParser->tokens()[ i+1 ].c_str() ) );
                        }

                        pLightData->times( intervals );
                }
                else if( pParser->tokens()[ 0 ] == "INTENSITIES" )
                {
                        intsDefined = true;
                        nIntensities = pParser->tokens().size();
                        nIntensities -= 1;

                        ctl_vector< MATHEX_SCALAR > intensities;
                        intensities.reserve( nIntensities );

                        for( size_t i=0; i<nIntensities; ++i)
                        {
                                intensities.push_back( atof( pParser->tokens()[ i+1 ].c_str() ) );
                        }

                        pLightData->intensities( intensities );
                }
                else
                {
                    pParser->parseNextLine();
                }

            pParser->parseNextLine();
        }

        ASSERT( type and scope and colour and direction and (dummyMesh or position), "basic elements missing");
        ASSERT( (uniform or point) and attenuations and range, " attenuations/range not specified"  );
        ASSERT_INFO( nTimes );
        ASSERT_INFO( nIntensities );

        ASSERT_INFO( timesDefined );
        ASSERT_INFO( intsDefined );
        bool result = not (timesDefined ^ intsDefined );
        ASSERT_INFO( result );
        ASSERT( not (timesDefined ^ intsDefined ), " TIMES or INTENSITIES missing " );

    pImpl_->addLightAnimationData( pLightData );
    pParser->parseNextLine();
}

void W4dEntity::doNotLight(bool lightIt)
{
        pImpl_->doNotLight( lightIt );
}

bool W4dEntity::doNotLight() const
{
        return pImpl_->doNotLight();
}

// Entity names aren't always usefully set so they don't work too well for
// identifying entities.  In addition to the name, try to print some useful
// info about an entity.
ostream& operator<<(ostream& s, const W4dEntity* entity)
{
        if (!entity)
        {
                s << "null entity ptr";
                return s;
        }

        if (entity->isDomain())
                s << "domain";
        else if (entity->isComposite())
                s << "composite";
        else
                s << "entity";

		s << " (" << (void*) entity << ")";

        if (entity->name().length() > 0)
                s << " " << entity->name();

        s << " at " << entity->globalTransform().position();
        return s;
}

void W4dEntity::nowOwnedByCountedPtr()
{
    pImpl_->isOwnedByCountedPtr_ = true;
}

bool W4dEntity::isOwnedByCountedPtr() const
{
    return pImpl_->isOwnedByCountedPtr_;
}

void W4dEntity::clientData(int newValue)
{
    pImpl_->clientData( newValue );
}

int W4dEntity::clientData() const
{
	return pImpl_->clientData();
}

#pragma GCC diagnostic pop

/* End ENTITY.CPP *************************************************/
