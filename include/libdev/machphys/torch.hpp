/*
 * T O R C H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysTorch

    part of the construction constructing animation
*/

#ifndef _MACHPHYS_TORCH_HPP
#define _MACHPHYS_TORCH_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"


class MachPhysTorch : public W4dComposite
// Canonical form revoked
{
public:
    //ctor
    MachPhysTorch( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysTorch& exemplar();

    //dtor
    ~MachPhysTorch();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTorch& t );

    PER_MEMBER_PERSISTENT( MachPhysTorch );

private:
    //Deliberately revoked
    MachPhysTorch( const MachPhysTorch& );
    MachPhysTorch& operator =( const MachPhysTorch& );
    bool operator ==( const MachPhysTorch& );

    //One-time constructor used to create the exemplar
    MachPhysTorch();

};

PER_READ_WRITE( MachPhysTorch );
PER_DECLARE_PERSISTENT( MachPhysTorch );


#endif

/* End TORCH.HPP *************************************************/
