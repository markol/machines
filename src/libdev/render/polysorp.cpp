/*
 * P O L Y S O R P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/internal/polysorp.hpp"
#include "ctl/algorith.hpp"
#include "render/device.hpp"
#include "render/texture.hpp"

RenIPrioritySortedItem::RenIPrioritySortedItem( const RenMaterial& m)
: material_(m),
  depth_(0)
{
}

bool RenIPrioritySortedItem::operator<(const RenIPrioritySortedItem& item) const
{
	return depth() > item.depth();
}

MATHEX_SCALAR RenIPrioritySortedItem::depth() const
{
	return depth_;
}

ostream& operator<<(ostream& o, const RenIPrioritySortedItem& i)
{
	i.print(o);
	return o;
}

class CompItems
{
public:
	bool operator()(const RenIPrioritySortedItem* i1, const RenIPrioritySortedItem* i2)
	{
		PRE(i1 && i2);
		return *i1 < *i2;
	}
};

RenIPriorityPostSorter::RenIPriorityPostSorter()
{
	items_.reserve(200);
    TEST_INVARIANT;
}

RenIPriorityPostSorter::~RenIPriorityPostSorter()
{
    TEST_INVARIANT;
}

void RenIPriorityPostSorter::addItem(std::auto_ptr<RenIPrioritySortedItem>& item)
{
	// release implies that ownership is transferred from the caller.
	items_.push_back(item.release());
}

void RenIPriorityPostSorter::render()
{
	// The list of polygons must be sorted by depth.  Timing shows that
	// sorting is insignificant when compared with the rendering.
	sort(items_.begin(), items_.end(), CompItems());

	RENDER_STREAM("Priority post sorter contains " << items_.size() << " items." << std::endl);
	RENDER_INDENT(2);
	//Ren::out() << "Post sorter contains " << items_.size() << " items:" << std::endl;

	ItemVector::iterator it = items_.begin();
	while (it != items_.end())
	{
		RenIPrioritySortedItem* item = *it;

		RENDER_STREAM("  " << *item << std::endl);
		//Ren::out() << "  " << *item << std::endl;

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

	RENDER_INDENT(-2);
	RENDER_STREAM(std::endl);
}

void RenIPriorityPostSorter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End POLYSORT.CPP *************************************************/
