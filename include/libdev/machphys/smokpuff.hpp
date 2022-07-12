/*
 * S M O K P U F F . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSmokePuff

    A single puff of smoke
*/

#ifndef _MACHPHYS_SMOKPUFF_HPP
#define _MACHPHYS_SMOKPUFF_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/sprite3d.hpp"

#include "machphys/machphys.hpp"

class MachPhysSmokePuff : public W4dSprite3d
{
public:
    
    MachPhysSmokePuff(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysPuffType type,
        MATHEX_SCALAR size,
        MATHEX_SCALAR depthOffset );

    ~MachPhysSmokePuff();

    static  void preload();

    static  MachPhysPuffType randomPuff();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSmokePuff& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysSmokePuff );
    
private:
    // Operation deliberately revoked
    MachPhysSmokePuff( const MachPhysSmokePuff& );

    // Operation deliberately revoked
    MachPhysSmokePuff& operator =( const MachPhysSmokePuff& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSmokePuff& );

    static  RenMaterial material( MachPhysPuffType puffType );

    static  RenMaterial puff1();
    static  RenMaterial puff2();
    static  RenMaterial puff3();
    static  RenMaterial puff4();
    static  RenMaterial puff5();
    static  RenMaterial puff6();

    static  RenMaterial puff( const RenColour& colour );

    static  RenMaterial material( const string& textureFileName );
};

PER_DECLARE_PERSISTENT( MachPhysSmokePuff );
PER_READ_WRITE( MachPhysSmokePuff );

#endif

/* End SMOKPUFF.HPP *************************************************/
