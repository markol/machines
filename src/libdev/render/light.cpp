/*
 * L I G H T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/light.hpp"
#include "render/internal/inlight.hpp"
#include "render/device.hpp"

PER_DEFINE_PERSISTENT( RenLight );
PER_DEFINE_PERSISTENT( RenDirectionalLight );
PER_DEFINE_PERSISTENT( RenAttenuatedLight );
PER_DEFINE_PERSISTENT( RenPointLight );
PER_DEFINE_PERSISTENT( RenUniformLight );

RenLight::RenLight(RenILight* l):
	pILight_(l)
{
	PRE(l);
}

// virtual
RenLight::~RenLight()
{
	_DELETE(pILight_);
}

void RenLight::colour(const RenColour& col)
{
  	pILight_->colour(col);
}

const RenColour& RenLight::colour() const
{
    return pILight_->colour();
}

// static
const RenLightColourTransform* RenLight::globalColourTransform()
{
	return RenILight::globalColourTransform();
}

void RenLight::globalColourTransform(const RenLightColourTransform* c)
{
	RenILight::globalColourTransform() = c;
}

void RenLight::turnOn()
{
	pILight_->on(true);
}

void RenLight::turnOff()
{
	pILight_->on(false);
}

bool RenLight::isOn() const
{
	return pILight_->on();
}

bool RenLight::wasUsedThisFrame() const
{
	return pILight_->used();
}

RenILight* RenLight::pILight()
{
	return pILight_;
}

void RenLight::print(ostream& o) const
{
	pILight_->print(o);
}

ostream& operator<<(ostream& o, const RenLight* l)
{
	if (!l)
		o << "Null light pointer";
	else
		l->print(o);

	return o;
}

ostream& operator<<(ostream& o, const RenLight& l)
{
	l.print(o);
	return o;
}

RenLight::RenLight( PerConstructor )
: pILight_( NULL )
{
}

void perWrite( PerOstream& ostr, const RenLight& light )
{
    ostr << light.pILight_;
}

void perRead( PerIstream& istr, RenLight& light )
{
    istr >> light.pILight_;
}

/////////////////////////////////////////////////////////////////////////////
RenDirectionalLight::RenDirectionalLight():
	RenLight(_NEW(RenIDirectionalLight)),
	pDir_(_STATIC_CAST(RenIDirectionalLight*, pILight()))
{
}

// virtual
RenDirectionalLight::~RenDirectionalLight()
{
}

void RenDirectionalLight::direction(const MexVec3& lightVec)
{
	PRE(lightVec.modulus() <= 1.0001);

	//PRE(!RenDevice::current()->rendering3D());
    pDir_->direction(lightVec);
}

const MexVec3& RenDirectionalLight::direction() const
{
    return pDir_->direction();
}

void perWrite( PerOstream& ostr, const RenDirectionalLight& light )
{
    const RenLight& base = light;

    ostr << base;
    ostr << light.pDir_;
}

void perRead( PerIstream& istr, RenDirectionalLight& light )
{
    RenLight& base = light;

    istr >> base;
    istr >> light.pDir_;
}

ostream& operator<<(ostream& o, const RenDirectionalLight&)
{
    o << "Directional light" << std::endl;

    return o;
}
/////////////////////////////////////////////////////////////////////////////
RenAttenuatedLight::RenAttenuatedLight(RenIAttenuatedLight* l):
	RenLight(l),
	atLight_(l)
{
	constantAttenuation(0);
	linearAttenuation(0.7);
	quadraticAttenuation(0.3);
}

const MexPoint3d& RenAttenuatedLight::position() const
{
    return atLight_->position();
}

MATHEX_SCALAR RenAttenuatedLight::maxRange() const
{
	return atLight_->maxRange();
}

MATHEX_SCALAR RenAttenuatedLight::constantAttenuation() const
{
	return atLight_->constantAttenuation();
}

MATHEX_SCALAR RenAttenuatedLight::linearAttenuation() const
{
	return atLight_->linearAttenuation();
}

MATHEX_SCALAR RenAttenuatedLight::quadraticAttenuation() const
{
	return atLight_->quadraticAttenuation();
}

void RenAttenuatedLight::position(const MexPoint3d& pos)
{
	//PRE(!RenDevice::current()->rendering3D());
    atLight_->position(pos);
}

void RenAttenuatedLight::maxRange(MATHEX_SCALAR  r)
{
	PRE(r > 0);
	PRE(!RenDevice::current()->rendering3D());
	atLight_->maxRange(r);
}

void RenAttenuatedLight::constantAttenuation(MATHEX_SCALAR r)
{
	PRE(!RenDevice::current()->rendering3D());
	PRE(r >= 0);
	PRE(r <= 1);
	atLight_->constantAttenuation(r);
}

void RenAttenuatedLight::linearAttenuation(MATHEX_SCALAR r)
{
	PRE(!RenDevice::current()->rendering3D());
	PRE(r >= 0);
	PRE(r <= 1);
	atLight_->linearAttenuation(r);
}

void RenAttenuatedLight::quadraticAttenuation(MATHEX_SCALAR r)
{
	PRE(!RenDevice::current()->rendering3D());
	PRE(r >= 0);
	PRE(r <= 1);
	atLight_->quadraticAttenuation(r);
}

// static
void RenAttenuatedLight::resultTollerance(MATHEX_SCALAR v)
{
	PRE(v >= 0);
	RenIAttenuatedLight::resultTollerance() = v;
}

// static
MATHEX_SCALAR RenAttenuatedLight::resultTollerance()
{
	return RenIAttenuatedLight::resultTollerance();
}

RenAttenuatedLight::RenAttenuatedLight( PerConstructor con )
: RenLight( con ),
  atLight_( NULL )
{
}

void perWrite( PerOstream& ostr, const RenAttenuatedLight& light )
{
    const RenLight& base = light;

    ostr << base;
	ostr << light.atLight_;
}

void perRead( PerIstream& istr, RenAttenuatedLight& light )
{
    RenLight& base = light;

    istr >> base;
	istr >> light.atLight_;
}

ostream& operator<<(ostream& o, const RenAttenuatedLight&)
{
    o << "Attenuated light" << std::endl;

    return o;
}

/////////////////////////////////////////////////////////////////////////////
RenPointLight::RenPointLight(MATHEX_SCALAR range):
	RenAttenuatedLight(_NEW(RenIPointLight(range)))
{
}

RenPointLight::RenPointLight( PerConstructor con )
: RenAttenuatedLight( con )
{
}

void perWrite( PerOstream& ostr, const RenPointLight& light )
{
    const RenAttenuatedLight& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, RenPointLight& light )
{
    RenAttenuatedLight& base = light;

    istr >> base;
}

/////////////////////////////////////////////////////////////////////////////
RenUniformLight::RenUniformLight(MATHEX_SCALAR range):
	RenAttenuatedLight(_NEW(RenIUniformLight(range)))
{
}

RenUniformLight::RenUniformLight( PerConstructor con )
: RenAttenuatedLight( con )
{
}

void perWrite( PerOstream& ostr, const RenUniformLight& light )
{
    const RenAttenuatedLight& base = light;

    ostr << base;
}

void perRead( PerIstream& istr, RenUniformLight& light )
{
    RenAttenuatedLight& base = light;

    istr >> base;
}

/* End LIGHT.CPP ****************************************************/
