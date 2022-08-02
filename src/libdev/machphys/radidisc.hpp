/*
 * R A D I D I S C . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysRadialDisc

    Models a disc which is used in various effects.
    The unscaled size is +/- 0.2m.
*/

#ifndef _MACHPHYS_RADIDISC_HPP
#define _MACHPHYS_RADIDISC_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"

//forward refs
class MexTransform3d;

class MachPhysRadialDisc : public W4dEntity
// Canonical form revoked
{
public:
    //ctor. Object attached to pParent at localTransform
    MachPhysRadialDisc( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysRadialDisc();

    //Return an exemplar disc - ensures the disc mesh is loaded
    static const MachPhysRadialDisc& exemplar();

    //The cutout texture that converts the disc into a ring
    static const RenTexture& circleTexture();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysRadialDisc& t );

    PER_MEMBER_PERSISTENT( MachPhysRadialDisc );

private:
    MachPhysRadialDisc( const MachPhysRadialDisc& );
    MachPhysRadialDisc& operator =( const MachPhysRadialDisc& );
    bool operator ==( const MachPhysRadialDisc& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysRadialDisc();

};

PER_READ_WRITE( MachPhysRadialDisc );
PER_DECLARE_PERSISTENT( MachPhysRadialDisc );

#endif

/* End RADIDISC.HPP *************************************************/
