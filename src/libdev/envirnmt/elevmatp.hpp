/*
 * E L E V M A T P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_ELEVMATP_HPP
#define _ENVIRNMT_ELEVMATP_HPP

#include "base/base.hpp"
#include "ctl/countptr.hpp"
#include "render/render.hpp"
#include "world4d/matplan.hpp"

class EnvSatellite;
class EnvElevationColourTable;

// A concrete material plan subclass which defines a material which changes
// with the elevation of a satellite.
class EnvElevationMaterialPlan : public W4dMaterialPlan
{
public:
    EnvElevationMaterialPlan(const RenMaterialVec&, const EnvSatellite&, const EnvElevationColourTable&);
    virtual ~EnvElevationMaterialPlan();

    // Always returns true -- this plan applies to any LOD.
    virtual bool isLODDefined( W4dLOD lodId ) const;

	// Return a vector of size nMaterials() all the elements will be the same
	// material which is controlled by the satellite and CLUT.  As this is 
	// intended for controlling the materials of satellites, the diffuse colour
	// is black and the emissive colour is taken from the table.  The time and
	// lod arguments are ignored.
    virtual const Ren::MaterialVecPtr& materialVec(const PhysRelativeTime&, W4dLOD) const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvElevationMaterialPlan& t );

private:
	Ren::MaterialVecPtr				materials_;
	const EnvSatellite&				satellite_;
	const EnvElevationColourTable&	clut_;
	
    // Operations deliberately revoked
    EnvElevationMaterialPlan( const EnvElevationMaterialPlan& );
    EnvElevationMaterialPlan& operator =( const EnvElevationMaterialPlan& );
    bool operator ==( const EnvElevationMaterialPlan& );
};


#endif

/* End ELEVMATP.HPP *************************************************/
