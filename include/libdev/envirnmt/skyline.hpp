/*
 * S K Y L I N E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_SKYLINE_HPP
#define _ENVIRNMT_SKYLINE_HPP

#include "world4d/matplan.hpp"
#include "ctl/countptr.hpp"

class W4dEntity;
class W4dGeneric;
class SysPathName;
class EnvElevationColourTable;
class EnvSatellite;
class RenColour;

class EnvSkyline
{
public:
    EnvSkyline(W4dEntity* parent, const SysPathName& meshFile);
    ~EnvSkyline();
	
	// Temporarily change the skyline to some fixed colour.
	void overrideColour(const RenColour&);

	// Put the colour back to normal.
	void resetColour();

	void colourTable(EnvElevationColourTable* clut,	const EnvSatellite* controller);

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvSkyline& t );

private:
	EnvElevationColourTable*	matTable_;
	W4dGeneric*					entity_;
	W4dMaterialPlanPtr			plan_;
	
    EnvSkyline( const EnvSkyline& );
    EnvSkyline& operator =( const EnvSkyline& );
    bool operator ==( const EnvSkyline& );
};

#endif

/* End SKYLINE.HPP *************************************************/
