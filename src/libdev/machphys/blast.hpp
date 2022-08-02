/*
 * B L A S T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBlast

    Models part of a gorrila punch animation
*/

#ifndef _MACHPHYS_BLAST_HPP
#define _MACHPHYS_BLAST_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"


class MachPhysBlast : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysBlast( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysBlast& exemplar();

    //dtor
    ~MachPhysBlast();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

	void startBlast( const PhysAbsoluteTime& startTime );

    friend ostream& operator <<( ostream& o, const MachPhysBlast& t );

    PER_MEMBER_PERSISTENT( MachPhysBlast );

private:
    //Deliberately revoked
    MachPhysBlast( const MachPhysBlast& );
    MachPhysBlast& operator =( const MachPhysBlast& );
    bool operator ==( const MachPhysBlast& );

	friend class MachPhysOtherPersistence;

    //One-time constructor used to create the exemplar
    MachPhysBlast();

};

PER_READ_WRITE( MachPhysBlast );
PER_DECLARE_PERSISTENT( MachPhysBlast );


#endif

/* End BLAST.HPP *************************************************/
