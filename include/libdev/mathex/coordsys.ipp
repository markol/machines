/*
 * C O O R D S Y S . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
void MexCoordSystem::set(Type t)
{
	PRE(!isSet()); 
	t_ = t;
	set_ = true;
	POST(isSet());
}

_CODE_INLINE
MexCoordSystem::operator const Type&() const
{
	return t_;
}

_CODE_INLINE
bool MexCoordSystem::isSet() const
{
	return set_;
}

_CODE_INLINE
ostream& operator <<(ostream& o, const MexCoordSystem& t)
{
	MexCoordSystem::Type x = t;
	switch (x)
	{
		case MexCoordSystem::LEFT_HANDED:	o << "left-handed co-ordinate system";  break;
		case MexCoordSystem::RIGHT_HANDED:	o << "right-handed co-ordinate system"; break;
	}
	return o;
}

/* End COORDSYS.IPP *************************************************/
