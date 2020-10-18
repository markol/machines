/*
 * C O L O U R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <math.h>
#include "render/colour.hpp"

#ifndef _INLINE
    #include "render/colour.ipp"
#endif

PER_DEFINE_PERSISTENT( RenColour );

// Lights can have colours whose components are outside the range [0,1].
// Should RenColour allow this, or should lights be a special case?
RenColour::RenColour(float grey)
: rep_( grey, grey, grey, 1 )
{
	// Removed for negative light support.
	// TBD: ideally the lights should use a different unbounded colour.
	// TBD: remove the pre-conditions in the ipp file to match.
	// PRE(lessThanOrEq(0, grey));
    TEST_INVARIANT;
}

RenColour::RenColour(float r, float g, float b, float a)
: rep_( r, g, b, a )
{
    PRE_INFO(r);
    PRE_INFO(g);
    PRE_INFO(b);
    PRE_INFO(a);
	PRE(lessThanOrEq(0, a) && lessThanOrEq(a, 1));

	// Removed for negative light support.  TBD: see other ctor.
	// PRE(lessThanOrEq(0, r));
	// PRE(lessThanOrEq(0, g));
	// PRE(lessThanOrEq(0, b));
    TEST_INVARIANT;
}

RenColour& RenColour::linearInterpolate
(
	float i,
	const RenColour& c1, float i1,
	const RenColour& c2, float i2
)
{
    TEST_INVARIANT;
	*this =  c2;
	*this -= c1;
	*this *= ((i - i1) / (i2 - i1));
	*this += c1;

    TEST_INVARIANT;
	return *this;
}

void RenColour::CLASS_INVARIANT
{
	// The alpha value is packed into the MS byte of a ulong.  The other three
	// bytes should be zero.
	INVARIANT( (rep_.a_ & 0xffffff ) == 0);
}

ostream& operator <<( ostream& o, const RenColour& t )
{
	// Don't print alpha if it's set to the default, opaque value.
    const std::ios::fmtflags oldFlags = o.flags();
	const int oldPrecision       = o.precision();

	o.precision(2);
	o.setf(std::ios::fixed, std::ios::floatfield);

	if (t.rep_.a_ < 255)
    	o << "RenColour: (" << t.rep_.r_ << "," << t.rep_.g_ << "," << t.rep_.b_ << ", a=" << t.a() << ")";
	else
    	o << "RenColour: (" << t.rep_.r_ << "," << t.rep_.g_ << "," << t.rep_.b_ << ")";

	o.precision(oldPrecision);
	o.setf(oldFlags);

    return o;
}

#define EXPECT(expected)	i>>c; if (c!=expected) return i;

istream& operator >>( istream& i, RenColour& t )
{
	char c;

	EXPECT('(');
	i >> t.rep_.r_;
	EXPECT(',');
	i >> t.rep_.g_;
	EXPECT(',');
	i >> t.rep_.b_;
	EXPECT(')');

	// TBD: add logic to deal with alpha in the input.
	return i;
}

bool RenColour::operator==(const RenColour& c) const
{
	// A tollerance of 1/255 should be satisfactory for all applications.
	const float epsilon = 0.001;
	return	fabs(rep_.r_ - c.rep_.r_) < epsilon &&
			fabs(rep_.g_ - c.rep_.g_) < epsilon &&
			fabs(rep_.b_ - c.rep_.b_) < epsilon &&
			rep_.a_ == c.rep_.a_;
}

bool RenColour::operator!=(const RenColour& c) const
{
    return !(*this == c);
}

// static
const RenColour& RenColour::black()
{
	static const RenColour c(0);
	return c;
}

// static
const RenColour& RenColour::white()
{
	static const RenColour c(1);
	return c;
}

// static
const RenColour& RenColour::red()
{
	static const RenColour c(1,0,0);
	return c;
}

// static
const RenColour& RenColour::green()
{
	static const RenColour c(0,1,0);
	return c;
}

// static
const RenColour& RenColour::blue()
{
	static const RenColour c(0,0,1);
	return c;
}

// static
const RenColour& RenColour::yellow()
{
	static const RenColour c(1,1,0);
	return c;
}

// static
const RenColour& RenColour::cyan()
{
	static const RenColour c(0,1,1);
	return c;
}

// static
const RenColour& RenColour::magenta()
{
	static const RenColour c(1,0,1);
	return c;
}

void perWrite( PerOstream& ostr, const RenColour& colour )
{
    PER_WRITE_RAW_DATA( ostr, &colour.rep_, sizeof( RenColour::IColour ) );
}

void perRead( PerIstream& istr, RenColour& colour )
{
    PER_READ_RAW_DATA( istr, &colour.rep_, sizeof( RenColour::IColour ) );
}

// virtual
RenColourTransform::~RenColourTransform()
{
}

/* End COLOUR.CPP ***************************************************/
