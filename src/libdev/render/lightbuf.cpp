/*
 * L I G H T B U F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/lightbuf.hpp"
#include "render/internal/vtxdata.hpp"
#include "render/internal/colpack.hpp"
#include "render/colour.hpp"

#ifndef _INLINE
    #include "render/internal/lightbuf.ipp"
#endif

#include <algorithm>
//#include <new.h>

///////////////////////////////////////////////////////////////////////////////
RenILightingBuffer::RenILightingBuffer(const char* type):
	typeDescription_(type),
	vertices_(NULL),
	fogs_(NULL),
	nValid_(0),
	capacity_(0)
{
	flags_.matAppliedFlags_   = NULL;
	flags_.perVertexMatFlags_ = NULL;
}

RenILightingBuffer::~RenILightingBuffer()
{
	_DELETE_ARRAY(vertices_);
	_DELETE_ARRAY(fogs_);
	_DELETE_ARRAY(flags_.matAppliedFlags_);
	_DELETE_ARRAY(flags_.perVertexMatFlags_);
}

void RenILightingBuffer::checkCapacity(int c)
{
	if (!vertices_ || capacity_ < c)
	if ( capacity_ < c)
	{
		_DELETE_ARRAY(vertices_);
		_DELETE_ARRAY(fogs_);
		_DELETE_ARRAY(flags_.matAppliedFlags_);
		_DELETE_ARRAY(flags_.perVertexMatFlags_);

		// Set a reasonable lower size to avoid resizing fequently on the
		// first render pass.
		const int minSize = 150;
		capacity_ = std::max(c, minSize);

		vertices_				  = _NEW_ARRAY(RenIVertex, capacity_);
		fogs_					  = _NEW_ARRAY(float, capacity_);
		flags_.matAppliedFlags_   = _NEW_ARRAY(bool,  capacity_);
		flags_.perVertexMatFlags_ = _NEW_ARRAY(bool,  capacity_);

		RENDER_STREAM("D3D vertex buffer [" << capacity_ << "] address = " << (void*) vertices_ << "\n");

		// Allow the subclass to resize any arrays it owns.
		doCheckCapacity();
	}

	POST(vertices());
	POST(fogAttenuations());
	POST(matAppliedFlags());
	POST(perVertexMatFlags());
	POST(capacity() >= c);
}

void RenILightingBuffer::setAllSpecular(const RenColour& s)
{
    const uint spec = packColourChecked(s.r(), s.g(), s.b());
	for (size_t i=0; i!=capacity_; ++i)
		vertices_[i].specular = spec;
}

void RenILightingBuffer::setAllAppliedFlags(bool value)
{
	memset(flags_.matAppliedFlags_, value, nValid_ * sizeof(bool));
}

void RenILightingBuffer::setAllPerVertexFlags(bool value)
{
	memset(flags_.perVertexMatFlags_, value, nValid_ * sizeof(bool));
}

void RenILightingBuffer::copyCoords(const RenIVertexData& in)
{
	nValid_ = in.size();

	// The layout of the lit and unlit vertices should be the same, so we
	// can simply memcpy from one to the other to move the vertex position
	// and UV data.
	memcpy(vertices_, &in.front(), sizeof(RenIVertex) * in.size());

	// Check that the specular components are all black.
	#ifndef NDEBUG
	RenIVertex* pVtx = vertices_;
	for (int i=in.size(); i!=0; --i)
	{
		ASSERT_DATA(static const uint black = packColourChecked(0,0,0));
		ASSERT(pVtx->specular == black, "Vertex buffer specular not black.");
		++pVtx;
	}
	#endif

	POST(nValid() == in.size());
}

// virtual
void RenILightingBuffer::printDebug(ostream& o) const
{
	o << "Internal " << typeDescription_<< " lighting buffer\n";
	o << "  capacity=" << capacity_ << ", no. valid=" << nValid_ << "\n";
	o << "  contents are:\n";

	for (size_t i=0; i!=nValid_; ++i)
	{
		o << "    ";
		printVertex(o, i);
		o << "\n";
	}

	o << "\n";
}

// virtual
void RenILightingBuffer::printVertex(ostream& o, const int index) const
{
	o << vertices_[index];
}

ostream& operator<<(ostream& o, const RenILightingBuffer& b)
{
	b.printDebug(o);
	return o;
}

///////////////////////////////////////////////////////////////////////////////
RenIFloatLightingBuffer::RenIFloatLightingBuffer():
	RenILightingBuffer("floating point"),
	rLamberts_(NULL),
	gLamberts_(NULL),
	bLamberts_(NULL),
	underflows_(NULL),
	maxLambertianR_(-10000),
	maxLambertianG_(-10000),
	maxLambertianB_(-10000)
{
}

RenIFloatLightingBuffer::~RenIFloatLightingBuffer()
{
	_DELETE_ARRAY(rLamberts_);
	_DELETE_ARRAY(gLamberts_);
	_DELETE_ARRAY(bLamberts_);
	_DELETE_ARRAY(underflows_);
}

void RenIFloatLightingBuffer::setAllUnderflowFlags(bool value)
{
	memset(underflows_, value, nValid() * sizeof(bool));
}

void RenIFloatLightingBuffer::doCheckCapacity()
{
	_DELETE_ARRAY(rLamberts_);
	_DELETE_ARRAY(gLamberts_);
	_DELETE_ARRAY(bLamberts_);
	_DELETE_ARRAY(underflows_);
	rLamberts_    = _NEW_ARRAY(float, capacity());
	gLamberts_    = _NEW_ARRAY(float, capacity());
	bLamberts_    = _NEW_ARRAY(float, capacity());
	underflows_   = _NEW_ARRAY(bool,  capacity());

	POST(rLamberts());
	POST(gLamberts());
	POST(bLamberts());
	POST(underflowFlags());
}

///////////////////////////////////////////////////////////////////////////////
RenIFixPtLightingBuffer::RenIFixPtLightingBuffer():
	RenILightingBuffer("fixed point (S11.4)")
	//lamberts_(NULL)
{
	// Under Watcom 10.6 check that the types are the same size.
	//ASSERT_COMPILE_TIME(sizeof(bool) == sizeof(MmxBool));
}

RenIFixPtLightingBuffer::~RenIFixPtLightingBuffer()
{
	//_DELETE_ARRAY(lamberts_);
}

// virtual
void RenIFixPtLightingBuffer::doCheckCapacity()
{
	//_DELETE_ARRAY(lamberts_);
	//lamberts_ = _NEW_ARRAY(MmxColourS11_4, capacity());

	//POST(lamberts());
}

/* End LIGHTBUF.CPP *************************************************/
