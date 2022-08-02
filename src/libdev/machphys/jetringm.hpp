/*
 * J E T R I N G M . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysJetRing

    part of the construction constructing animation
*/

#ifndef _MACHPHYS_JETRINGM_HPP
#define _MACHPHYS_JETRINGM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"

class W4dLink;

class MachPhysJetRing : public W4dComposite
// Canonical form revoked
{
public:
    //ctor
    MachPhysJetRing( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysJetRing& exemplar();

    //dtor
    ~MachPhysJetRing();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startGlow( const PhysAbsoluteTime& startTime, const MexPoint3d& targetOffsetGlobal );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysJetRing& t );

    PER_MEMBER_PERSISTENT( MachPhysJetRing );
    PER_FRIEND_READ_WRITE( MachPhysJetRing );

private:
    //Deliberately revoked
    MachPhysJetRing( const MachPhysJetRing& );
    MachPhysJetRing& operator =( const MachPhysJetRing& );
    bool operator ==( const MachPhysJetRing& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysJetRing();

	W4dLink* pTorch_;
};

PER_DECLARE_PERSISTENT( MachPhysJetRing );


#endif

/* End JETRINGM.HPP *************************************************/
