/*
 * A R T E F A C T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysArtefact

    Models a terrain artefact.
*/

#ifndef _MACHPHYS_ARTEFACT_HPP
#define _MACHPHYS_ARTEFACT_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machphys/artfdata.hpp"

//forward refs
class W4dEntity;
class MexConvexPolygon2d;
class MachPhysEntityDamage;
class MexPoint3d;

class MachPhysArtefact
// Canonical form revoked
{
public:
    //ctor. pModel is the model representing the artefact.
    //Object and artefact specific info can be obtained from data.
    MachPhysArtefact( W4dEntity* pModel, const MachPhysArtefactData& data );

    //dtor. Does NOT delete the physical model entity passed in the ctor.
    //This must be executed by the client.
    ~MachPhysArtefact();

    //The data object for this artefact
    const MachPhysArtefactData& artefactData() const;

    //Returns a new obstacle polygon which has global coordinates derived from the
    //current global position of the artefact model.
    //Client to assume ownership of the polygon.
    MexConvexPolygon2d* newGlobalBoundary() const;

    //The entity model for the artefact
    W4dEntity& physEntity();
    const W4dEntity& physEntity() const;

    //////////////////////////////////////////////////
    // Damage indication

    //Set get the current damage level as a percentage
	void damageLevel( const double& percent );
	const double damageLevel() const;

	//Allows the damage animation to proceed correctly. Should be called
    //approx every 60 seconds.
	void updateDamageLevel();

	bool damaged() const;
    //////////////////////////////////////////////////

    //Invoke a destruction animation at startTime, returning the duration of the animation.
    //This object can be deleted immediately after this call, but the animation will proceed
    //as requested.
    PhysRelativeTime destroy( const PhysAbsoluteTime& startTime );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysArtefact& t );

    MachPhysArtefact( const MachPhysArtefact& );
    MachPhysArtefact& operator =( const MachPhysArtefact& );

    //data members
    W4dEntity* pModel_; //The artefact model in the render tree
    const MachPhysArtefactData* pData_; //The data object for this artefact
    MachPhysEntityDamage* pDamage_; //If damaged, stores the progress of the damage animation
};

#ifdef _INLINE
    #include "machphys/artefact.ipp"
#endif


#endif

/* End ARTEFACT.HPP *************************************************/
