/*
 * E L E V M A T P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/elevmatp.hpp"
#include "envirnmt/satelite.hpp"
#include "envirnmt/elevclut.hpp"
#include "render/matvec.hpp"

EnvElevationMaterialPlan::EnvElevationMaterialPlan
(
	const RenMaterialVec& vec,
	const EnvSatellite& sat,
	const EnvElevationColourTable& clut
):
	W4dMaterialPlan(HUGE_VAL, 100000),		// duration & max LOD: both effectively infinite
	materials_(_NEW(RenMaterialVec(vec))),
	satellite_(sat),
	clut_(clut)
{
	// Set the diffuse part of each material to black.
	RenMaterialVec::iterator it = materials_->begin();
	while (it != materials_->end())
	{
		// As these materials are modified each frame, the overhead of sharing
		// will be too much (and the material manager is unlikely to find
		// matches because the colours will probably be unique.)
		(*it).makeNonSharable();
		(*it).diffuse(RenColour::black());
		++it;
	}

    TEST_INVARIANT;
}

// virtual
EnvElevationMaterialPlan::~EnvElevationMaterialPlan()
{
    TEST_INVARIANT;
}

// virtual
bool EnvElevationMaterialPlan::isLODDefined(W4dLOD) const
{
	return true;
}

// virtual
const Ren::MaterialVecPtr& EnvElevationMaterialPlan::materialVec(const PhysRelativeTime&, W4dLOD) const
{
	const RenColour col = clut_.colour(satellite_.elevation());
	const RenColour blackAlpha(0,0,0, col.a());

	// Set the diffuse part of each material to black.
	RenMaterialVec::iterator it = materials_->begin();
	while (it != materials_->end())
	{
		(*it).diffuse(blackAlpha);
		(*it).emissive(col);
		++it;
	}

	return materials_;
}

void EnvElevationMaterialPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const EnvElevationMaterialPlan& t )
{
    o << "EnvElevationMaterialPlan " << (void*)&t << " start" << std::endl;
    o << "EnvElevationMaterialPlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ELEVMATP.CPP *************************************************/
