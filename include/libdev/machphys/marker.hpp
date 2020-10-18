/*
 * M A R K E R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMarker

    Physical model consisting of a wire frame bounding box.
    The colour depends on whether indicated as permanent or temporary.
*/

#ifndef _MACHPHYS_MARKER_HPP
#define _MACHPHYS_MARKER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/custom.hpp"

//Forward refs
class MexAlignedBox3d;

//Orthodox canonical (revoked)
class MachPhysMarker : public W4dCustom
{
public:
    //ctor. Marker encloses boundary.
    //Colour depends on permanent flag.
    MachPhysMarker( W4dEntity* pParent, const W4dTransform3d& localTransform,
                    const MexAlignedBox3d& boundary, bool permanent = true, int percentageHp = 100 );

    //dtor
    virtual ~MachPhysMarker();

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMarker& t );

    PER_MEMBER_PERSISTENT( MachPhysMarker );
private:
    // Operations deliberately revoked
    MachPhysMarker( const MachPhysMarker& );
    MachPhysMarker& operator =( const MachPhysMarker& );
    bool operator ==( const MachPhysMarker& );

};

PER_READ_WRITE( MachPhysMarker );
PER_DECLARE_PERSISTENT( MachPhysMarker );

#endif

/* End MARKER.HPP ***************************************************/
