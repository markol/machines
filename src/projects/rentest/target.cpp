/*
 * T A R G E T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "rentest/target.hpp"
#include "phys/mcfly.hpp"
#include "render/mesh.hpp"
#include "render/light.hpp"
#include "render/uvanim.hpp"
#include "render/colour.hpp"
#include "mathex/eulerang.hpp"

class TestUVAnim : public RenUVTransform
{
public:
	TestUVAnim(size_t n, float m, const DevTimer& t): nVerts_(n), counter_(0), magnitude_(m), timer_(t) {}

	virtual void transform(float uIn, float vIn, float* uOut, float* VOut) const;
	virtual void print(ostream&) const;
	virtual RenUVTransform* clone() const;

private:
	size_t				nVerts_, counter_;
	const DevTimer&		timer_;
	const double		magnitude_;
};

inline float rand0To1()
{
	return (rand() & 255) / 255.0;
}

// virtual
void TestUVAnim::transform(float uIn, float vIn, float* uOut, float* vOut) const
{
	static float randomOffsets[1000], randomRates[1000];
	static bool first = true;

	if (first)
	{
		first = false;

		for (int i=0; i!=1000; ++i)
		{
			randomOffsets[i] = 2 * rand0To1();
			randomRates  [i] = 1 + rand0To1();
		}
	}

	const double time = timer_.time();
	*uOut = uIn + magnitude_ * sin(randomRates[2*counter_]   * time);
	*vOut = vIn + magnitude_ * cos(randomRates[2*counter_+1] * time);

	TestUVAnim* crufty = _CONST_CAST(TestUVAnim*, this);
	crufty->counter_++;
	if (counter_ >= nVerts_)
		crufty->counter_ = 0;
}

// virtual
RenUVTransform* TestUVAnim::clone() const
{
	return _NEW(TestUVAnim(*this));
}

// virtual
void TestUVAnim::print(ostream&) const
{
}

void overrideDefaultKeyboardMapping( PhysFlyControl* );

ControlTarget::ControlTarget(const MexTransform3d& tx):
	control_(_NEW( PhysFlyControl( _NEW( PhysMotionControlledTransform( &xform_ ) ) ) ) )
{
	control_->degreesPerSecond(8);
	control_->metresPerSecond(0.6);
	control_->setDefaultPosition(tx);
	control_->resetPosition();
	overrideDefaultKeyboardMapping( control_ );
}

ControlTarget::~ControlTarget()
{
	_DELETE(control_);
}

ControlledModel::ControlledModel(const Ren::MeshPtr& m, const MexTransform3d& tx):
	ControlTarget(tx),
	mesh_(m),
	draw_(true),
	randomiseIntensities_(false),
	randomiseColours_(false),
	uvAnim_(NULL)
{
}

// virtual
ControlledModel::~ControlledModel()
{
	_DELETE(uvAnim_);
}

void ControlledModel::toggleWarpAnim(const DevTimer& timer)
{
	if (!uvAnim_)
	{
		uvAnim_ = _NEW(TestUVAnim(mesh_->nVertices(), 0.1, timer));
	}
	else
	{
		_DELETE(uvAnim_);
		uvAnim_ = NULL;
	}
}

// virtual
string ControlledModel::description()
{
	std::ostringstream ostr;
	ostr << "model " << mesh_->meshName() << ((draw_)? "(on) ": "(off) ");
	ostr << " at " << xform_.position() << std::ends;
	return ostr.str();
}

ControlledLight::ControlledLight(RenLight* l, const MexTransform3d& tx):
	ControlTarget(tx),
	light_(l)
{
}

// virtual
ControlledLight::~ControlledLight()
{
	_DELETE(light_);
}

// Create a transform which represents the direction of a given light.
static MexTransform3d lightTransform(const RenDirectionalLight* l)
{
	MexVec3 xBasis = l->direction();
	xBasis.makeUnitVector();

	// Use the light's direction as the transform's x-basis.  The other
	// basis vectors can be arbitrary, so we make some up.
	MexVec3 yBasis(0,0,1);
	yBasis.crossProduct(xBasis);

	// We can't use zBasis if it is parallel to the xBasis (zero cross product).
	if (fabs(yBasis.modulus()) < MexEpsilon::instance())
	{
		// Try again with the y-axis instead of the z-axis.
		yBasis.setVector(0, 1, 0);
		yBasis.crossProduct(xBasis);

		// The direction can't be parallel to both the z- and the y-axes.
		ASSERT(fabs(yBasis.modulus()) >= MexEpsilon::instance(), "Parallel basis vector mix up.");
	}

	MexVec3 zBasis = xBasis;
	zBasis.crossProduct(yBasis);

	// This funciton isn't efficiency critical, so do the normalisation
	// just to make sure.
	xBasis.makeUnitVector();
	yBasis.makeUnitVector();
	zBasis.makeUnitVector();

	MexTransform3d result(xBasis, yBasis, zBasis, MexPoint3d(0,0,0));
	return result;
}

ControlledDirLight::ControlledDirLight(RenDirectionalLight* l):
	ControlledLight(l, lightTransform(l)),
	dirLight_(l)
{
}

// virtual
void ControlledDirLight::update()
{
	dirLight_->direction(xform_.xBasis());
}

// virtual
string ControlledDirLight::description()
{
	const bool on = light_->isOn();
	MexEulerAngles ang = xform_.rotationAsEulerAngles();
	MexDegrees da = ang.azimuth();
	MexDegrees de = ang.elevation();

	if (da < MexDegrees(0))
		da += MexDegrees(360);

	std::ostringstream ostr;
	ostr.precision(1);
	ostr.setf(std::ios::fixed, std::ios::floatfield);
	ostr << "dir light " << ((on)? "(on) ": "(off) ");
	ostr << light_->colour() << ", az=" << da << ", el=" << de << std::ends;
	return ostr.str();
}

ControlledPointLight::ControlledPointLight(RenPointLight* l):
	ControlledLight(l, MexTransform3d(l->position())),
	ptLight_(l)
{
}

// virtual
void ControlledPointLight::update()
{
	ptLight_->position(xform_.position());
}

// virtual
string ControlledPointLight::description()
{
	const bool on = light_->isOn();

	std::ostringstream ostr;
	ostr.precision(1);
	ostr.setf(std::ios::fixed, std::ios::floatfield);
	ostr << "point light " << ((on)? "(on) ": "(off) ");
	ostr << light_->colour() << ", at " << ptLight_->position() << std::ends;
	return ostr.str();
}

ControlledUniformLight::ControlledUniformLight(RenUniformLight* l):
	ControlledLight(l, MexTransform3d(l->position())),
	ptLight_(l)
{
}

// virtual
void ControlledUniformLight::update()
{
	ptLight_->position(xform_.position());
}

// virtual
string ControlledUniformLight::description()
{
	const bool on = light_->isOn();

	std::ostringstream ostr;
	ostr.precision(1);
	ostr.setf(std::ios::fixed, std::ios::floatfield);
	ostr << "uniform light " << ((on)? "(on) ": "(off) ");
	ostr << light_->colour() << ", at " << ptLight_->position() << std::ends;
	return ostr.str();
}

/* End TARGET.CPP ***************************************************/
