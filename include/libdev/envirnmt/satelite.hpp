/*
 * S A T E L I T E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_SATELITE_HPP
#define _ENVIRNMT_SATELITE_HPP

#include "base/base.hpp"
#include "mathex/angle.hpp"
#include "stdlib/string.hpp"

class RenColour;
class EnvOrbit;
class W4dEntity;
class W4dGeneric;
class W4dDirectionalLight;
class SysPathName;
class EnvElevationColourTable;

// Ties together all the physical and dynamic aspects of a satellite within
// a planet environment (it wouldn't apply to a space combat simulator).  The
// properties are: visual representation; orbit; dynamic colour change; and
// light source.  A satellite's position may be used to control other physical
// properties, such as sky colour.
class EnvSatellite
{
public:
	// TBD: add colour tables args for light colour and satellite colour.
	// There are two ctors: with and without a visual representation.
    EnvSatellite(const string& name, EnvOrbit*);
    ~EnvSatellite();

	void update();

	void visible( bool setVisible );
	const string& name() const;

	EnvOrbit& orbit();
	const EnvOrbit& orbit() const;

	// this->name() is used as the mesh name.
	void loadMesh(const SysPathName&, EnvElevationColourTable* colours=NULL);

	void setDirectionalLight(EnvElevationColourTable*);
	void setAmbientLight(EnvElevationColourTable*);

	// What is this satellite's contribution to ambient lighting (if any)?
	RenColour ambient() const;

	W4dDirectionalLight* light();
	const W4dDirectionalLight* light() const;

	MexRadians elevation() const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvSatellite& t );

private:
	EnvOrbit* const				orbit_;
	const string				name_;
	W4dDirectionalLight*		light_;
	EnvElevationColourTable*	lightTable_;
	EnvElevationColourTable*	ambTable_;
	EnvElevationColourTable*	matTable_;

    EnvSatellite( const EnvSatellite& );
    EnvSatellite& operator =( const EnvSatellite& );
    bool operator ==( const EnvSatellite& );
};

#endif

/* End SATELITE.HPP *************************************************/
