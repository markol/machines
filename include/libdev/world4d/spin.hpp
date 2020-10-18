/*
 * S P I N . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    W4dSpinSprite

    A 3D sprite (spin to face) which has a position in space and will always face the camera
*/

#ifndef _WORLD4D_SPIN_HPP
#define _WORLD4D_SPIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "render/render.hpp"
#include "world4d/entity.hpp"
#include "mathex/mathex.hpp"

class RenMesh;
class RenMaterial;
class RenMeshInstance;
class MexVec2;
class MexVec3;
class MexRadians;
class MexLine3d;

class W4dSpinSprite : public W4dEntity
{
public:
    W4dSpinSprite(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      MATHEX_SCALAR width,
      MATHEX_SCALAR height,
      const RenMaterial& material,
      const MexVec3& spinAxis );

    ~W4dSpinSprite( void );

    //Will only be visible up to distance metres
    void visibleRange( MATHEX_SCALAR distance );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dSpinSprite );
    PER_FRIEND_READ_WRITE( W4dSpinSprite );

private:
    // Operation deliberately revoked
    W4dSpinSprite( const W4dSpinSprite& );

    // Operation deliberately revoked
    W4dSpinSprite& operator =( const W4dSpinSprite& );

    // Operation deliberately revoked
    bool operator ==( const W4dSpinSprite& );

    Ren::MeshPtr        pMesh_;
    RenMeshInstance*    pMeshInstance_;

    size_t              startFrame_;
};

PER_DECLARE_PERSISTENT( W4dSpinSprite );


#endif

/* End SPIN.HPP ***************************************************/
