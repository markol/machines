/*
 * L I G H T B U F . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_LITVERTX_HPP
#define _RENDER_LITVERTX_HPP

#include "base/base.hpp"
//#include "mmxlight/lightout.h"
#include <glm/gtc/matrix_transform.hpp>

class RenIVertexData;
class RenColour;
class RenIVertex;
//class MmxColourS11_4;

struct RenILightingFlags
{
	// Arrays of RGB values and flags.
	bool*		matAppliedFlags_;
	bool*		perVertexMatFlags_;
};

// Every time we light the vertices of a mesh, we need storage for the
// resulting lit vertices.  We could call new every time to allocate
// storage.  However, RenMesh makes available the maximum number of
// vertices in all the meshes ever created.  So we allocate one buffer
// of this size and overwrite it for each mesh we render.
class RenILightingBuffer
{
public:
    virtual ~RenILightingBuffer();

	// POST(vertices());
	// POST(capacity() >= c);
	void checkCapacity(int c);
	int capacity() const				{ return capacity_; }
	int nValid() const					{ return nValid_;   }

	// Used in numerous places for checking pre-conditions.
	bool sizeMatches(int s) const		{ return nValid_ == s; }

	// The specular part of a lit vertex is typically not used.  Thus, we can
	// initialise all specular values to black and leave them alone thereafter.
	void setAllSpecular(const RenColour& s);
	void setAllAppliedFlags(bool);
	void setAllPerVertexFlags(bool);

	RenIVertex*		vertices()			{ return vertices_; }
	float*	   		fogAttenuations()	{ return fogs_; }
	bool*	   		matAppliedFlags()	{ return flags_.matAppliedFlags_; }
	bool*	   		perVertexMatFlags()	{ return flags_.perVertexMatFlags_; }

	// POST(nValid() == in.size());
	void copyCoords(const RenIVertexData& in);

	virtual void printDebug(ostream&) const;
    void CLASS_INVARIANT;

protected:
    RenILightingBuffer(const char* type);

private:
	// Template methods filled by derived classes.
	virtual void doCheckCapacity() = 0;
	virtual void printVertex(ostream&, const int index) const;

	RenILightingFlags	flags_;
	RenIVertex*			vertices_;
	float*				fogs_;
	int		   			capacity_, nValid_;
	const char* const	typeDescription_;

    // Operations deliberately revoked.
    RenILightingBuffer( const RenILightingBuffer& );
    RenILightingBuffer& operator =( const RenILightingBuffer& );
    bool operator ==( const RenILightingBuffer& );
};

ostream& operator<<(ostream&, const RenILightingBuffer&);

class RenIFloatLightingBuffer : public RenILightingBuffer
{
public:
	RenIFloatLightingBuffer();
	virtual ~RenIFloatLightingBuffer();

	float* 	rLamberts()			{ return rLamberts_; }
	float*	gLamberts()			{ return gLamberts_; }
	float*	bLamberts()			{ return bLamberts_; }
	bool*	underflowFlags()	{ return underflows_; }

	float maxLambertianR() const;
	float maxLambertianG() const;
	float maxLambertianB() const;

	void checkMaxLambertian(float r, float g, float b);
	void resetMaxLambertian();

	void setAllUnderflowFlags(bool);

private:
	virtual void doCheckCapacity();

	float*		rLamberts_;
	float*		gLamberts_;
	float*		bLamberts_;
	bool*		underflows_;
	float		maxLambertianR_, maxLambertianG_, maxLambertianB_;
};

class RenIFixPtLightingBuffer : public RenILightingBuffer
{
public:
	RenIFixPtLightingBuffer();
	virtual ~RenIFixPtLightingBuffer();

	//MmxColourS11_4*	lamberts()			{ return lamberts_; }

private:
	virtual void doCheckCapacity();

	//MmxColourS11_4*		lamberts_;
};

#ifdef _INLINE
    #include "internal/lightbuf.ipp"
#endif

#endif

/* End LIGHTBUF.HPP *************************************************/
