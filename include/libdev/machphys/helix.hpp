/*
 * H E L I X . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysHelix

    A left and right version of a helix strip used in various effects.
*/

#ifndef _MACHPHYS_HELIX_HPP
#define _MACHPHYS_HELIX_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"

class MachPhysHelix : public W4dEntity
// Canonical form revoked
{
public:
    //Constructs appropriate
    enum Twist {LEFT_TWIST, RIGHT_TWIST};
    MachPhysHelix( W4dEntity* pParent, const MexTransform3d& localTransform, Twist twist );

    //dtor
    virtual ~MachPhysHelix();

    //Return an exemplar helix - ensures the mesh is loaded
    static const MachPhysHelix& exemplar( Twist twist );

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysHelix& t );

    PER_MEMBER_PERSISTENT( MachPhysHelix );

private:
    MachPhysHelix( const MachPhysHelix& );
    MachPhysHelix& operator =( const MachPhysHelix& );
    bool operator ==( const MachPhysHelix& );

	friend class  MachPhysOtherPersistence;
    //One-time constructor (per twist) used to create the exemplars
    MachPhysHelix( Twist twist );
};

PER_DECLARE_PERSISTENT( MachPhysHelix );
PER_READ_WRITE( MachPhysHelix );

#endif

/* End HELIX.HPP ****************************************************/
