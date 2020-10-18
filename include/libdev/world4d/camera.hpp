/*
 * C A M E R A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _W4D_CAMERA_HPP
#define _W4D_CAMERA_HPP

#include "base/base.hpp"
#include "world4d/entity.hpp"
#include "mathex/t3dkey.hpp"

class MexPoint3d;
class MexQuad3d;
class RenCamera;
class W4dSceneManager;
class W4dCameraVolume;
class W4dEnvironment;

class W4dCamera : public W4dEntity
{
public:
	// PRE(pParent) -- doesn't make sense for a camera to be the root.
	// PRE(mgr)     -- a camera requires a scene manager.
    W4dCamera(W4dSceneManager* mgr, W4dEntity* pParent, const W4dTransform3d& localTransform);

    virtual ~W4dCamera();

	// Various camera properties.
	// PRE(dist > 0 );
	void   hitherClipDistance(double dist);		// default = 1
	double hitherClipDistance() const;

	void adaptToEnvironment( W4dEnvironment* env );

	// PRE(dist > 0 );
	void   yonClipDistance(double dist);		// default = 500
	double yonClipDistance() const;

	// Set the vertical field of view angle in radians.
	// Only the vertical angle can be set.  The horizontal angle can be read.
	// PRE(angle > 0 );
	void   verticalFOVAngle(double angle);		// default = 80 * PI / 180
	double verticalFOVAngle() const;

	// The camera will be set-up such that the horizontal angle will be
	// computed from the viewport's aspect ratio and the vertical angle.
	double horizontalFOVAngle() const;

	bool canSee(const MexPoint3d& pt ) const;
	bool canSee(const MexQuad3d& quad) const;
	// MexVec2 projectToScreen(const MexPoint3d& pt) const;

	W4dSceneManager* manager() const;

	const ulong& renderPassId() const { return passId_; };
	const ulong& lastRenderPassId() const { return lastPassId_; };
	const MexTransform3dKey& lastRenderTransformKey( void ) const;

	// Rendering statistics.
	uint   entitiesRendered() const;
	uint    domainsRendered() const;

	RenCamera* pRenCamera() const;

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dCamera );
    PER_FRIEND_READ_WRITE( W4dCamera );

protected:
    virtual void doDebugOutput( ostream& ostr );

private:
    //  Allow the scene manager render method to call the camera render methods.
    friend class W4dSceneManager;
    friend class W4dComposite;

    enum TraversalType { IN_ORDER_RENDER, DOMAIN_RENDER, HELD_OBJECT_RENDER };

	void renderTree(W4dEntity* node, TraversalType);
	void renderSubTree(W4dEntity* node);
	void recursiveDomainRender(W4dDomain* entering, int depth, int maxDepth);

	void updateRenCamera();

    //Enable/disable clipping of entities at the yon clip plane as far as pre-render testing concerned.
    //Default is enabled
    void isYonClippingEnabled( bool isIt );
    bool isYonClippingEnabled() const;

	void savedYonClipDistance( double );
	// PRE( dist >= 0 );
	double savedYonClipDistance( void ) const;

	// Render the tree of W4dEntities that this camera is attached to,
	// starting from the root of the tree and doing a traversal of the
	// entire entity tree.
	void inOrderRender();

	// Render the tree that this camera is attached to using domain and
	// portal culling.  maxDepth is the maximum number of domains to
	// traverse, i.e. the radius rendered measured in domains.  Eventually,
	// it will probably be more useful to measure this as a distance.
	// PRE(maxDepth > 0);							we must render something
	// PRE(intersectingDomains().size() == 1);		camera's domain must be uniquely set
	void domainRender(const int maxDepth);

	W4dSceneManager*	manager_;
	ulong				passId_;
	ulong				lastPassId_;
	MexTransform3dKey	lastRenderTransformKey_;
	uint				entitiesRendered_, domainsRendered_;

    //  TBD: remove these entirely
    void*               dummy1, *dummy2; // Was: UtlDebugTimer*		cullTimer_, *renderTimer_;

    RenCamera*          camera_;
    W4dCameraVolume*    pVolume_;//Object used to assist in view volume clipping
	double		 		savedYonClipDistance_;

    // Operations deliberately revoked.
    W4dCamera( const W4dCamera& );
    const W4dCamera& operator =( const W4dCamera& );
    bool operator ==( const W4dCamera& ) const;
};

PER_DECLARE_PERSISTENT( W4dCamera );

#endif
