/*
 * P A R T I C L E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysParticles
    Models part of a vortex weapon bomb explosion animation
*/

#ifndef _MACHPHYS_PARTICLE_HPP
#define _MACHPHYS_PARTICLE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "world4d/entity.hpp"

class MexPoint3d;

class MachPhysParticles : public W4dEntity
// Canonical form revoked
{
public:
    //ctor
    MachPhysParticles( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Return an exemplar vortex bomb - ensures the bomb meshes and textures are loaded
    static const MachPhysParticles& exemplar();

    //dtor
    ~MachPhysParticles();

     //Inherited from W4dEntity. Returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysParticles& t );

    PER_MEMBER_PERSISTENT( MachPhysParticles );

private:
    //Deliberately revoked
    MachPhysParticles( const MachPhysParticles& );
    MachPhysParticles& operator =( const MachPhysParticles& );
    bool operator ==( const MachPhysParticles& );
	void createSTFParticle(const Ren::MeshPtr& mesh);

	friend class MachPhysOtherPersistence;
	//one time ctor
    MachPhysParticles();

};

PER_DECLARE_PERSISTENT( MachPhysParticles );
PER_READ_WRITE( MachPhysParticles );

#endif

/* End VORTBOMB.HPP *************************************************/
