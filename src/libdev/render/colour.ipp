/*
 * C O L O U R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool RenColour::lessThanOrEq(float d1, float d2) const
{
	// A tollerance of 1/255 should be satisfactory for all applications.
	const float epsilon = 0.001;
	return d2 - d1 > -epsilon;
}

_CODE_INLINE
void RenColour::IColour::packAlpha(float a)
{	
	if (a >= 1.0)
		a_ = 0xff000000L;
	else if (a <= 0.0)	
		a_ = 0;
	else	
		a_ = _STATIC_CAST(ulong, 255 * a) << 24;
}


_CODE_INLINE
float RenColour::IColour::unpackAlpha() const
{	
	return _STATIC_CAST(float, (a_>>24)) * (1.0/255);
}

_CODE_INLINE
RenColour::IColour::IColour(float r, float g, float b, float a):
	r_( r ), g_( g ), b_( b )
{
	packAlpha(a);
}	

_CODE_INLINE
void RenColour::r(float r)
{	
	rep_.r_ = r;
}

_CODE_INLINE
void RenColour::g(float g)
{	
	rep_.g_ = g;
}

_CODE_INLINE
void RenColour::b(float b)
{	
	rep_.b_ = b;
}

_CODE_INLINE
void RenColour::a(float a)
{	
	PRE(lessThanOrEq(0, a) && lessThanOrEq(a, 1));
	rep_.packAlpha(a);
}

_CODE_INLINE
float RenColour::r() const
{	
	return rep_.r_;
}

_CODE_INLINE
float RenColour::g() const
{	
	return rep_.g_;
}

_CODE_INLINE
float RenColour::b() const
{	
	return rep_.b_;
}

_CODE_INLINE
float RenColour::a() const
{	
	return rep_.unpackAlpha();
}

_CODE_INLINE
ulong RenColour::packedAlpha() const
{
	return rep_.a_;
}	

_CODE_INLINE
bool RenColour::isTransparent() const
{
	return rep_.a_ != 0xff000000L;
}	

_CODE_INLINE
RenColour& RenColour::operator+=(const RenColour& c)
{
	rep_.r_ += c.rep_.r_;
	rep_.g_ += c.rep_.g_;
	rep_.b_ += c.rep_.b_;
	rep_.a_ += c.rep_.a_;
	return *this;
}

_CODE_INLINE
RenColour& RenColour::operator-=(const RenColour& c)
{
	rep_.r_ -= c.rep_.r_;
	rep_.g_ -= c.rep_.g_;
	rep_.b_ -= c.rep_.b_;
	rep_.a_ -= c.rep_.a_;
	return *this;
}

_CODE_INLINE
RenColour& RenColour::operator*=(const RenColour& col)
{
	rep_.r_ *= col.rep_.r_;
	rep_.g_ *= col.rep_.g_;
	rep_.b_ *= col.rep_.b_;
	a(a() * col.a());
	return *this;
}

_CODE_INLINE
RenColour& RenColour::operator*=(float s)
{
	rep_.r_ *= s;
	rep_.g_ *= s;
	rep_.b_ *= s;
	a(a() * s);
	return *this;
}

_CODE_INLINE
RenColour& RenColour::operator/=(float s)
{
	PRE(s != 0);
	rep_.r_ /= s;
	rep_.g_ /= s;
	rep_.b_ /= s;
	a(a() / s);
	return *this;
}

/* End COLOUR.IPP ***************************************************/
