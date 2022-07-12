/*
 * C A M E R A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "ctl/list.hpp"
#include "world4d/camera.hpp"

#include "render/camera.hpp"
#include "render/device.hpp"

#include "world4d/domain.hpp"
#include "world4d/portal.hpp"
#include "world4d/root.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "world4d/lllist.hpp"
#include "world4d/camvolum.hpp"
#include "world4d/envirnmt.hpp"
#include "world4d/internal/complexi.hpp"
#include "world4d/internal/entityi.hpp"

#include "stdlib/string.hpp"

// For debugging the culling.  This generates loads of output, so it's not
// normally compiled into the code.
//#define DO_CULL_TRACING
#ifdef DO_CULL_TRACING
#define CULL_TRACE(x)	x
#define CULL_STREAM(x)	IAIN_STREAM(x)
#define CULL_INDENT(x)	IAIN_INDENT(x)
#else
#define CULL_TRACE(x)	;
#define CULL_STREAM(x)	;
#define CULL_INDENT(x)	;
#endif

PER_DEFINE_PERSISTENT( W4dCamera );

W4dCamera::W4dCamera(W4dSceneManager* mgr, W4dEntity* pParent, const W4dTransform3d& localXform):
	W4dEntity(pParent, localXform, NOT_SOLID),
	camera_(_NEW(RenCamera)),
	manager_(mgr),
    pVolume_( _NEW( W4dCameraVolume( *this ) ) ),
	lastPassId_( 0 ),
	passId_( 0 ),
	savedYonClipDistance_( 500 )
{
	PRE(mgr);
    PRE( pParent );
	ASSERT(camera_, 	 runtime_error("Out of memory."));
}

W4dCamera::~W4dCamera()
{
	_DELETE(camera_);
	_DELETE(pVolume_);
}

// Render the tree of W4dEntities that this camera is attached to,
// starting from the root of the tree.
void W4dCamera::inOrderRender()
{
	W4dRoot* root = findRoot();
	ASSERT(!root->hasParent(), logic_error());

	CULL_TRACE( entitiesRendered_ = 0 );
	CULL_TRACE( domainsRendered_  = 0 );

    //Update the camera clipping volume
    pVolume_->update();

	passId_ = W4dManager::instance().generateRenderPassId();

	renderTree(root, IN_ORDER_RENDER);

}

// Render the given node and any tree below it.
void W4dCamera::renderTree(W4dEntity* node, TraversalType traversalType)
{
	PRE(node);
	CULL_STREAM("renderTree for " << node << "\n" );
	CULL_INDENT(2);

	RenDevice* device = RenDevice::current();
	W4dEntityImpl& nodeImpl = node->entityImpl();

	// Has the node been rendered by the given camera already on the current
	// render pass?  If so, don't repeatedly render this entity.
	if( nodeImpl.passId() == renderPassId() )
	{
		CULL_STREAM("Processed already according to PassID\n");
	}
	else
	{
		// Mark this entity as rendered during this pass.
		nodeImpl.passId( renderPassId() );
	    CULL_TRACE( ++entitiesRendered_ );

	    //Check it is visible
	    if( node->visible() )
	    {
			// If this object is flagged as totally unlit, disable the render
			// lighting.  This logic should ensure that we don't disable the
			// lighting and, more particularly, re-enable it if this node is a
			// child of a node which already disabled the lighting.
			bool lightingDisabledHere = false;
			if (nodeImpl.doNotLight() && device->lightingEnabled())
			{
				CULL_STREAM("Disabled lighting for " << node << "\n");
				device->disableLighting();
				lightingDisabledHere = true;
			}
	    	else if(nodeImpl.hasLightList())
			{
		        //If we have a light list turn it on
				CULL_STREAM("lights on for " << node << "\n");
	    		nodeImpl.lightListForEdit().turnOnAll();
			}

	        W4dEntity::RenderType renderType = RENDER_CHILDREN;

			// TBD: make this robust, it's a *REAL HACK*.  What we really want to
			// test is: will W4dEntity::render do anything?  With the current
			// virtual implementations of render, the test below should amount to
			// the same thing.  This isn't guaranteed.
			bool nodeIsComposite = nodeImpl.isComposite();
			if( nodeIsComposite or nodeImpl.hasMesh() )
			{
		        // Check to see if the bounding volume of the entity intersects the
				// camera's bv.
		        if( pVolume_->intersects( *node ) )
		        {
					// There's no need to call this method if the lighting system is
					// turned off.  We don't check the doNotLight flag because the
					// lighting could have been turned off by a parent of the node.
					if (device->lightingEnabled())
						manager_->cullDynamicLights(node);

					renderType = node->render(*this, manager_->complexity() );
				}
			}

   	        //render the node's children if required
	    	if (renderType == W4dEntity::RENDER_CHILDREN)
	    	{
    	    	const W4dEntity::W4dEntities& kids = nodeImpl.children();
				if( nodeIsComposite )
	    	    {
					// render the composite children based without recomputing the intersection relationships
	    			for (W4dEntity::W4dEntities::const_iterator it=kids.begin(); it!=kids.end(); ++it)
	    			{
	                	if( traversalType == IN_ORDER_RENDER or ( not (*it)->isDomain() ) )
	        				renderSubTree(*it);
	    			}
	    		}
				else
				{
	    			for (W4dEntity::W4dEntities::const_iterator it=kids.begin(); it!=kids.end(); ++it)
	    			{
	                	if( traversalType == IN_ORDER_RENDER or ( not (*it)->isDomain() ) )
	        				renderTree(*it, traversalType);
	    			}
				}
			}

			if (lightingDisabledHere)
			{
				// Re-enable the lighting system.
				ASSERT(!device->lightingEnabled(), "Expected lighting to be off.");
				device->enableLighting();
			}
	    	else if (nodeImpl.hasLightList())
			{
		        //turn off any lights
				CULL_STREAM("lights off for " << node << "\n");
	    		nodeImpl.lightListForEdit().turnOffAll();
			}
	    }
	}

	CULL_INDENT(-2);
}
// Render the given node and any tree below it. without recomputing the intersecting volume
// and calling cullDynamicLights
// shoud be called to render the children and the heldObjects of a composite object

void W4dCamera::renderSubTree(W4dEntity* node)
{
	PRE(node);
	CULL_STREAM("renderSubTree for " << node << "\n" );
	CULL_INDENT(2);

	RenDevice* device = RenDevice::current();
	W4dEntityImpl& nodeImpl = node->entityImpl();

	// Has the node been rendered by the given camera already on the current
	// render pass?  If so, don't repeatedly render this entity.
	if( nodeImpl.passId() == renderPassId() )
	{
		CULL_STREAM("Processed already according to PassID\n");
	}
	else
	{
		// Mark this entity as rendered during this pass.
		nodeImpl.passId( renderPassId() );
	    CULL_TRACE( ++entitiesRendered_ );

	    //Check it is visible
	    if( node->visible() )
	    {
			// If this object is flagged as totally unlit, disable the render
			// lighting.  This logic should ensure that we don't disable the
			// lighting and, more particularly, re-enable it if this node is a
			// child of a node which already disabled the lighting.
			bool lightingDisabledHere = false;
			if (nodeImpl.doNotLight() && device->lightingEnabled())
			{
				CULL_STREAM("Disabled lighting for " << node << "\n");
				device->disableLighting();
				lightingDisabledHere = true;
			}
	    	else if(nodeImpl.hasLightList())
			{
		        //If we have a light list turn it on
				CULL_STREAM("lights on for " << node << "\n");
	    		nodeImpl.lightListForEdit().turnOnAll();
			}

	        //Check to see if the bounding volume of the entity intersects the camera's bv
	        W4dEntity::RenderType renderType = RENDER_CHILDREN;

			// TBD: make this robust, it's a *REAL HACK*.  What we really want to
			// test is: will W4dEntity::render draw anything which needs lights?
			// With the current virtual implementations of render, the test below
			// should amount to the same thing.  This isn't guaranteed.
			if( nodeImpl.isComposite() or nodeImpl.hasMesh() )
				renderType = node->render(*this, manager_->complexity() );

	    	if (renderType == W4dEntity::RENDER_CHILDREN)
	    	{
	    	    const W4dEntity::W4dEntities& kids = nodeImpl.children();
	    		for (W4dEntity::W4dEntities::const_iterator it=kids.begin(); it!=kids.end(); ++it)
	    		{
	                if( not (*it)->isDomain() )
	        			renderSubTree(*it);
	    		}
	    	}

			if (lightingDisabledHere)
			{
				// Re-enable the lighting system.
				ASSERT(!device->lightingEnabled(), "Expected lighting to be off.");
				device->enableLighting();
			}
	    	else if(nodeImpl.hasLightList())
			{
		        //turn off any lights
				CULL_STREAM("lights off for " << node << "\n");
	    		nodeImpl.lightListForEdit().turnOffAll();
			}
	    }
	}

	CULL_INDENT(-2);
}

// Render the tree that this camera is attached to using domain and
// portal culling.
void W4dCamera::domainRender(const int maxDepth)
{
	PRE(maxDepth > 0);

    //Check the camera is in a domain
	W4dDomain* myDomain;
    if( hasContainingDomain( &myDomain ) )
    {
        CULL_TRACE( entitiesRendered_ = 0 );
    	CULL_TRACE( domainsRendered_  = 0 );

        //Update the camera clipping volume
		if( lastRenderTransformKey_!=globalTransform().key() )
	        pVolume_->update();

    	passId_ = W4dManager::instance().generateRenderPassId();
    	recursiveDomainRender(myDomain, NULL, maxDepth);

    	// We're finished with the domain culling.

    	// For test purposes only: render any non-domain children of the root.
    	W4dRoot* root = findRoot();
    	ASSERT(!root->hasParent(), logic_error());

        const W4dEntity::W4dEntities& kids = root->children();
    	for (W4dEntity::W4dEntities::const_iterator it=kids.begin(); it!=kids.end(); ++it)
    	{
    		W4dEntity* entity = *it;
    		if (!entity->isDomain())
    		{
                renderTree( entity, DOMAIN_RENDER);
    		}
    	}

		lastRenderTransformKey_=globalTransform().key();
		lastPassId_=passId_;
    	// Finished rendering.

    }
    else
    {
        //camera not in a domain, so do an inorder render
        inOrderRender();
    }
}

void W4dCamera::recursiveDomainRender
(
	W4dDomain* in, 				// what this call takes us into
	int depth,					// how many domains we've gone through
	int maxDepth				// stop after this many domains
)
{
	PRE(in);

	// The domain traversal algorithm should ensure that we don't process
	// the same domain twice.
	PRE(in->latestRenderPassId() != renderPassId());

	CULL_STREAM("traversing into " << (W4dEntity*) in << " depth=" << depth << "\n" );
	CULL_INDENT(2);
	CULL_TRACE( ++domainsRendered_ );

	// Limit the rendering.
	if (depth > maxDepth)
	{
		CULL_STREAM("reached maximum domain depth (" << maxDepth << ")\n");
	}
	else
	{
		// Render the domain's subtree.
		// NB: also tags the domain with this camera's render pass Id.
		renderTree( in, DOMAIN_RENDER);

	    // The above renderTree call will turn on the domain's light list then
		// turn it off again.  We need the list to be on for any intersecting
		// entities or for recursive domains.
		if (in->hasLightList())
		{
			CULL_STREAM("lights on for " << (W4dEntity*) in << "\n");
			in->lightListForEdit().turnOnAll();
		}

		// Render any entities which (might) intersect the current domain.
		const W4dEntities& entities = in->intersectingEntities();
		for (W4dEntities::const_iterator it = entities.begin(); it != entities.end(); ++it)
		{
			if (*it)
			{
				CULL_STREAM("processing intersecting entity " << (W4dEntity*) (*it) << "\n");
				renderTree(*it, DOMAIN_RENDER);
			}
		}

		if (in->hasLightList())
		{
			CULL_STREAM("lights off for " << (W4dEntity*) in << "\n");
			in->lightListForEdit().turnOffAll();
		}

		// Recursively traverse to neighbouring domains.
	    const W4dDomain::W4dPortals& portals = in->portals();
		for (W4dDomain::W4dPortals::const_iterator it = portals.begin(); it!=portals.end(); ++it)
		{
			const W4dPortal* portal = *it;

			// Don't traverse through closed portals.
			if (portal && portal->isOpenFrom(in))
			{
				W4dDomain* nextDomain = portal->otherDomain(in);
				CULL_STREAM("Considering " << nextDomain->name() << ": " );

				// Don't traverse if we have visited the domain previously during
				// this render pass.
				if (nextDomain->latestRenderPassId() != renderPassId())
				{
					CULL_STREAM("through " << portal->globalAperture() << " " );
					// Has the camera moved since the last render pass?
					if( lastRenderTransformKey() == globalTransform().key() )
					{
						// the camera has not moved, render the domain if it's been rendered at the last pass
						if( nextDomain->passId() == lastRenderPassId() )
						{
							CULL_STREAM("visible (since visible at last pass and camera has not moved)\n" );
							recursiveDomainRender(nextDomain, depth+1, maxDepth);
						}
						else
						{
							CULL_STREAM("not visible (since not visible at last pass and camera has not moved)\n" );
						}
					}
					else
					{
						// Do the most expensive test last: is the portal in our FOV.
						if( canSee( portal->globalAperture() ) )
						{
							CULL_STREAM("visible\n" );
							recursiveDomainRender(nextDomain, depth+1, maxDepth);
						}

						else
    	            	{
							CULL_STREAM("not visible\n");
	    	            }
					}
				}
				else
				{
					CULL_STREAM("processed already\n");
				}
			}
		}
	}

	CULL_STREAM("leaving " << (W4dEntity*) in << " depth=" << depth << "\n\n");
	CULL_INDENT(-2);
}

void W4dCamera::adaptToEnvironment( W4dEnvironment* env )
{
	MATHEX_SCALAR newYonClipDistance;
	if( env and env->fogOn() )
	{
		newYonClipDistance = 1.5 * env->fogVisibility();
		if( newYonClipDistance > savedYonClipDistance() )
			newYonClipDistance = savedYonClipDistance();
	}
	else
	{
		// reset to default
	 	newYonClipDistance = savedYonClipDistance();
	}
	// use RenCamera::yonClipCamera instead of its local implementation
	// the saved value of the clip distance is not overriden
	camera_->yonClipDistance( newYonClipDistance );
}

void W4dCamera::savedYonClipDistance( double dist )
{
	savedYonClipDistance_=dist;
}

double W4dCamera::savedYonClipDistance() const
{
	return savedYonClipDistance_;
}

void W4dCamera::hitherClipDistance(double dist)
{
	PRE(dist > 0 );

	camera_->hitherClipDistance( dist );
}

double W4dCamera::hitherClipDistance() const
{
	return camera_->hitherClipDistance();
}

void W4dCamera::yonClipDistance(double dist)
{
	PRE(dist > 0 );

	// save locally the clip distance to make sure that adaptToEnvironment() has sthg
	// to fall on in case the fog distance goes crazy
	savedYonClipDistance( dist );
	camera_->yonClipDistance( dist );
}

double W4dCamera::yonClipDistance() const
{
	return camera_->yonClipDistance();
}

// Only the vertical angle can be set.  The horizontal angle can be read.
void W4dCamera::verticalFOVAngle(double angle)
{
	PRE(angle > 0);
	camera_->verticalFOVAngle( angle );
}

double W4dCamera::verticalFOVAngle() const
{
	return camera_->verticalFOVAngle();
}

// The camera will be set-up such that the horizontal angle will
// be w/h*FOVAngle(), where w and h are the dimensions of the
// viewport/window.
double W4dCamera::horizontalFOVAngle() const
{
    //Get the aspect ratio of the viewport via the scenemanager's device
    RenDevice* pDevice = manager_->pDevice();
    double ratio = double( pDevice->windowWidth() ) /
                   double( pDevice->windowHeight() );

	return camera_->horizontalFOVAngle( ratio );
}

bool W4dCamera::canSee(const MexPoint3d& pt ) const
{
    return manager_->pDevice()->canSee( pt );
}

bool W4dCamera::canSee(const MexQuad3d& quad) const
{
//    return camera_->canSee( quad );
    return manager_->pDevice()->canSee( quad );
}

uint W4dCamera::entitiesRendered() const
{
	return entitiesRendered_;
}

uint W4dCamera::domainsRendered() const
{
	return domainsRendered_;
}

void W4dCamera::updateRenCamera()
{
    camera_->transform( globalTransform() );
}

W4dSceneManager* W4dCamera::manager() const
{
	return manager_;
}

RenCamera* W4dCamera::pRenCamera() const
{
	return camera_;
}

const MexTransform3dKey& W4dCamera::lastRenderTransformKey() const
{
	return lastRenderTransformKey_;
}

// virtual
void W4dCamera::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
    ostr << "Start W4dCamera " << (void*)this << std::endl;
    ostr << "End   W4dCamera " << (void*)this << std::endl;
}

//virtual
bool W4dCamera::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void perWrite( PerOstream& ostr, const W4dCamera& camera )
{
    const W4dEntity& base = camera;

    ostr << base;
}

void perRead( PerIstream& istr, W4dCamera& camera )
{
    W4dEntity& base = camera;

    istr >> base;
}

W4dCamera::W4dCamera( PerConstructor con )
: W4dEntity( con ),
  pVolume_( _NEW( W4dCameraVolume( *this ) ) ),
  lastPassId_( 0 )
{
}

void W4dCamera::isYonClippingEnabled( bool isIt )
{
    pVolume_->isYonClippingEnabled( isIt );
}

bool W4dCamera::isYonClippingEnabled() const
{
    return pVolume_->isYonClippingEnabled();
}
