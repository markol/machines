/*
 * F L A M E S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    Flames

    A brief description of the class should go in here
*/

#ifndef _FLAMES_HPP
#define _FLAMES_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "render/render.hpp"
#include "world4d/sprite3d.hpp"

class RenTexture;
class MexTransform3d;

enum MachPhysFlameType : unsigned char { FLAME_1 };

class MachPhysFlame : public W4dSprite3d
{
public:
    //  Construct the flame and set it going at the given startTime
    MachPhysFlame(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFlameType flameType,
        MATHEX_SCALAR width,
        MATHEX_SCALAR height,
        MATHEX_SCALAR depthOffset,
        const PhysAbsoluteTime& startTime,
        const PhysRelativeTime& duration );

    //  Construct the flame but don't start it going yet
    MachPhysFlame(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFlameType flameType,
        MATHEX_SCALAR size,
        MATHEX_SCALAR depthOffset,
        const PhysRelativeTime& duration );

	//a rectangular flame
    MachPhysFlame(
        W4dEntity* pParent,
        const MexTransform3d& localTransform,
        MachPhysFlameType flameType,
        MATHEX_SCALAR width,
        MATHEX_SCALAR height,
        MATHEX_SCALAR depthOffset,
        const PhysRelativeTime& duration );

    ~MachPhysFlame( void );

    //  Start the flame at the given time, and turning off after the duration
    //defined in the constructor.
    void startFlame( const PhysAbsoluteTime& startTime );

    //Applies the material plan to get the flame burning from startTime,
    //and makes immediately visible. However does not apply a visibility plan to turn
    //the flame on/off. The client is left to do this.
    void startBurning( const PhysAbsoluteTime& startTime );

    //  Load all of the flame bitmaps in advance so that the
    //  flame can be created 'instantly'

    static  void preload( void );

    //  return a random flame type out of those available
    static  MachPhysFlameType randomFlame( void );

    typedef ctl_vector< Ren::MaterialVecPtr > Materials;
    typedef CtlCountedPtr< Materials > MaterialsPtr;

    static  const MaterialsPtr&   materialsPtr( MachPhysFlameType flameType );
    static  RenMaterial initialMaterial( MachPhysFlameType );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysFlame );
    PER_FRIEND_READ_WRITE( MachPhysFlame );

private:
    // Operations deliberately revoked
    MachPhysFlame( const MachPhysFlame& );
    MachPhysFlame& operator =( const MachPhysFlame& );
    bool operator ==( const MachPhysFlame& );

    static  const MaterialsPtr&   materials1Ptr( void );
    static  void addMaterial( Materials* pMaterials, const RenTexture& texture );

    //data members
    PhysRelativeTime duration_;
    MachPhysFlameType flameType_;
};

PER_DECLARE_PERSISTENT( MachPhysFlame );

PER_ENUM_PERSISTENT( MachPhysFlameType );

ostream& operator <<( ostream&, MachPhysFlameType );

#endif

/* End FLAMES.HPP ***************************************************/
