/*
 * S K Y . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_SKY_HPP
#define _ENVIRNMT_SKY_HPP

/*
  Classes for representing skies in serveral ways. The classes are ordered
  as follows:

  EnvSky
    EnvControlledSky
	  EnvUniformSky
	  EnvDynamicSky
	EnvStaticSky

  // EnvSky //
  Abstract base class that provides virtual methods such as visible() and update().
  Allows polymorphic pointers to be stored in EnvPlanetEnvironment. Also has
  facilities for using W4dStars.

  // EnvControlledSky //
  Abstract base class Derived from EnvSky and adding functionality for setting and
  getting a controlling EnvSatellite.

  // EnvUniformSky //
  Concrete class derived from EnvControlledSky. Simply changes colour according to a
  colour table with colours mapped against the elevation angle of the controlling EnvSatellite.

  // EnvDynamicSky //
  Concrete class derived from EnvControlledSky. Uses a mesh to represent the sky.
  The background colour and the colour of the mesh are changed according to the
  elevation of the controlling EnvSatellite.

  // EnvStaticSky //
  Concrete class derived from EnvSky. Represented as a mesh but has no controller.
*/

#include "base/base.hpp"

#include "mathex/angle.hpp"

#include "ctl/map.hpp"

#include "render/render.hpp"
#include "render/colour.hpp"

class W4dEntity;
class W4dCustom;
class W4dSceneManager;
class W4dStars;

class SysPathName;

class EnvElevationColourTable;
class EnvSatellite;

class RenVertex;

////////////////////////////////////////////////////////////

class EnvSky
// Cannonical form revoked.
{
public:
    virtual ~EnvSky();

	virtual void update(W4dSceneManager*) = 0;
	virtual void write(ostream&) const = 0;
    virtual bool visible() const = 0;
    virtual void visible(bool yesNo) = 0;

	// Temporarily change the sky to some fixed colour.
	// POST(colourChanged()); POST(doColourOverride());
	void overrideColour(const RenColour&);

	// Put it back to whatever normal colour the sub-class implements.
	// POST(colourChanged()); POST(!doColourOverride());
	void resetColour();

	// Sub-classes should examine these values to check for colour overrides.
	const RenColour& overrideColour() const;
	bool doColourOverride() const;

	void attachStars(W4dStars* const pStars);
	// PRE(pStars);
	// POST(pStars_);
	void detachStars();
	// PRE(pStars_);
	// POST(not pStars_);

	// Returns 0 if the no stars have been attached or they have been detached.
    const W4dStars*	pStars() const;
    W4dStars*	pStars();

    void CLASS_INVARIANT;

protected:
	EnvSky();

	// Using vertex colouring, apply the given colour to the entire mesh.
	// Supplied as a service for sub-classes.
	void uniformlyColourMesh(const RenColour&, RenMesh*);

	// If the colour has just changed, then in some cases we may need to update
	// the entire sky in one frame.
	bool colourChanged() const;
	void colourChangeUpdated();		// POST(!colourChanged());

private:
	W4dStars*	pStars_;
	RenColour	overrideColour_;
	bool		doColourOverride_;
	bool		colourChanged_;
};

ostream& operator <<(ostream& o, const EnvSky& sky);

////////////////////////////////////////////////////////////

class EnvControlledSky : public EnvSky
// Cannonical form revoked.
{
public:
	virtual ~EnvControlledSky();

    void controller(const EnvSatellite* c);
    // PRE(c);
	const EnvSatellite* controller() const;

	void CLASS_INVARIANT;

protected:
	EnvControlledSky();

private:
	const EnvSatellite* pController_;
};

////////////////////////////////////////////////////////////

class EnvUniformSky : public EnvControlledSky
{
public:
    EnvUniformSky();
    virtual ~EnvUniformSky();

	virtual void update(W4dSceneManager* pSceneManager);
	// PRE(pSceneManager);
	virtual void write(ostream&) const;
	// PRE(colourTable());
    virtual bool visible() const;
    virtual void visible(bool yesNo);

	void colourTable(EnvElevationColourTable* pColourTable);
	// PRE(pColourTable);

    void CLASS_INVARIANT;

private:
	// Operations deliberately revoked.
    EnvUniformSky( const EnvUniformSky& );
    EnvUniformSky& operator =( const EnvUniformSky& );
    bool operator ==( const EnvUniformSky& );

	EnvElevationColourTable*	pColourTable_;
	bool						visible_;
};

class EnvDynamicSky : public EnvControlledSky
{
public:
    EnvDynamicSky(W4dEntity* pParent);
    virtual ~EnvDynamicSky();

	virtual void update(W4dSceneManager* pSceneManager);
	// PRE(pSceneManager);
	virtual void write(ostream&) const;
    virtual bool visible() const;
    virtual void visible(bool);

	// Specify a colour table for vertices of a given elevation.  Vertices with
	// intermediate elevations are assigned interpolated colours.
	void colourTable(MexDegrees elevation, EnvElevationColourTable* pElevationClut);
	// PRE(pElevationClut);

	// Specify a file containing a sky-dome mesh.  The mesh must be called "sky".
	void mesh(const SysPathName& meshFile);
	// PRE(meshFile.existsAsFile());

    void CLASS_INVARIANT;

private:
	// Operations deliberately revoked.
    EnvDynamicSky(const EnvDynamicSky&);
    EnvDynamicSky& operator =(const EnvDynamicSky&);
    bool operator ==(const EnvDynamicSky&);

	// If all cluts have one entry they are simple, otherwise they are complex.
	enum ClutComplexity
	{
		UNTESTED,
		SIMPLE,
		COMPLEX
	};

	ClutComplexity testComplexity() const;

	// In the case that the sky is simple. This is called once to set the
	// colours for the sky up properly.
	void updateAllVertices();

	const RenColour& findVertexColour(MexDegrees) const;
	void colourSomeVertices(RenMesh* mesh, int startIndex, int nVertices);

	typedef ctl_map<MexDegrees, EnvElevationColourTable*, std::less<MexDegrees> > ClutMap;

	W4dCustom*		pDome_;
	ClutMap	  		cluts_;
	Ren::VertexIdx	index_;
	RenColour		topColour_;
	MexDegrees		maxElevation_;
	ClutComplexity	complexity_;
};

////////////////////////////////////////////////////////////

class EnvStaticSky : public EnvSky
{
public:
	EnvStaticSky(W4dEntity* pParent);
	~EnvStaticSky();

	virtual void update(W4dSceneManager*);
	// PRE(pSceneManager);
	virtual void write(ostream&) const;
    virtual bool visible() const;
    virtual void visible(bool yesNo);

	void mesh(const SysPathName& meshFile);

	void backgroundColour(RenColour newBackgroundColour);
	const RenColour& backgroundColour() const;

	void CLASS_INVARIANT;

private:
	// Operations deliberately revoked.
	EnvStaticSky(const EnvStaticSky&);
	EnvStaticSky& operator =(const EnvStaticSky& assignMe);

	W4dCustom*	pSkyMesh_;
	RenColour	backgroundColour_;
};

////////////////////////////////////////////////////////////

#ifdef _INLINE
	#include "envirnmt/sky.ipp"
#endif

#endif

/* End SKY.HPP **************************************************/
