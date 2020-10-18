/*
 * C A M V O L U M . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCameraVolume

    Represents the frustrum associated with a W4dCamera, and is used for culling W4dEntitys
    which do not need to be rendered.
*/

#ifndef _WORLD4D_CAMVOLUM_HPP
#define _WORLD4D_CAMVOLUM_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

//forward refs
class W4dCamera;
class W4dEntity;

class W4dCameraVolume
// Canonical form revoked
{
public:
    //The camera ref is stored, so client must delete this if the camera is deleted.
    W4dCameraVolume( const W4dCamera& camera );

    //dtor.
    ~W4dCameraVolume();

    //Enable/disable whether intersects() method affected by the yon clip plane.
    //Default is enabled
    void isYonClippingEnabled( bool isIt );
    bool isYonClippingEnabled() const;

    //Updates the cached data used in clipping.
    //Should be called once per frame before rendering.
    void update();

    //True if the entity may intersect the camera's view volume
    bool intersects( const W4dEntity& entity ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dCameraVolume& t );

private:
    W4dCameraVolume( const W4dCameraVolume& );
    W4dCameraVolume& operator =( const W4dCameraVolume& );
    bool operator ==( const W4dCameraVolume& );

    //data members
    const W4dCamera* pCamera_; //The camera being clipped against
    MexPoint3d nearPlanePoint_;//Point on near clipping plane (global)
    MexPoint3d farPlanePoint_;//Point on far clipping plane (global)
    MexPoint3d eyePoint_;//Point at eye (global)
    MexVec3 lineOfSight_;//Vector along line of sight (global)
    MexVec3 horizontalRightNormal_;//Clipping plane normal (global)
    MexVec3 horizontalLeftNormal_;//Clipping plane normal (global)
    MexVec3 verticalUpNormal_;//Clipping plane normal (global)
    MexVec3 verticalDownNormal_;//Clipping plane normal (global)
    bool clipFarPlane_; //True if clipping at yon clip plne is required
};


#endif

/* End CAMVOLUM.HPP *************************************************/
