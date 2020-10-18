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

    ~MachPhysSmokePuff( void );

    static  void preload( void );

    static  MachPhysPuffType randomPuff( void );

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

    static  RenMaterial puff1( void );
    static  RenMaterial puff2( void );
    static  RenMaterial puff3( void );
    static  RenMaterial puff4( void );
    static  RenMaterial puff5( void );
    static  RenMaterial puff6( void );

    static  RenMaterial puff( const RenColour& colour );

    static  RenMaterial material( const string& textureFileName );
};

PER_DECLARE_PERSISTENT( MachPhysSmokePuff );
PER_READ_WRITE( MachPhysSmokePuff );

#endif

/* End SMOKPUFF.HPP *************************************************/
