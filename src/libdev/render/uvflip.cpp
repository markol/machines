/*
 * U V F L I P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/uvflip.hpp"

RenUVFlip::RenUVFlip():
	uFlipped_(false),
	vFlipped_(false)
{
}

// virtual 
RenUVFlip::~RenUVFlip()
{
}

// virtual 
void RenUVFlip::transform(float uIn, float vIn, float* uOut, float* vOut) const
{
	if (uFlipped_)
		*uOut = -uIn;
	else	
		*uOut = uIn;

	if (vFlipped_)
		*vOut = -vIn;
	else	
		*vOut = vIn;
}

// virtual 
void RenUVFlip::print(ostream& o) const
{
	switch (flipped())
	{
		case NONE:		o << "non-UV-flipped texture"; break;
		case U:			o << "texture flip in U only"; break;
		case V:			o << "texture flip in V only"; break;
		case BOTH:		o << "texture flip in both U&V"; break;
		default:		ASSERT_BAD_CASE;
	}
}

// virtual 
RenUVTransform* RenUVFlip::clone() const
{
	return _NEW(RenUVFlip(*this));
}

void RenUVFlip::flipped(RenUVFlip::Flip f)
{
	switch (f)
	{
		case NONE:		uFlipped_ = false; vFlipped_ = false; break;
		case U:			uFlipped_ = true;  vFlipped_ = false; break;
		case V:			uFlipped_ = false; vFlipped_ = true;  break;
		case BOTH:		uFlipped_ = true;  vFlipped_ = true;  break;
		default:		ASSERT_BAD_CASE;
	}
}

RenUVFlip::Flip RenUVFlip::flipped() const
{
	if (uFlipped_)
	{
		if (vFlipped_)
			return BOTH;
		else
			return U;	
	}
	else
	{
		if (vFlipped_)
			return V;
		else
			return NONE;	
	}
}

void RenUVFlip::invertU()
{
	uFlipped_ = !uFlipped_;
}

void RenUVFlip::invertV()
{
	vFlipped_ = !vFlipped_;
}

void RenUVFlip::invertBoth()
{
	uFlipped_ = !uFlipped_;
	vFlipped_ = !vFlipped_;
}

/* End UVFLIP.CPP ***************************************************/
