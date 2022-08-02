/*
 * S A T P A R A M . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_SATPARAM_HPP
#define _ENVIRNMT_SATPARAM_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"

class W4dSceneManager;
class EnvOrbit;
class EnvSatellite;
class EnvElevationColourTable;

// See EnvIOribtParams.  The rationale for this class is the same.
class EnvISatelliteParams
{
public:
    EnvISatelliteParams(const string* name);	// PRE(name);
    ~EnvISatelliteParams();

	const string& name() const	{ return name_; }

	void orbit(const string* name);
	void dirLightClut(const string* name);
	void ambLightClut(const string* name);
	void mesh(const string* name);
	void meshColourClut(const string* name);

	EnvSatellite* createSatellite(W4dSceneManager*);

    void CLASS_INVARIANT;

private:
	const string				name_;
	string						meshName_;
	EnvOrbit*					orbit_;
	EnvElevationColourTable*	dirClut_;
	EnvElevationColourTable*	ambClut_;
	EnvElevationColourTable*	meshClut_;

    EnvISatelliteParams( const EnvISatelliteParams& );
    EnvISatelliteParams& operator =( const EnvISatelliteParams& );
    bool operator ==( const EnvISatelliteParams& );
};

#endif

/* End SATPARAM.HPP *************************************************/
