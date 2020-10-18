/*
 * P O L Y S O R D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_POLYSORD_HPP
#define _RENDER_POLYSORD_HPP

#include "base/base.hpp"
#include "stdlib/memory.hpp"
#include <memory>
#include "mathex/mathex.hpp"
#include "ctl/vector.hpp"
#include "render/material.hpp"

class RenIDepthSortedItem
{
public:
	RenIDepthSortedItem( const RenMaterial& );
	virtual ~RenIDepthSortedItem()	{}
	virtual void render() = 0;
	MATHEX_SCALAR depth() const;

	ushort meshId() const			{ return meshId_; }
	void meshId(ushort i)			{ meshId_ = i; }

	// The default implementation orders the items in decreasing depth.
	virtual bool operator<(const RenIDepthSortedItem&) const;

	virtual void print(ostream&) const = 0;

protected:
    const RenMaterial material_;
	MATHEX_SCALAR depth_;
	ushort meshId_;
};

ostream& operator<<(ostream&, const RenIDepthSortedItem&);

// Certain polygons cannot be correctly prioritised by z-buffering.  These
// are dumped into lists of this type during rendering, then when the scene
// is finished, this class draws them all in sorted order, without using
// the z-buffer.
class RenIDepthPostSorter
{
public:
    RenIDepthPostSorter();
    ~RenIDepthPostSorter();

	// Ownership of the item *passes* from the caller to this object.
	void addItem(std::auto_ptr<RenIDepthSortedItem>& item);
	void render();

    void CLASS_INVARIANT;

private:
	typedef ctl_vector<RenIDepthSortedItem*>	ItemVector;
	ItemVector items_;

    // Operations deliberately revoked
    RenIDepthPostSorter( const RenIDepthPostSorter& );
    RenIDepthPostSorter& operator =( const RenIDepthPostSorter& );
    bool operator ==( const RenIDepthPostSorter& );
};

#endif

/* End POLYSORD.HPP *************************************************/
