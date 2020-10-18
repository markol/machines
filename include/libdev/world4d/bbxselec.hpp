/*
 * B B X S E L E C . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dBoundingBoxSelector

    Used to identify the entities which lie within view from a camera,
    seen through a quadrilateral in its projection plane.
*/

#ifndef _WORLD4D_BBXSELEC_HPP
#define _WORLD4D_BBXSELEC_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "mathex/quad3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

//forward refs
class W4dCamera;
class W4dEntity;
class W4dDomain;
class W4dPortal;

class W4dBoundingBoxSelector
// Canonical form revoked
{
public:
    //ctor. The camera is supplied. boundingBox is the rectangle in the camera's projection plane,
    //with the coordinates being defined in the local axis system of the camera.
    //The quad coordinates must be in clockwise order as seen from the camera.
    W4dBoundingBoxSelector( const W4dCamera& camera, const MexQuad3d& boundingBox );

    //dtor
    ~W4dBoundingBoxSelector();

    typedef ctl_pvector< W4dEntity > Entities;

    //Set/get the bounding box, defined as in the ctor
    void boundingBox( const MexQuad3d& boundingBox );
    const MexQuad3d& boundingBox() const;

    //Set/get flag indicating whether entities further from teh camera than the fog distance
    //are to be ignored. (Default is false).
    void clipAtFogDistance( bool doClip );
    bool clipAtFogDistance() const;

    //Set/get flag indicating whether W4dEntitys with no assocaited W4dSubject are to be
    //ignored during the selection call. Default is true.
    void ignoreNonSubjectEntities( bool doIgnore );
    bool ignoreNonSubjectEntities() const;

    //The set of entities visible through the bounding box from the camera
    Entities selectedEntities() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dBoundingBoxSelector& t );

private:
    W4dBoundingBoxSelector( const W4dBoundingBoxSelector& );
    W4dBoundingBoxSelector& operator =( const W4dBoundingBoxSelector& );
    bool operator ==( const W4dBoundingBoxSelector& );

    //Update the cached plane data
    void updateGlobalData();

    //True if p lies outside (ie on the normal direction side) of the ith cached plane
    bool pointOutsidePlane( uint i, const MexPoint3d& p ) const;

    //true if p lies outside all the bounding planes
    bool pointOutsideTrapezoid( const MexPoint3d& p ) const;

    //True if pPortal can intersect the selection trapezoid
    bool portalIntersects( W4dPortal* pPortal ) const;

    //true if the origin of entity is selected inside the trapezoid and visibility range
    bool entitySelected( const W4dEntity& entity ) const;

    //Recursivley add pEntity and any of its descendants to pEntities if it or they meet the
    //selection criteria
    void addTreeEntities( W4dEntity* pEntity, Entities* pEntities ) const;

    //Recursivley add pDomain and any of its descendants to pEntities if it or they meet the
    //selection criteria. Also recursively deal with adjacent domains, if the adjoining portal
    //intersects the selection volume.
    void addDomainEntities( W4dDomain* pDomain, Entities* pEntities ) const;

    //data members
    const W4dCamera* pCamera_; //The camera doing the seeing
    MexQuad3d localBoundingBox_; //The projection plane bounding box
    MexPoint3d globalCameraPosition_; //Global camera origin
    MexVec3 aGlobalPlaneNormals_[5]; //The 5 bounding plane normals
    ulong checkId_; //The id in use for a single pass selection check
    bool clipAtFogDistance_; //true if entities further from the camera than the fog distance are to be ignored
    bool ignoreNonSubjectEntities_; //true if entities with no associated subject are to be ignored
};


#endif

/* End BBXSELEC.HPP *************************************************/
