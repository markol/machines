/*
 * S P A R K S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSparks

    part of the construction constructing animation
*/

#ifndef _MACHPHYS_SPARKS_HPP
#define _MACHPHYS_SPARKS_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"


class MachPhysSparks : public W4dComposite
// Canonical form revoked
{
public:
    //ctor
    MachPhysSparks( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysSparks& exemplar();

    //dtor
    ~MachPhysSparks();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSparks& t );

    PER_MEMBER_PERSISTENT( MachPhysSparks );

private:
    //Deliberately revoked
    MachPhysSparks( const MachPhysSparks& );
    MachPhysSparks& operator =( const MachPhysSparks& );
    bool operator ==( const MachPhysSparks& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysSparks();

};

PER_READ_WRITE( MachPhysSparks );
PER_DECLARE_PERSISTENT( MachPhysSparks );


#endif

/* End SPARKS.HPP *************************************************/
