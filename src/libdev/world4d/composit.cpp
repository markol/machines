/*
 * C O M P O S I T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/composit.hpp"

// Note: unfortunately there are #include order dependencies due to forward
// declarations of templates.  These includes must come before compplan.hpp.
#include "phys/linemoti.hpp"		// before compplan.hpp
#include "phys/rampacce.hpp"		// before compplan.hpp
#include "mathex/transf3d.hpp"		// before compplan.hpp
#include "stdlib/string.hpp"				// before compplan.hpp

#include "world4d/camera.hpp"
#include "world4d/root.hpp"
#include "world4d/shadow.hpp"
#include "world4d/manager.hpp"
#include "world4d/compplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/planentr.hpp"
#include "world4d/scalplan.hpp"

#include "world4d/internal/animdata.hpp"
#include "world4d/internal/nolight.hpp"
#include "world4d/internal/composii.hpp"
#include "world4d/internal/complexi.hpp"
#include "world4d/internal/entityi.hpp"

#include "mathex/line3d.hpp"
#include "mathex/radians.hpp"
#include "mathex/coordsys.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/double.hpp"

#include "render/scale.hpp"
#include "render/device.hpp"
#include "render/colour.hpp"

#include "utility/linetok.hpp"

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include <algorithm>
#include <math.h>

#define CB_W4dComposite_DEPIMPL() \
	CB_DEPIMPL( W4dLinks, links_ ); \
	CB_DEPIMPL( W4dLink::Id, maxLinkId_ ); \
	CB_DEPIMPL( W4dCompositeImpl::StoredLinkPositions*, pStoredLinkPositions_ ); \
	CB_DEPIMPL( HeldEntities*, pHeldEntities_ ); \
	CB_DEPIMPL( W4dDistance, maxRange_ ); \
	CB_DEPIMPL( MexAlignedBox3d, compositeBoundingVolume_ ); \
	CB_DEPIMPL( W4dCompositeImpl::CompositePlans, compositePlans_ );


PER_DEFINE_PERSISTENT_ABSTRACT( W4dComposite );

W4dComposite::W4dComposite( W4dEntity* pParent, const W4dTransform3d& localTransform, Solidity solid )
:   W4dEntity( pParent, localTransform, solid ),
	pImpl_( _NEW( W4dCompositeImpl( this ) ) )
{
	CB_W4dComposite_DEPIMPL();
    //Set the allocation size for the W4dLink vector
    links_.reserve( 6 );
    //Mark as a composite
    isComposite( true );

    TEST_INVARIANT;
}

W4dComposite::W4dComposite
(
	const W4dComposite& copyMe,
	W4dEntity* pParent,
	const W4dTransform3d& localTransform
)
:	W4dEntity( copyMe, pParent, localTransform ),
	pImpl_( _NEW( W4dCompositeImpl( copyMe.compositeImpl(), this ) ) )
{
	sharedCopyCtor(copyMe, COPY_LIGHTS);
}

W4dComposite::W4dComposite
(
	const W4dComposite& copyMe,
	W4dEntity* pParent,
	const W4dTransform3d& localTransform,
	CopyLights copyLights
)
:	W4dEntity( copyMe, pParent, localTransform ),
	pImpl_( _NEW( W4dCompositeImpl( copyMe.compositeImpl(), this ) ) )
{
	sharedCopyCtor(copyMe, copyLights);
}

void W4dComposite::sharedCopyCtor(const W4dComposite& copyMe, CopyLights copyLights)
{
	CB_W4dComposite_DEPIMPL();
    //Set the allocation size for the W4dLink vector
    links_.reserve( copyMe.links().size() );

    for( W4dLinks::const_iterator i = copyMe.links().begin(); i != copyMe.links().end(); ++i )
    {
        W4dLink* pLink;

        const W4dLink&  copyMeLink = **i;

        // Because links aren't in their parents childlist we have to set up
        // the parent for the copy by finding the index of the parent in the
        // copyMe link list, then looking up the same index in the new link
        // list

        W4dEntity*    pNewOwner;

        ASSERT_INFO( (void*)this );
        ASSERT_INFO( (void*)copyMeLink.pParent() );

        if( copyMeLink.pParent() == copyMeLink.pComposite() )
            pNewOwner = this;
        else
        {
            W4dLinks::const_iterator parent = find(
              copyMe.links().begin(),
              copyMe.links().end(),
              copyMeLink.pParent() );

            size_t parentIndex = parent - copyMe.links().begin();

            ASSERT_INFO( parentIndex );
            ASSERT_INFO( links_.size() );

            ASSERT( parentIndex < links_.size(), "" );

            pNewOwner = links_[ parentIndex ];
        }

        //  The id is the index into the links_ vector
        pLink = _NEW( W4dLink( copyMeLink, this, pNewOwner, copyMeLink.localTransform(), maxLinkId_++ ) );

        links_.push_back( pLink );

        ASSERT( maxLinkId_ == links_.size(), "Id is no longer same as index into links_" );

        //Increment root's descendant count
        findRoot()->adjustDescendantCount( 1 );
    }

	// TBD: this is a huge hack for ECTS.  Remove it.
	// If the composite has any shadow children, copy them.  We need to use
	// the child's name as a form of RTTI in order to down-cast to a shadow
	// interface (very crufty).
    for( W4dEntities::const_iterator i = copyMe.children().begin(); i != copyMe.children().end(); ++i )
    {
	    if ((*i)->name() == "SHADOW_PROJ" || (*i)->name() == "SHADOW_FIXED")
	    {
			W4dShadow* shd = _STATIC_CAST(W4dShadow*, *i);
			shd->copy(this);
		}
    }

	// The animations aren't copied by the ctor, so reapply the animation data.
	applyAllAnimationData(copyLights);
}

W4dComposite::~W4dComposite()
{
    TEST_INVARIANT;
	CB_W4dComposite_DEPIMPL();

    //Decrement the root's descendant count by the number of child links
    size_t nChildren = links_.size();
    if( nChildren != 0 )
        findRoot()->adjustDescendantCount( -nChildren );

    _DELETE( pImpl_ );
}

W4dCompositeImpl& W4dComposite::compositeImpl()
{
	CB_W4dComposite_DEPIMPL();
	PRE( pImpl_ != NULL )
	return *pImpl_;
}

const W4dCompositeImpl& W4dComposite::compositeImpl() const
{
	CB_W4dComposite_DEPIMPL();
	PRE( pImpl_ != NULL )

	return *pImpl_;
}

W4dLink* W4dComposite::addLink( const W4dTransform3d& localTransform, Solidity solid )
{
	CB_W4dComposite_DEPIMPL();
    W4dLink* pLink;

    pLink = _NEW( W4dLink( this, localTransform, solid, maxLinkId_++ ) );

    links_.push_back( pLink );

    //Increment root's descendant count
    findRoot()->adjustDescendantCount( 1 );

    return pLink;
}

// Constructs and adds a new link to the W4dComposite, with initial transform.
// Variant attaches link to parent link. The W4dLink is not part of the main render tree, in that it
// is not added to the list of child W4dEntities owned by the W4dComposite. The links form a
// distinct subtree.
W4dLink* W4dComposite::addLink( W4dLink* pParent, const W4dTransform3d& localTransform, Solidity solid )
{
	CB_W4dComposite_DEPIMPL();
    W4dLink* pLink;

    pLink = _NEW( W4dLink( pParent, localTransform, solid, maxLinkId_++ ) );

    links_.push_back( pLink );

    //Increment root's descendant count
    findRoot()->adjustDescendantCount( 1 );

    return pLink;
}

void W4dComposite::renderLinks( const W4dCamera& camera,
                                MATHEX_SCALAR range, const W4dComplexity& complexity ) const
{
	CB_W4dComposite_DEPIMPL();
	RenDevice* device = RenDevice::current();

    //Visit each link and render its mesh
    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
	{
    	W4dLink* pLink = *it;
		W4dEntityImpl& linkImpl = pLink->entityImpl();
		ASSERT(pLink, "Invalid link");

        //  Only render this link if its parent is visible and has
        //  been rendered this pass
        const W4dEntity* pParent = linkImpl.pParent();
		ulong parentPassId = pParent->passId();

        //JG commented out the parent id check below. It was causing links attached to links with no mesh
        //to not be rendered. I don't know what the check was for. It makes no sense to me.
        if( pParent->visible() /*and parentPassId == pLink->pComposite()->passId()*/ )
        {
            if( linkImpl.hasMesh() and pLink->visible() )
            {
				// If the link is flagged as totally unlit, disable the render
				// lighting.  This logic should ensure that we don't disable the
				// lighting and, more particularly, re-enable it if this node is a
				// child of a node which already disabled the lighting.
				bool lightingDisabledHere = false;
				if (linkImpl.doNotLight() && device->lightingEnabled())
				{
					//IAIN_STREAM("Disabled lighting for link " << (W4dEntity*) pLink << "\n");
					device->disableLighting();
					lightingDisabledHere = true;
				}

                linkImpl.passId( parentPassId );
                pLink->renderAtRange( camera, range, complexity );

				if (lightingDisabledHere)
				{
					// Re-enable the lighting system.
					ASSERT(!device->lightingEnabled(), "Expected lighting to be off.");
					device->enableLighting();
				}
            }
	    }
	}
}

bool W4dComposite::hasHeldEntities( void ) const
{
  CB_W4dComposite_DEPIMPL();
  return  pHeldEntities_!=NULL;
}

const W4dComposite::HeldEntities& W4dComposite::heldEntities( void )
{
  CB_W4dComposite_DEPIMPL();
  PRE(hasHeldEntities());
  return *pHeldEntities_;
}

void W4dComposite::hold( W4dEntity* pObject, W4dLink* pParent, const W4dTransform3d& newLocalTransform )
{
	CB_W4dComposite_DEPIMPL();
    //Attach to link. This cancels any previous holding relation
    pObject->attachTo( pParent, newLocalTransform );

    //Tell the object it is being held
    pObject->beHeldBy( this );

    //Ensure we have a holding vector
    if( pHeldEntities_ == NULL )
    {
        pHeldEntities_ = _NEW( HeldEntities );
        pHeldEntities_->reserve( 2 );
    }

    //Add the object to the vector
    pHeldEntities_->push_back( pObject );
}

// virtual
void W4dComposite::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
	CB_W4dComposite_DEPIMPL();
    ostr << "Start W4dComposite " << (void*)this << "  " << name() << std::endl;

    for( W4dLinks::const_iterator i = links_.begin(); i != links_.end(); ++i )
    {
        (*i)->debugOutput( ostr );
    }

    ostr << "End   W4dComposite " << (void*)this << std::endl;
}

void W4dComposite::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// cmake   #include "render/device.hpp"			// TBD: temporary optimisation stuff.

inline float square(float v)
{
	return v * v;
}

// virtual
W4dEntity::RenderType W4dComposite::render( const W4dCamera& camera,
                           W4dComplexity& complexity )
{
	CB_W4dComposite_DEPIMPL();
	PRE_INFO( name() );

	// Calculate the effective range we are going to use for the composite
	const MexPoint3d eyePos( camera.globalTransform().position() );
    const MexPoint3d myPos( globalTransform().position() );
	const MATHEX_SCALAR effectiveRangeSqr = eyePos.sqrEuclidianDistance(myPos);

	// As an optimisation, maxRange_ holds the highest LOD range of any
	// mesh owned by this composite, its links or its children.  If we are
	// beyond this range, then the composite can be entirely skipped.
	if ( W4dDistance(effectiveRangeSqr) <= maxRange_)
	{
		// Enable any colour filter attached to this entity.
		const RenColour& filter = (filterColourIsSet())? filterColour(): RenColour::white();
		RenDevice::Filter colourfilter(RenDevice::current(), filter);

		// TBD: this is a temporary optimisation.  If a composite is beyond
		// the maximum fogging range, then it is completely obscured and
		// we can discount it.  In practice, whether or not it can be
		// discounted is subjective and depends on things like the fog and
		// background colours.
		const RenDevice* dev = RenDevice::current();
		if (effectiveRangeSqr <= square(dev->fogEnd()))
		{
			// The composite and its links has a highest rendering priority
			// if any motion plan is defined
			if( complexity.enabled() )
			{
				uint priority=0;
				priority= ( hasPlan() and entityPlan().hasMotionPlan() ) ? 1 : 0;
		    	for( W4dLinks::iterator it = links_.begin(); it != links_.end() and (priority==0); ++it )
    			{
        			W4dLink* pLink = *it;
	        		priority = ( pLink != NULL  and  pLink->hasPlan() and pLink->entityPlan().hasMotionPlan() ) ? 1 : 0;
    			}

				complexity.usePriority( priority );
				complexity.range( effectiveRangeSqr );
			}

	        //Render the composite's own mesh
			if( hasMesh() )
	        	W4dEntity::renderAtRange( camera, effectiveRangeSqr, complexity );

	        //Render the links
	        renderLinks( camera, effectiveRangeSqr, complexity );
		}

        //Render any held objects
        renderHeldObjects( camera );
	}

	return RENDER_CHILDREN;
}

void W4dComposite::plan( const W4dCompositePlan& newPlan, const PhysAbsoluteTime& startTime,
                         uint nRepetitions, uint animId )
{
	CB_W4dComposite_DEPIMPL();
    //Apply any composite entity plan
    if( newPlan.hasCompositePlan() )
        entityPlanForEdit().plan( newPlan.compositePlan(), startTime, nRepetitions, animId );

    //Deal with the link plans
    W4dCompositePlan::Entries entries = newPlan.entries();
    W4dCompositePlan::Entries::iterator i;

    for( i = entries.begin(); i != entries.end(); ++i )
    {
        W4dCompositePlanEntry* pEntry = *i;
        W4dLink::Id id = pEntry->id();
        ASSERT( id < maxLinkId_, "" );
        W4dLink* pLink = links_[id];
        if( pLink != NULL )
        {
            pLink->entityPlanForEdit().plan( pEntry->plan(), startTime, nRepetitions, animId );
        }
    }
}


bool W4dComposite::findLink( const string& name, W4dLink** ppLink ) const
{
    return compositeImpl().findLink( name, ppLink );
}

bool W4dComposite::findCompositePlan( const string& name, W4dCompositePlanPtr* pPlanPtr ) const
{
	CB_W4dComposite_DEPIMPL();
    bool    planFound = false;
    for( W4dCompositeImpl::CompositePlans::const_iterator i = compositePlans_.begin(); i != compositePlans_.end() and not planFound; ++i )
    {
        if( (*i)->name() == name )
        {
            planFound = true;
            *pPlanPtr = *i;
        }
    }
    return planFound;
}

bool W4dComposite::readAnimation( const SysPathName& fileName,
                                     const string& animationName,
                                     W4dCompositePlan* pCompositePlan,
                                     MATHEX_SCALAR framesPerSecond ) const
{
    //Check arguments
    PRE( pCompositePlan != NULL );
    PRE_INFO( fileName );
    PRE( fileName.existsAsFile() );
	return compositeImpl().readAnimation( fileName, animationName, pCompositePlan, framesPerSecond );
}

//This cycles through the animation sequences of a plan if it has any.
bool W4dComposite::cycleAnims(W4dCompositePlanPtr* pPlanPtr) const
{
	CB_W4dComposite_DEPIMPL();

	if(compositePlans_.size()==0)
		return false;


	W4dCompositePlanPtr ptr = *pPlanPtr;
	if(!ptr.isDefined())
	{
		ptr = compositePlans_.front();
	}
	else
	{
		W4dCompositeImpl::CompositePlans::const_iterator it = find(compositePlans_.begin(), compositePlans_.end(), ptr);
		ASSERT(it != compositePlans_.end(), "invalid parameter");

		if( *it == compositePlans_.back() )
		{
			ptr = compositePlans_.front();
		}
		else
		{
			++it;
			ptr = *it;
		}
	}

	*pPlanPtr = ptr;

	return true;
}



const W4dComposite::W4dLinks& W4dComposite::links( void ) const
{
	CB_W4dComposite_DEPIMPL();
    return links_;
}

void W4dComposite::clearAllPlans( void )
{
	CB_W4dComposite_DEPIMPL();
    //Clear the composite's plans
    clearPlan();

    //Clear the plans of all its links
    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
        (*it)->clearPlan();
}

void W4dComposite::storeLinkTransforms( void ) const
{
	CB_W4dComposite_DEPIMPL();
    //We want to mutate our mutables
    W4dComposite* nonConstThis = (W4dComposite*)this;

    //Clear any existing data
    _DELETE( pStoredLinkPositions_ );

    //Add the data for each link
    nonConstThis->compositeImpl().pStoredLinkPositions_ = _NEW( W4dCompositeImpl::StoredLinkPositions );
    nonConstThis->compositeImpl().pStoredLinkPositions_->reserve( links_.size() );

    for( W4dLinks::const_iterator it = links_.begin(); it != links_.end(); ++it )
    {
        W4dLink* pLink = *it;
        nonConstThis->compositeImpl().pStoredLinkPositions_->push_back(
            W4dCompositeImpl::StoredLinkPosition( pLink->pParent(), pLink->localTransform() ) );
    }
}

void W4dComposite::restoreLinkTransforms( void )
{
	CB_W4dComposite_DEPIMPL();
    //Check stored
    PRE( pStoredLinkPositions_ );
    PRE( links_.size() == pStoredLinkPositions_->size() );

    //Run through the links restoring
    W4dCompositeImpl::StoredLinkPositions::const_iterator is = pStoredLinkPositions_->begin();
    for( W4dLinks::iterator il = links_.begin(); il != links_.end(); ++il )
    {
        //Extract the data for this link
        W4dLink* pLink = *il;
        const W4dCompositeImpl::StoredLinkPosition& lp = *is;
        ++is;

        //If the parent has changed,...
        if( lp.first != pLink->pParent() )
        {
            //Attach and set transform in one go
            pLink->attachTo( lp.first, lp.second );
        }
        else
        {
            //Just set the transform
            pLink->localTransform( lp.second );
        }
    }
}

void    W4dComposite::addMaterials( RenMaterialSet* pSet ) const
{
	CB_W4dComposite_DEPIMPL();
    for( W4dLinks::const_iterator i = links_.begin(); i != links_.end(); ++i )
    {
        (*i)->addMaterials( pSet );
    }
}

// virtual
void    W4dComposite::materialMap( const RenMaterialMap& map )
{
	CB_W4dComposite_DEPIMPL();
    for( W4dLinks::const_iterator i = links_.begin(); i != links_.end(); ++i )
    {
        (*i)->materialMap( map );
    }
}

W4dLink* W4dComposite::pEquivalentLink( const W4dLink* pOriginalLink ) const
{
	CB_W4dComposite_DEPIMPL();
    PRE( pOriginalLink != NULL );
    PRE( pOriginalLink->id() < links().size() );

    return links()[ pOriginalLink->id() ];
}

void W4dComposite::computeMaxRange()
{
	CB_W4dComposite_DEPIMPL();
	maxRange_ = 0;

	// Search this composite's links for the largest LOD range.
    for (W4dLinks::const_iterator i = links_.begin(); i != links_.end(); ++i)
    {
        W4dLink* link = *i;
		if (link)
			maxRange_ = std::max(maxRange_, link->maxRange());
    }

	// Search this composite's own meshes & children for the largest LOD range.
	maxRange_ = std::max(maxRange_, maxRange());
//	cout << "Max range for composite \"" << name() << "\" is " << maxRange_ << std::endl;

	POST(maxRange_ >= W4dDistance(0) );
}

void W4dComposite::readCompositeFile( const SysPathName& pathname )
{
	CB_W4dComposite_DEPIMPL();
    PRE_INFO( pathname.pathname() );
    PRE( pathname.existsAsFile() );

    const SysPathName& directoryName = pathname.directory();

    UtlLineTokeniser   parser( pathname );

	// Name this object.  Quite important for debugging.
	name(pathname.filename());

    W4dLOD  id = 0;
	bool hasShadow = false;

    while( not parser.finished() )
    {
        ASSERT_INFO( parser.tokens()[ 0 ] );

        if( parser.tokens()[ 0 ] == "MODEL" )
        {
            compositeImpl().parseModel( directoryName, &parser, id++ );
        }
        else if( parser.tokens()[ 0 ] == "MESH_RANGES" )
        {
            compositeImpl().parseMeshRanges( directoryName, &parser );
        }
        else if( parser.tokens()[ 0 ] == "SHADOW_FIXED" || parser.tokens()[ 0 ] == "SHADOW_PROJ" )
        {
            compositeImpl().parseShadow( directoryName, &parser );
			hasShadow = true;
        }
        else if( parser.tokens()[ 0 ] == "ANIMATION" )
        {
            compositeImpl().parseAnimation( directoryName, &parser );
        }
		else if( parser.tokens()[ 0 ] == "ANIMATION_DATA" )
		{
			parseAnimationData(	&parser );
		}
        else
        {
            ASSERT_INFO( parser.tokens()[ 0 ] );
            ASSERT_FAIL( "Illegal token" );
        }
    }

	// This assumes that the composite's meshes, children and links don't
	// change after it is read from the file.  Given the hairiness of the
	// design, this is not guaranteed, but it's a reasonable assumption.
	computeMaxRange();

    //Update the bounding volume which includes the links in their initial positions
    updateCompositeBoundingVolume();

	applyAllAnimationData();

	// Check to see if this composite is entirely emissive.  Shadows ought
	// never be emissive, so that's a real quick test for most composites.
	// Thus the expensive needsLighting test is never done for most comps.
	if (!hasShadow)
	{
		if (!w4dNeedsLighting(this))
		{
			doNotLight(true);
			IAIN_STREAM("Disabled lighting for " << pathname << "\n");
		}
	}
}

const MexAlignedBox3d& W4dComposite::compositeBoundingVolume() const
{
	CB_W4dComposite_DEPIMPL();
    return compositeBoundingVolume_;
}

void W4dComposite::updateCompositeBoundingVolume()
{
	CB_W4dComposite_DEPIMPL();
    const MexAlignedBox3d emptyVolume( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

    //Start with the composites own boundary
    compositeBoundingVolume_ = boundingVolume();

    //We need the inverse global transform of the composite to compute
    //the transform from this to each link which is not directly owned by this.
    MexTransform3d inverseGlobal;
    globalTransform().invert( &inverseGlobal );

    //Get pointer to this as an entity
    W4dEntity* pThisAsEntity = this;

    //Visit each link
    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
    {
        //Check link has a mesh
        W4dLink* pLink = *it;
        if( pLink->hasMesh() )
        {
            //get ref to link's BV
            const MexAlignedBox3d& linkVolume = pLink->boundingVolume();

            if( linkVolume != emptyVolume )
            {
                //Get the transform relative to the composite. If owned directly, this
                //is its local transform. Otherwise we use the composite's inverse global to
                //compute it.
                if( pLink->pParent() == pThisAsEntity )
                {
                    if( compositeBoundingVolume_ == emptyVolume )
                        compositeBoundingVolume_ = MexAlignedBox3d( linkVolume, pLink->localTransform() );
                    else
                        compositeBoundingVolume_.uniteWith( linkVolume, pLink->localTransform() );
                }
                else
                {
                    MexTransform3d relativeTransform;
                    inverseGlobal.transform( pLink->globalTransform(), &relativeTransform );
                    if( compositeBoundingVolume_ == emptyVolume )
                        compositeBoundingVolume_ = MexAlignedBox3d( linkVolume, relativeTransform );
                    else
                        compositeBoundingVolume_.uniteWith( linkVolume, relativeTransform );
                }
            }
        }
    }

    //Now add in any held objects
    if( pHeldEntities_ != NULL )
    {
        HeldEntities& heldEntities = *pHeldEntities_;
        for( HeldEntities::iterator it = heldEntities.begin(); it != heldEntities.end(); ++it )
        {
            W4dEntity& held = *(*it);

			// If the held entity is a composite, use the composite bounding volume.
			const MexAlignedBox3d* heldVol = NULL;
			if (held.isComposite())
				heldVol = &(held.asComposite().compositeBoundingVolume());
			else
				heldVol = &(held.boundingVolume());
			ASSERT(heldVol, "logic error getting bounding volume");

            if( *heldVol != emptyVolume )
            {
                MexTransform3d relativeTransform;
                inverseGlobal.transform( held.globalTransform(), &relativeTransform );

                if( compositeBoundingVolume_ == emptyVolume )
                    compositeBoundingVolume_ = MexAlignedBox3d( *heldVol, relativeTransform );
                else
                    compositeBoundingVolume_.uniteWith( *heldVol, relativeTransform );
            }
        }
    }
}

bool W4dComposite::intersectsCompositeBoundingVolume
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance
) const
{
	CB_W4dComposite_DEPIMPL();
    //Get the global transform, and invert it so that we can produce a copy
    //of the line transformed into the local coordinate system this entity.
    MexTransform3d inverseGlobal;
    globalTransform().invert( &inverseGlobal );
    MexLine3d localLine( line );
    localLine.transform( inverseGlobal );

    //Intersect with the bounding volume
    MATHEX_SCALAR entryDistance, exitDistance;
    bool result = compositeBoundingVolume_.intersects( localLine, &entryDistance, &exitDistance );

    if( result )
    {
        //If intersects after the line start point, use the entry distance, otherwise return
        //zero indicating that the start point is inside the volume
        *pDistance = (entryDistance > 0.0 ? entryDistance : 0.0);
    }

    return result;
}

bool W4dComposite::defaultCompositeIntersectsLine
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance, Accuracy accuracy
) const
{
	CB_W4dComposite_DEPIMPL();
    //First check the all encompassing bounding volume
    bool result = intersectsCompositeBoundingVolume( line, pDistance );

    //If have a hit and not low accuracy, check each link plus the composite
    if( result and accuracy != LOW )
    {
        //Check the composite itself
        MATHEX_SCALAR minDistance, foundDistance;
        result = hasMesh()  and  defaultIntersectsLine( line, &foundDistance, accuracy );
      //JON_STREAM("  Composite mesh result " << result << std::endl;)
        if( result )
            minDistance = foundDistance;

        //Now check each link
        for( W4dLinks::const_iterator it = links_.begin(); it != links_.end(); ++it )
        {
            //Check link has a mesh
            W4dLink* pLink = *it;
            if( pLink->hasMesh() )
            {
              //JON_STREAM("  Checking link " << (void*)pLink << std::endl; )
                //Use the intersects method on this link
                if( pLink->intersectsLine( line, &foundDistance, accuracy ) )
                {
                    //Keep track of the nearest hit
                    if( not result  or  foundDistance < minDistance )
                        minDistance = foundDistance;
                    result = true;
                  //JON_STREAM("  Hit!" << std::endl;)
                }
                else
                {
                  //JON_STREAM("  Miss" << std::endl;)
                }
            }
        }

        //Update the distance parameter if we have a hit
        if( result )
            *pDistance = minDistance;
    }

    //Update the distance parameter if we have a hit
  //JON_STREAM("W4dComposite::defaultCompositeIntersectsLine exit " << result << " " << *pDistance << std::endl;)
    return result;
}

PhysAbsoluteTime W4dComposite::finishAnimation( uint animId )
{
	CB_W4dComposite_DEPIMPL();
    //finish any composite entity plan
    PhysAbsoluteTime latestFinishTime = 0.0;

    if( hasPlan() )
        latestFinishTime = entityPlanForEdit().finishAnimation( animId );

    //Deal with the links
    PhysAbsoluteTime linkFinishTime;

    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
    {
        W4dLink* pLink = *it;
        if( pLink != NULL  and  pLink->hasPlan() )
        {
            linkFinishTime = pLink->entityPlanForEdit().finishAnimation( animId );
            latestFinishTime = std::max( latestFinishTime, linkFinishTime );
        }
    }

    return latestFinishTime;
}

//virtual
void W4dComposite::temporaryScale( const RenScale& scale, PropogateScaleFlag propogate )
{
	CB_W4dComposite_DEPIMPL();
    //Apply the scale to the composite itself and non-link children
    W4dEntity::temporaryScale( scale, propogate );

    //Visit each link if propogating
    if( propogate  and  links_.size() != 0 )
    {
        //Check for non-uniform scale
        const RenNonUniformScale* pNonUniformScale =
            ( scale.isNonUniform() ? &scale.asNonUniform() : NULL );

        MexTransform3d toLinkTransform;

        for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
        {
            W4dLink* pLink = *it;

            //Make link inherit transform position scale from its parent
            pLink->scaleTransformFromParent( true );

            //Apply scale recursively via links
            if( pNonUniformScale )
            {
                //Get the link's transform
                linkTransform( *pLink, &toLinkTransform );

                //Apply scale to link and its descendants using special recursive function
                pLink->temporaryScale( *pNonUniformScale, toLinkTransform );
            }
            else
            {
                pLink->temporaryScale( scale, propogate );
            }
        }
    }
}

//virtual
void W4dComposite::temporaryScale
(
    const RenNonUniformScale& scale, const MexTransform3d& useTransform
)
{
	CB_W4dComposite_DEPIMPL();
    //Apply to the composite itself
    W4dEntity::temporaryScale( scale, useTransform );

    //Visit each link
    if( links_.size() != 0 )
    {
        MexTransform3d linkUseTransform;
        MexTransform3d toLinkTransform;

        for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
        {
            W4dLink* pLink = *it;

            //Make link inherit transform position scale from this entity
            pLink->scaleTransformFromParent( true );

            //Get the accumulated transform by combining the argument with the link's transform
            linkTransform( *pLink, &toLinkTransform );
            useTransform.transform( toLinkTransform, &linkUseTransform );

            //Apply scale to link
            pLink->temporaryScale( scale, linkUseTransform );
        }
    }
}

void W4dComposite::linkTransform( const W4dLink& link, MexTransform3d* pTransform ) const
{
	CB_W4dComposite_DEPIMPL();
    PRE( pTransform != NULL );

    //Initialise with local transform of the link
    *pTransform = link.localTransform();

    //Work up the tree
    for( const W4dEntity* pEntity = link.pParent(); pEntity != this; pEntity = pEntity->pParent() )
    {
        //Combine the new parent transform
        MexTransform3d transformSoFar( *pTransform );
        pEntity->localTransform().transform( transformSoFar, pTransform );
    }
}

//virtual
void W4dComposite::clearTemporaryScale( PropogateScaleFlag propogate )
{
	CB_W4dComposite_DEPIMPL();
    //Do the work for the composite itself
    W4dEntity::clearTemporaryScale( propogate );

    //Clear the links if propogating
    if( propogate )
    {
        for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
            (*it)->clearTemporaryScale( propogate );
    }
}

//virtual
void W4dComposite::propogateScalePlan
(
    const W4dScalePlanPtr& planPtr, const PhysAbsoluteTime& startTime,
    uint repetitions, uint animId
)
{
	CB_W4dComposite_DEPIMPL();
    //Apply the plan to the composite itself and non-link children
    W4dEntity::propogateScalePlan( planPtr, startTime, repetitions, animId );

    //Visit each link
    if( links_.size() != 0 )
    {
        //Check for non-uniform scale
        bool isNonUniform = planPtr->isNonUniform();

        MexTransform3d toLinkTransform;

        for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
        {
            W4dLink* pLink = *it;

            //Make link inherit transform position scale from its parent
            pLink->scaleTransformFromParent( true );

            //Apply plan recursively via links
            if( isNonUniform )
            {
                //Get the link's transform
                linkTransform( *pLink, &toLinkTransform );

                //Apply plan to link and its descendants using special recursive function
                pLink->propogateScalePlan( planPtr, startTime, repetitions, animId,
                                           toLinkTransform );
            }
            else
            {
                pLink->propogateScalePlan( planPtr, startTime, repetitions, animId );
            }
        }
    }
}

//virtual
void W4dComposite::propogateScalePlan
(
    const W4dScalePlanPtr& planPtr, const PhysAbsoluteTime& startTime,
    uint repetitions, uint animId, const MexTransform3d& offsetTransform
)
{
	CB_W4dComposite_DEPIMPL();
    PRE( planPtr->isNonUniform() );

    //Apply the plan to the composite itself and non-link children
    W4dEntity::propogateScalePlan( planPtr, startTime, repetitions, animId, offsetTransform );

    //Visit each link
    if( links_.size() != 0 )
    {
        MexTransform3d linkUseTransform;
        MexTransform3d toLinkTransform;

        for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
        {
            W4dLink* pLink = *it;

            //Make link inherit transform position scale from this entity
            pLink->scaleTransformFromParent( true );

            //Get the accumulated transform by combining the argument with the link's transform
            linkTransform( *pLink, &toLinkTransform );
            offsetTransform.transform( toLinkTransform, &linkUseTransform );

            //Apply plan to link and decsendants
            pLink->propogateScalePlan( planPtr, startTime, repetitions, animId, offsetTransform );
        }
    }
}

void W4dComposite::stopHolding( W4dEntity* pObject )
{
	CB_W4dComposite_DEPIMPL();
    //Check holding anything
    if( pHeldEntities_ != NULL )
    {
        //See if holding this object
        HeldEntities::iterator it = find( pHeldEntities_->begin(), pHeldEntities_->end(), pObject );
        if( it != pHeldEntities_->end() )
        {
            //Remove the entry
            pHeldEntities_->erase( it );

            //Cancel the holding entry in the object
            if( pObject->isHeld() )
                pObject->cancelIsHeld();
        }
    }
}

void W4dComposite::renderHeldObjects( const W4dCamera& camera ) const
{
	CB_W4dComposite_DEPIMPL();
    //Check we have some
    if( pHeldEntities_ != NULL )
    {
        for( HeldEntities::iterator it = pHeldEntities_->begin();
             it != pHeldEntities_->end(); ++it )
        {
            _CONST_CAST( W4dCamera&, camera).renderSubTree( *it );
        }
    }
}

void perWrite( PerOstream& ostr, const W4dComposite& composite )
{
    const W4dEntity& base = composite;

    ostr << base;
    ostr << composite.pImpl_;
}

void perRead( PerIstream& istr, W4dComposite& composite )
{
    W4dEntity& base = composite;
    istr >> base;
	ASSERT(composite.isComposite(), "Did a persistent read and didn't get a composite.");

	_DELETE( composite.pImpl_ );
    istr >> composite.pImpl_;

    //It is not necessary to attach the lights in exemplars, and for some reason this code
    //causes a crash on loading building interiors with lights.
	// Apply any animation data pertaining to the composite itself.  Links will
	// presumably have their animations copied by the entity perRead fns.
	//if (composite.entityImpl().hasAnimationData())
	//	composite.applyOwnAnimationData();
}

W4dComposite::W4dComposite( PerConstructor con )
: W4dEntity( con ),
    pImpl_( NULL )
{
}

void W4dComposite::applyOwnAnimationData()
{
	PRE(entityImpl().hasAnimationData());

	// Apply the light animations to this object only.
	const PhysAbsoluteTime now = W4dManager::instance().time();
	entityImpl().applyLightAnimations(this, now);
}

void W4dComposite::applyAllAnimationData(CopyLights copyLights)
{
    CB_W4dComposite_DEPIMPL();

	const PhysAbsoluteTime now = W4dManager::instance().time();

	if (entityImpl().hasAnimationData())
	{
		if (copyLights == COPY_LIGHTS)
			applyOwnAnimationData();

		// Apply the other kinds of animations to the links.
	    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
	    {
	        W4dLink* pLink = *it;
			if (pLink->hasMesh())
				entityImpl().applyNonLightAnimations(pLink, now);
		}
	}
}

void W4dComposite::clearAnimation( uint animId )
{
    CB_W4dComposite_DEPIMPL();
    //clear any composite entity plan
    if( hasPlan() )
        entityPlanForEdit().clearAnimation( animId );

    //Deal with the links
    for( W4dLinks::iterator it = links_.begin(); it != links_.end(); ++it )
    {
        W4dLink* pLink = *it;
        if( pLink != NULL  and  pLink->hasPlan() )
            pLink->entityPlanForEdit().clearAnimation( animId );
    }
}

/* End COMPOSIT.CPP *************************************************/
