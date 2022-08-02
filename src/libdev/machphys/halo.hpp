/*
 * H A L O . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLightningHalo
    Models the lightning effect of an electric charger weapon on the victim
*/

#ifndef _MACHPHYS_HALO_HPP
#define _MACHPHYS_HALO_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MachPhysLightningHalo : public W4dEntity
// Canonical form revoked
{
public:

	enum HaloType{ELECTRIC, VIRUS};
    //ctor
    MachPhysLightningHalo( W4dEntity* pParent, const MexTransform3d& localTransform , HaloType type);

    //Return an exemplar Lightning Halo - ensures the meshes and textures are loaded
    static const MachPhysLightningHalo& exemplar(HaloType type);

    //dtor
    ~MachPhysLightningHalo();

     //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
	//lightning animation on pVictim
	//sign = 1 : the halo travels to the top of the victim
	// sign = -1: the halo trvels to the bottom of the victim
	void startLightning
	(
		W4dEntity* pVictim,
		const PhysAbsoluteTime& startTime,
		const PhysRelativeTime& duration,
		const int& sign,
		const HaloType type
	);
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLightningHalo& t );

    PER_MEMBER_PERSISTENT( MachPhysLightningHalo );

private:
    //Deliberately revoked
    MachPhysLightningHalo( const MachPhysLightningHalo& );
    MachPhysLightningHalo& operator =( const MachPhysLightningHalo& );
    bool operator ==( const MachPhysLightningHalo& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysLightningHalo(HaloType type);

};

PER_READ_WRITE( MachPhysLightningHalo );
PER_DECLARE_PERSISTENT( MachPhysLightningHalo );

#endif

/* End HALO.HPP *************************************************/
