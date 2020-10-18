/*
 * B B X S E L E C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "world4d/bbxselec.hpp"
#include "world4d/entity.hpp"
#include "world4d/domain.hpp"
#include "world4d/portal.hpp"
#include "world4d/camera.hpp"
#include "world4d/composit.hpp"

#include "render/device.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/transf3d.hpp"

W4dBoundingBoxSelector::W4dBoundingBoxSelector( const W4dCamera& camera, const MexQuad3d& boundingBox )
:   pCamera_( &camera ),
    localBoundingBox_( boundingBox ),
    clipAtFogDistance_( false ),
    ignoreNonSubjectEntities_( true )
{

    TEST_INVARIANT;
}

W4dBoundingBoxSelector::~W4dBoundingBoxSelector()
{
    TEST_INVARIANT;
}

void W4dBoundingBoxSelector::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dBoundingBoxSelector& t )
{

    o << "W4dBoundingBoxSelector " << (void*)&t << " start" << std::endl;
    o << "W4dBoundingBoxSelector " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dBoundingBoxSelector::updateGlobalData()
{
    //Get the camera's global origin
    globalCameraPosition_ = pCamera_->globalTransform().position();
    MexPoint3d localCameraPosition = pCamera_->localTransform().position();

    //Produce a plane normal for the plane through the camera and each side of the bounding box
    for( uint i = 0; i != 4; ++i )
    {
        //Get direction vectors to the 2 vertices for the edge
        const MexVec3 v1( localBoundingBox_.vertex( i ) );
        const MexVec3 v2( localBoundingBox_.vertex( (i + 1) % 4 ) );

        //Treating these as vectors from camera origin, take the cross product and normalise to get plane normal
        MexVec3 normal = MexVec3::crossProduct( v1, v2 );
        ASSERT( normal.modulus() > MexEpsilon::instance(), "Illegal bounding box" );
        normal.makeUnitVector();

        //Now convert into global coords
        pCamera_->globalTransform().transform( &normal );
        aGlobalPlaneNormals_[i] = normal;
    }

    //We also need a plane to clip behind the camera
    aGlobalPlaneNormals_[4] = pCamera_->globalTransform().xBasis();
    aGlobalPlaneNormals_[4] *= -1.0;
}

bool W4dBoundingBoxSelector::pointOutsidePlane( uint i, const MexPoint3d& p ) const
{
    PRE( i < 5 );

    return MexVec3( globalCameraPosition_, p ).dotProduct( aGlobalPlaneNormals_[i] ) > 0.0;
}

bool W4dBoundingBoxSelector::pointOutsideTrapezoid( const MexPoint3d& p ) const
{
    //Check each of the bounding planes
    MexVec3 v( globalCameraPosition_, p );
    bool inside = true;
    for( uint i = 5; inside  and  i--; )
        inside = v.dotProduct( aGlobalPlaneNormals_[i] ) <= 0.0;

    return not inside;
}

bool W4dBoundingBoxSelector::entitySelected( const W4dEntity& entity ) const
{
    //Check for associated subject if required
    bool selected = not ignoreNonSubjectEntities_  or  entity.hasSubject();
    if( selected )
    {
		//Get the entity's global transform
   		const MexTransform3d& entityTransform = entity.globalTransform();

		//Get the bounding volume of the entity
		const MexAlignedBox3d& entityBoundary = (entity.isComposite() ? entity.asComposite().compositeBoundingVolume() : entity.boundingVolume());

		//Get the entity's centroid
		MexPoint3d entityPosition = entityBoundary.centroid();

		// Apply the global transform to the centroid
		entityTransform.transform( &entityPosition );

        //If rejecting on fog distance...
        if( clipAtFogDistance_ )
        {
            //Compute square of distance to camera
            MATHEX_SCALAR sqrCameraDistance = entityPosition.sqrEuclidianDistance( globalCameraPosition_ );

            //Check squared distance
            RenDevice* pDevice = RenDevice::current();
            //if( pDevice->isFogOn() )	// Not all cards support foging!!
            {
                MATHEX_SCALAR fogEnd = pDevice->fogEnd();
                selected = sqrCameraDistance < fogEnd * fogEnd;
            }
        }

        if( selected )
            selected = not pointOutsideTrapezoid( entityPosition );
    }

    //If still selected, check the origin lies inside the trapezoid
    return selected;
}

void W4dBoundingBoxSelector::addTreeEntities( W4dEntity* pEntity, Entities* pEntities ) const
{
    //Add this entity if selected
    if( pEntity->checkId() != checkId_  and  pEntity->visible() )
    {
        //Add to the list if selected
        if( entitySelected( *pEntity ) )
        {
            pEntities->push_back( pEntity );
        }

        //Mark as checked
        pEntity->checkId( checkId_ );

        //Deal recursively with any children
        const W4dEntity::W4dEntities& children = pEntity->children();
        for( W4dEntity::W4dEntities::const_iterator it = children.begin(); it != children.end(); ++it )
            addTreeEntities( (*it), pEntities );
    }
}

void W4dBoundingBoxSelector::addDomainEntities( W4dDomain* pDomain, Entities* pEntities ) const
{
    //Deal with the domain and its children
    addTreeEntities( pDomain, pEntities );

    //Deal with any entities having intersect relations with the domain
    const W4dEntity::W4dEntities& intersectingEntities = pDomain->intersectingEntities();
    for( W4dEntity::W4dEntities::const_iterator it = intersectingEntities.begin(); it != intersectingEntities.end(); ++it )
        addTreeEntities( (*it), pEntities );

    //Now iterate through the domain's portals
    const W4dDomain::W4dPortals& portals = pDomain->portals();
    for( W4dDomain::W4dPortals::const_iterator it = portals.begin(); it != portals.end(); ++it )
    {
		W4dPortal* pPortal = *it;

		// Don't traverse through closed portals.
		if( pPortal->isOpenFrom( pDomain ) )
		{
            //Check the next domain hasn't already been processed
			W4dDomain* pNextDomain = pPortal->otherDomain( pDomain );
            if( pNextDomain->checkId() != checkId_ )
            {
                //Check the portal intersects the trapezoid, and if so do the next domain recursively
                if( portalIntersects( pPortal ) )
                    addDomainEntities( pNextDomain, pEntities );
            }
        }
    }
}

bool W4dBoundingBoxSelector::portalIntersects( W4dPortal* pPortal ) const
{
    const MexQuad3d& portalAperture = pPortal->globalAperture();

    //check every point lies outside one of the four planes
    bool intersects = true;
    for( uint i = 0; intersects  and  i != 5; ++i )
    {
        bool outsidePlane = true;
        for( uint j = 0; outsidePlane and j != 4; ++j )
        {
            //Get the portal vertex
            const MexPoint3d& portalVertex = portalAperture.vertex( j );

            //check outside the current plane
            outsidePlane = pointOutsidePlane( i, portalVertex );
        }

        //If all points outside any one plane, can't intersect
        intersects = not outsidePlane;
    }

    return intersects;
}

W4dBoundingBoxSelector::Entities W4dBoundingBoxSelector::selectedEntities() const
{
    //Construct a collection to return
    Entities entities;
    entities.reserve( 256 );

    //update the cached data
    W4dBoundingBoxSelector* pSelector = _CONST_CAST( W4dBoundingBoxSelector*, this );
    pSelector->updateGlobalData();

    //Get a check id to use for this check
    pSelector->checkId_ = W4dEntity::nextCheckId();

    //get the camera's domain
    W4dDomain* pCameraDomain = NULL;
    bool gotDomain = pCamera_->hasContainingDomain( &pCameraDomain );
    ASSERT( gotDomain, "selecting camera must be inside a domain" );
    if( pCameraDomain )
        addDomainEntities( pCameraDomain, &entities );

    return entities;
}

void W4dBoundingBoxSelector::boundingBox( const MexQuad3d& boundingBox )
{
    localBoundingBox_ = boundingBox;
}

const MexQuad3d& W4dBoundingBoxSelector::boundingBox() const
{
    return localBoundingBox_;
}

void W4dBoundingBoxSelector::clipAtFogDistance( bool doClip )
{
    clipAtFogDistance_ = doClip;
}

bool W4dBoundingBoxSelector::clipAtFogDistance() const
{
    return clipAtFogDistance_;
}

void W4dBoundingBoxSelector::ignoreNonSubjectEntities( bool doIgnore )
{
    ignoreNonSubjectEntities_ = doIgnore;
}

bool W4dBoundingBoxSelector::ignoreNonSubjectEntities() const
{
    return ignoreNonSubjectEntities_;
}
/* End BBXSELEC.CPP *************************************************/
