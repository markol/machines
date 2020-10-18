/*
 * P O L Y S O R D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/internal/polysord.hpp"
#include "ctl/algorith.hpp"
#include "render/device.hpp"
#include "render/texture.hpp"
#include "render/material.hpp"

//#define POST_SORTER_STREAM(x)	IAIN_STREAM(x)
#define POST_SORTER_STREAM(x)	;

RenIDepthSortedItem::RenIDepthSortedItem( const RenMaterial& m)
: material_(m),
  depth_(0),
  meshId_(0)
{
}

bool RenIDepthSortedItem::operator<(const RenIDepthSortedItem& item) const
{
	bool result;
	if (meshId() != 0 && meshId() == item.meshId())
	{
		// If these don't have the same depth, then an invalid ordering can
		// occur, i.e. one that's not strict weak.
		ASSERT_INFO(depth());
		ASSERT_INFO(item.depth());
		ASSERT(depth() == item.depth(), "Depth sort items from the same mesh don't have matching depths");
		result = material_.alphaPriority() < item.material_.alphaPriority();
	}
	else if (material_.absoluteAlphaPriority() || item.material_.absoluteAlphaPriority())
	{
		result = material_.alphaPriority() < item.material_.alphaPriority();
	}
	else
	{
		result = depth() > item.depth();
	}
	return result;
}

MATHEX_SCALAR RenIDepthSortedItem::depth() const
{
	return depth_;
}

ostream& operator<<(ostream& o, const RenIDepthSortedItem& i)
{
	i.print(o);
	return o;
}

class CompItems
{
public:
	bool operator()(const RenIDepthSortedItem* i1, const RenIDepthSortedItem* i2)
	{
		PRE(i1 && i2);
		return *i1 < *i2;
	}
};

RenIDepthPostSorter::RenIDepthPostSorter()
{
	items_.reserve(200);
    TEST_INVARIANT;
}

RenIDepthPostSorter::~RenIDepthPostSorter()
{
    TEST_INVARIANT;
}

void RenIDepthPostSorter::addItem(std::auto_ptr<RenIDepthSortedItem>& item)
{
	// release implies that ownership is transferred from the caller.
	items_.push_back(item.release());
}

#ifndef NDEBUG
static void checkTransitivity
(
	const RenIDepthSortedItem* i1,
	const RenIDepthSortedItem* i2,
	const RenIDepthSortedItem* i3
)
{
	PRE(i1);
	PRE(i2);
	PRE(i3);
	const RenIDepthSortedItem& r1 = *i1;
	const RenIDepthSortedItem& r2 = *i2;
	const RenIDepthSortedItem& r3 = *i3;

	ASSERT_INFO(r1);
	ASSERT_INFO(r2);
	ASSERT_INFO(r3);
	ASSERT(implies(r1<r2 && r2<r3, r1<r3), "Alpha sorting transitivity error.");
	ASSERT(implies(r1<r3 && r3<r2, r1<r2), "Alpha sorting transitivity error.");
	ASSERT(implies(r2<r3 && r3<r1, r2<r1), "Alpha sorting transitivity error.");
	ASSERT(implies(r3<r1 && r1<r2, r3<r2), "Alpha sorting transitivity error.");
	ASSERT(implies(r2<r1 && r1<r3, r2<r3), "Alpha sorting transitivity error.");
	ASSERT(implies(r3<r2 && r2<r1, r3<r1), "Alpha sorting transitivity error.");
}

static void checkTransitivity(const ctl_vector<RenIDepthSortedItem*>& vec)
{
	// We need three or more elements to define a test.
	if (vec.size() <= 2)
		return;

	typedef ctl_vector<RenIDepthSortedItem*>::const_iterator It;

	const It stopAt = vec.begin() + vec.size() - 3;
	for (It i=vec.begin(); i!=stopAt; ++i)
	{
		checkTransitivity(*i, *(i+1), *(i+2));
	}
}
#endif

ostream& operator<<(ostream& o, const ctl_vector<RenIDepthSortedItem*>& vec)
{
	typedef ctl_vector<RenIDepthSortedItem*>::const_iterator It;

	o << "Alpha depth post sorter contains " << vec.size() << " items:\n";

	It it = vec.begin();
	while (it != vec.end())
	{
		RenIDepthSortedItem* item = *it;

		o << "  " << *item << "\n";
		++it;
	}

	return o;
}

/*void RenIDepthPostSorter::render(IDirect3DDevice2* d3dDev)
{
	#ifndef NDEBUG
	POST_SORTER_STREAM("Before sort " << items_ << "\n");
	checkTransitivity(items_);
	#endif

	// The list of polygons must be sorted by depth.  Timing shows that
	// sorting is insignificant when compared with the rendering.
	sort(items_.begin(), items_.end(), CompItems());

	#ifndef NDEBUG
	POST_SORTER_STREAM("After sort " << items_ << "\n");
	checkTransitivity(items_);
	#endif

	ItemVector::iterator it = items_.begin();
	while (it != items_.end())
	{
		RenIDepthSortedItem* item = *it;

		if (item)
		{
			item->render(d3dDev);

			// As soon as the item is drawn, we can delete it.
			_DELETE(item);
			(*it) = NULL;
		}

		++it;
	}

	// Empty the buffer for the next render pass.
	items_.erase(items_.begin(), items_.end());
}*/

void RenIDepthPostSorter::render()
{
	#ifndef NDEBUG
	POST_SORTER_STREAM("Before sort " << items_ << "\n");
	checkTransitivity(items_);
	#endif

	// The list of polygons must be sorted by depth.  Timing shows that
	// sorting is insignificant when compared with the rendering.
	sort(items_.begin(), items_.end(), CompItems());

	#ifndef NDEBUG
	POST_SORTER_STREAM("After sort " << items_ << "\n");
	checkTransitivity(items_);
	#endif

	ItemVector::iterator it = items_.begin();
	while (it != items_.end())
	{
		RenIDepthSortedItem* item = *it;

		if (item)
		{
			item->render();

			// As soon as the item is drawn, we can delete it.
			_DELETE(item);
			(*it) = NULL;
		}

		++it;
	}

	// Empty the buffer for the next render pass.
	items_.erase(items_.begin(), items_.end());
}

void RenIDepthPostSorter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End POLYSORD.CPP *************************************************/
