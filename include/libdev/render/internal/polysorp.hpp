/*
 * P O L Y S O R P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_POLYSORP_HPP
#define _RENDER_POLYSORP_HPP

#include "base/base.hpp"
#include "stdlib/memory.hpp"
#include "mathex/mathex.hpp"
#include "ctl/vector.hpp"
#include "render/material.hpp"
#include <memory>

class RenIPrioritySortedItem
{
public:
	RenIPrioritySortedItem( const RenMaterial& material);
	virtual ~RenIPrioritySortedItem()	{}
	virtual void render() = 0;
	MATHEX_SCALAR depth() const;

	// The default implementation orders the items in decreasing depth.
	virtual bool operator<(const RenIPrioritySortedItem&) const;

	virtual void print(ostream&) const = 0;

protected:
	const RenMaterial		material_;
	MATHEX_SCALAR			depth_;
};

ostream& operator<<(ostream&, const RenIPrioritySortedItem&);

// Certain polygons cannot be correctly prioritised by z-buffering.  These
// are dumped into lists of this type during rendering, then when the scene
// is finished, this class draws them all in sorted order, without using
// the z-buffer.
class RenIPriorityPostSorter
{
public:
    RenIPriorityPostSorter();
    ~RenIPriorityPostSorter();

	// Ownership of the item *passes* from the caller to this object.
	void addItem(std::auto_ptr<RenIPrioritySortedItem>& item);
	void render();

    void CLASS_INVARIANT;

private:
	typedef ctl_vector<RenIPrioritySortedItem*>	ItemVector;
	ItemVector items_;

    // Operations deliberately revoked
    RenIPriorityPostSorter( const RenIPriorityPostSorter& );
    RenIPriorityPostSorter& operator =( const RenIPriorityPostSorter& );
    bool operator ==( const RenIPriorityPostSorter& );
};

#endif

/* End POLYSORP.HPP *************************************************/
