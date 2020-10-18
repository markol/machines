/*
 * O U T C O D E . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#ifndef _MATHEX_OUTCODE_HPP
#define _MATHEX_OUTCODE_HPP

#include "base/base.hpp"

class MexAlignedBox2d;
class MexVec2;

// Cohen-Sutherland outcodes.
// Let the compiler generate the canonical form.
class OutCode
{
public:
	OutCode(): code_(inside)	{}
	OutCode(const MexAlignedBox2d& box, const MexVec2& pt);

	bool is_left   () const		{ return code_ &  left;   }
	bool is_right  () const		{ return code_ &  right;  }
	bool is_below  () const		{ return code_ &  below;  }
	bool is_above  () const		{ return code_ &  above;  }
	bool is_inside () const		{ return code_ == inside; }
	bool is_outside() const		{ return code_ != inside; }
	
	bool same_side_as(const OutCode& oc) const  { return oc.code_ & code_; }
	bool is_directly_opposite(const OutCode& oc) const;
	
private:	 
	enum { left = 8, right = 4, below = 2, above = 1, inside = 0 };
	unsigned int code_;
};

#endif
