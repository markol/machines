/*
 * I N T M A P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/intmap.hpp"
#include "ctl/algorith.hpp"
#include "render/mesh.hpp"

RenIVertexIntensities::RenIVertexIntensities(size_t nElements):
	Base(0)
{
	reserve(nElements);
    TEST_INVARIANT;
}

RenIVertexIntensities::RenIVertexIntensities(const RenIVertexIntensities& copyMe):
	Base(copyMe)
{
}

RenIVertexIntensities::~RenIVertexIntensities()
{
    TEST_INVARIANT;
}

bool RenIVertexIntensities::contains(Ren::VertexIdx i, MATHEX_SCALAR* value) const
{
	RenIVtxIntensity target;
	target.index_ = i;
	const_iterator it = find(begin(), end(), target);
	const bool result = it != end();

	if (value)
	{
		if (!result)
			*value = 1;
		else
			*value = (*it).int_;
	}

	POST(implies(result == true  && value, *value!=1));
	POST(implies(result == false && value, *value==1));
	return result;
}

MATHEX_SCALAR RenIVertexIntensities::value(Ren::VertexIdx i) const
{
	MATHEX_SCALAR result;
	contains(i, &result);
	POST(implies(!contains(i), result==1));
	return result;
}

void RenIVertexIntensities::value(Ren::VertexIdx i, MATHEX_SCALAR newValue)
{
	RenIVtxIntensity target;
	target.index_ = i;

	// There appears to be a Watcom bug here.  The compiler fails to recognise
	// that begin and end can refer to overloaded functions which return a
	// iterator rather than a const_iterator.
	#ifdef NDEBUG
//	const_iterator cit = find(begin(), end(), target);
	iterator it = find(begin(), end(), target);
//	iterator it = _CONST_CAST(iterator, cit);
	#else
	iterator it = find(begin(), end(), target);
	#endif

	if (newValue == 1)
	{
		if (it != end())
			erase(it);
	}
	else
	{
		if (it != end())
			(*it).int_ = newValue;
		else
		{
			RenIVtxIntensity newElement;
			newElement.index_ = i;
			newElement.int_   = newValue;
			push_back(newElement);
		}
	}

	POST(implies(newValue==1, !contains(i)));
	POST(implies(newValue!=1,  contains(i)));
}

void RenIVertexIntensities::CLASS_INVARIANT
{
	// There are no intensities of 1.0 in the map.
	const_iterator it = begin();
	while (it != end())
	{
		INVARIANT((*it).int_ != 1);
		++it;
	}
}

RenIExpandedIntensityMap::RenIExpandedIntensityMap(size_t nElements):
	Base(nElements, 1)
{
	POST(size() >= nElements);
}

void RenIExpandedIntensityMap::checkSize(size_t nElements)
{
	if (size() < nElements)
	{
		for (size_t i = size(); i!=nElements; ++i)
			push_back(1);
	}

	POST(size() >= nElements);
}

void RenIExpandedIntensityMap::expand(const RenIVertexIntensities* map)
{
	// Reset every element to 1.
	Base::iterator it2 = begin();
	while (it2 != end())
	{
		*it2 = 1;
		++it2;
	}

	if (map)
	{
		// Iterate through the map and set all the non-unity elements of
		// the expansion.
		RenIVertexIntensities::Base::const_iterator it = map->begin();
		while (it != map->end())
		{
			(*this)[(*it).index()] = (*it).intensity();
			++it;
		}
	}
}

/* End INTMAP.CPP ***************************************************/
