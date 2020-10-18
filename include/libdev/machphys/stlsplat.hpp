/*
 * S T I N G S P L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLightStingSplat

    The WASP LIGHT_STING impact
*/

#ifndef _MACHPHYS_STINGSPL_HPP
#define _MACHPHYS_STINGSPL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/composit.hpp"


class MachPhysLightStingSplat : public W4dComposite
// Canonical form revoked
{
public:
    //ctor
    MachPhysLightStingSplat( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysLightStingSplat& exemplar();

    //dtor
    virtual ~MachPhysLightStingSplat();

    //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

	void startLightStingSplat( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

	static void preloadTextures();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLightStingSplat& t );

    PER_MEMBER_PERSISTENT( MachPhysLightStingSplat );

private:
    //Deliberately revoked
    MachPhysLightStingSplat( const MachPhysLightStingSplat& );
    MachPhysLightStingSplat& operator =( const MachPhysLightStingSplat& );
    bool operator ==( const MachPhysLightStingSplat& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLightStingSplat();

};

PER_DECLARE_PERSISTENT( MachPhysLightStingSplat );
PER_READ_WRITE( MachPhysLightStingSplat );

#endif

/* End STINGSPL.HPP *************************************************/
