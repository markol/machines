/*
 * H E A L T H M A . C P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#include "machphys/internal/healthma.hpp"
#include "render/colour.hpp"

MachPhysIHealthMaterials::MachPhysIHealthMaterials(int nMats, const RenTexture& t, double alpha):
	nMats_(nMats),
	materials_(_NEW_ARRAY(RenMaterial, nMats)),
	texture_(t),
	alpha_(alpha)
{
	PRE(nMats > 1);
	PRE(alpha >= 0);
	PRE(alpha <= 1);
	sharedCtor();
}

MachPhysIHealthMaterials::MachPhysIHealthMaterials(int nMats, double alpha):
	nMats_(nMats),
	materials_(_NEW_ARRAY(RenMaterial, nMats)),
	texture_(),
	alpha_(alpha)
{
	PRE(nMats > 1);
	PRE(alpha >= 0);
	PRE(alpha <= 1);
	sharedCtor();
}

void MachPhysIHealthMaterials::sharedCtor()
{
	const RenColour markerBlue(0.0, 122.0/255.0, 179.0/255.0);
	const RenColour diffuse(0,0,0, alpha_);
	blue_.makeNonSharable();
	blue_.emissive(markerBlue);
	blue_.diffuse(diffuse);
	blue_.texture(texture_);

	for (int i=0; i!=nMats_; ++i)
	{
		const double percent = 100.0 * i / (nMats_-1);
		materials_[i].makeNonSharable();
		materials_[i].diffuse(diffuse);
		materials_[i].emissive(computeColour(percent));
		materials_[i].texture(texture_);

		// Give the markers a high absolute priority.  Otherwise they can be
		// incorrectly sorted against alpha'd terrain etc.  The choice of value
		// shouldn't really be hardcoded.  It is chosen to be higher than most
		// things, but lower than the ion cannon special effects.		
		if (diffuse.isTransparent())
		{
			materials_[i].absoluteAlphaPriority(true);
			materials_[i].alphaPriority(10000);
		}
	}
}

MachPhysIHealthMaterials::~MachPhysIHealthMaterials()
{
	_DELETE_ARRAY(materials_);
}

const RenMaterial& MachPhysIHealthMaterials::material
(
	double percentageHp
) const
{
	PRE(percentageHp >= 0);
	PRE(percentageHp <= 100);

	const int idx = _STATIC_CAST(int, nMats_ * percentageHp / 100.1);
	ASSERT(idx >= 0,     "MachPhysIHealthMaterials indexing error");
	ASSERT(idx < nMats_, "MachPhysIHealthMaterials indexing error");
	
	const RenMaterial& result = materials_[idx];
	POST_INFO(result.diffuse().a());
	POST_INFO(alpha());
	POST_INFO(result.texture());
	POST_INFO(texture());
	POST(result.texture() == texture());
	POST(fabs(result.diffuse().a() - alpha()) < 1.1/255.0);
	return result;
}

RenColour MachPhysIHealthMaterials::computeColour(double percentageHp) const
{
	PRE(percentageHp >= 0);
	PRE(percentageHp <= 100);

	static const RenColour markerGreen(0.0, 194.0/255.0, 0.0);
	static const RenColour markerYellow(238.0/255.0, 215.0/255.0, 6.0/255.0);
	static const RenColour markerRed(224.0/255.0, 0.0, 0.0);

	RenColour result;
	if ( percentageHp > 90 )
	{
		result = markerGreen;
	}	
	else if ( percentageHp > 50 )
	{
		result.linearInterpolate(percentageHp, markerGreen, 90.0, markerYellow, 50.0);
	}
	else if ( percentageHp > 10 )
	{
		result.linearInterpolate(percentageHp, markerYellow, 50.0, markerRed, 10.0);
	}
	else
	{
		result = markerRed;
	}
	
	return result;
}	

const RenMaterial& MachPhysIHealthMaterials::blueMaterial() const
{
	const RenMaterial& result = blue_;
	POST_INFO(result.diffuse().a());
	POST_INFO(alpha());
	POST_INFO(result.texture());
	POST_INFO(texture());
	POST(result.texture() == texture());
	POST(fabs(result.diffuse().a() - alpha()) < 1.1/255.0);
	return result;
}	

/*
void MachPhysIHealthMaterials::alpha(double a)
{
	PRE(a >= 0);
	PRE(a <= 1);
	
	if (a != alpha_)
	{
		alpha_ = a;
		
		RenColour dif = blue_.diffuse();
		dif.a(alpha_);
		blue_.diffuse(dif);
		
		for (int i=0; i!=nMats_; ++i)
		{
			RenColour dif = materials_[i].diffuse();
			dif.a(alpha_);
			materials_[i].diffuse(dif);
		}
	}
}

void MachPhysIHealthMaterials::texture(const RenTexture& t)
{
	if (t != texture_)
	{
		texture_ = t;
		
		blue_.texture(texture_);
		
		for (int i=0; i!=nMats_; ++i)
			materials_[i].texture(texture_);
		}
	}
}
*/
	
/* End HEALTHMA.CPP *************************************************/
