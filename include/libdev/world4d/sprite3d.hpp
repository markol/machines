/*
 * S P R I T E 3 D. H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dSprite3d

    A 3D sprite which has a position in space and will always face the camera
*/

#ifndef _WORLD4D_SPRITE_HPP
#define _WORLD4D_SPRITE_HPP

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
class MexRadians;
class MexLine3d;

class W4dSprite3d : public W4dEntity
{
public:
    W4dSprite3d(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      MATHEX_SCALAR width,
      MATHEX_SCALAR height,
      const RenMaterial& materialPtr );

    //Alternative ctor supplying uv coords
    W4dSprite3d(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      MATHEX_SCALAR width,
      MATHEX_SCALAR height,
      const RenMaterial& materialPtr,
      const MexVec2& uv0, const MexVec2& uv1, const MexVec2& uv2, const MexVec2& uv3 );

    ~W4dSprite3d();

    void depthOffset( MATHEX_SCALAR offset );

    //Will only be visible up to distance metres
    void visibleRange( MATHEX_SCALAR distance );

    //  Note that some tiling effects might show up at non 90 degree angles
    void rotate( const MexRadians& angle );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dSprite3d );
    PER_FRIEND_READ_WRITE( W4dSprite3d );

private:
    // Operation deliberately revoked
    W4dSprite3d( const W4dSprite3d& );

    // Operation deliberately revoked
    W4dSprite3d& operator =( const W4dSprite3d& );

    // Operation deliberately revoked
    bool operator ==( const W4dSprite3d& );

    Ren::MeshPtr        pMesh_;
    RenMeshInstance*    pMeshInstance_;

    size_t              startFrame_;
};

PER_DECLARE_PERSISTENT( W4dSprite3d );


#endif

/* End SPRITE3D.HPP ***************************************************/
