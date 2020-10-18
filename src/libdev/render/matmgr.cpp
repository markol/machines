/*
 * M A T M G R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/matmgr.hpp"

#include <iomanip>
#include <algorithm>
#include "ctl/vector.hpp"
#include "render/material.hpp"
#include "render/internal/matbody.hpp"
#include <limits.h>

// static
RenIMatManager& RenIMatManager::instance()
{
    static RenIMatManager instance_;
    return instance_;
}

RenIMatManager::RenIMatManager():
	bodies_(_NEW(Bodies)),
	unusedBodies_(_NEW(Bodies)),
	defaultBody_(_NEW(RenIMatBody)),
	nullSlots_(0),
	minPriority_(SHRT_MAX),
	maxPriority_(SHRT_MIN),
	globalMaterialXform_(NULL)
{
	PRE(Ren::initialised());
	ASSERT(bodies_, "Out of memory");

	bodies_->reserve(1000);
    unusedBodies_->reserve( 128 );

	// Ensure that the default body won't get deleted and add it to the set.
	defaultBody_->incRefCount();
	bodies_->push_back(defaultBody_);

    TEST_INVARIANT;
}

RenIMatManager::~RenIMatManager()
{
    TEST_INVARIANT;

    clearUnusedMatBodyList();

	if (defaultBody_)
	{
		defaultBody_->decRefCount();
		defaultBody_ = NULL;
	}

	if (bodies_)
	{
		// By the time the material manager is destroyed, no client should still
		// be holding a material.
		RenIMatManager::Bodies::iterator it = bodies_->begin();
		while (it !=bodies_->end())
		{
			RenIMatBody* ptr = *it;
			// TBD: put this check in when all memory leaks have been fixed in clients.
			// ASSERT(!ptr || ptr->refCount() == 0, "Leaked material in mat mgr shutdown");
			++it;
		}

		_DELETE(bodies_);
		bodies_ = NULL;
	}

    _DELETE( unusedBodies_ );
}

RenIMatBody* RenIMatManager::defaultBody()
{
	return defaultBody_;
}

// This is a linear search.  If too much time is being spent searching for
// material bodies, the performance may need to be improved.  One possiblity
// is to sort the vector and use a binary search.
RenIMatManager::Bodies::iterator findSharableBody(RenIMatManager::Bodies* vec, RenIMatBody* body)
{
	// It doesn't make sense to search for a copy of a non-sharable body.
	PRE(vec && body);
	PRE(body->sharable());

	RenIMatManager::Bodies::iterator it = vec->begin();
	while (it !=vec->end())
	{
		RenIMatBody* ptr = *it;

		// Don't include non-sharable bodies in the search.
		if (ptr && ptr->sharable() && (ptr == body || *ptr == *body))
			break;
		++it;
	}

	return it;
}

RenIMatBody* RenIMatManager::createBody(const RenColour& dif)
{
	RenIMatBody* newBody = _NEW(RenIMatBody(dif));

	DBG_MAT_SHARE(Diag::instance().renderStream() << " created new material body with " << newBody->texture() << std::endl);
	Bodies::iterator it = findSharableBody(bodies_, newBody);

	// If an extant, sharable version of newBody was found, we can delete it.
	if (it != bodies_->end())
	{
		DBG_MAT_SHARE(Diag::instance().renderStream() << "Found existing copy of " << *newBody << std::endl);
		_DELETE(newBody);

		return *it;
	}
	else
	{
		DBG_MAT_SHARE(cout << "Created new " << *newBody << std::endl);
		addBody(newBody);
		return newBody;
	}
}

RenIMatBody* RenIMatManager::addPossiblyNewBody(RenIMatBody* body)
{
	PRE(body && bodies_);

	// If this body is non-sharable, then don't bother searching for duplicates.
	if (!body->sharable())
	{
		addBody(body);
		return body;
	}

	Bodies::iterator it = findSharableBody(bodies_, body);

	// If an extant version of newBody was found, we can delete it.
	if (it != bodies_->end())
	{
		DBG_MAT_SHARE(Diag::instance().renderStream() << "COW: found existing copy of " << *body << std::endl);
		return *it;
	}
	else
	{
		DBG_MAT_SHARE(Diag::instance().renderStream() << "COW: created new " << *body << std::endl);
		addBody(body);
		return body;
	}
}

RenIMatBody* RenIMatManager::findDuplicate(const RenIMatBody* body) const
{
	PRE(body && bodies_);
	PRE(body->sharable());

	RenIMatBody* retval = NULL;
	RenIMatManager::Bodies::iterator it = bodies_->begin();
	while (it !=bodies_->end())
	{
		RenIMatBody* ptr = *it;
		if (ptr != body && ptr && ptr->sharable() && *ptr == *body)
		{
			retval = ptr;
			break;
		}

		++it;
	}

	POST(retval != body);
	POST(!retval || *retval == *body);
	POST(implies(retval, retval->sharable()));
	return retval;
}

// If there are any null slots in the list of bodies, then don't call
// bodies_->push_back, but reuse an existing empty slot.  As an
// optimisation, we track the total number of null slots.
void RenIMatManager::addBody(RenIMatBody* body)
{
	if (nullSlots_ == 0)
		bodies_->push_back(body);
	else
	{
		Bodies::iterator it = find(bodies_->begin(), bodies_->end(), (RenIMatBody*) NULL);
		ASSERT(*it == NULL, logic_error("Null slot not found."));

		*it = body;
		--nullSlots_;
	}
}

void RenIMatManager::killMe(RenIMatBody* body)
{
	if (body && bodies_)
	{
		// NB: rather than use findBody, we must use std::find here.  The former
		// compares pointers *and* material body values.  If there were duplicate
		// bodies in the list, there is a chance that we could delete the wrong
		// one because their attribues match.  By using std:find, only the
		// pointers are compared.
		Bodies::iterator it = find(bodies_->begin(), bodies_->end(), body);

		// Rather than resize the vector, just set the element to null.
		if (it != bodies_->end())
		{
			*it = NULL;
			++nullSlots_;
		}

		_DELETE(body);
	}
}

void RenIMatManager::forceUpdateAll()
{
	if (bodies_)
	{
		RenIMatManager::Bodies::iterator it = bodies_->begin();
		while (it !=bodies_->end())
		{
			RenIMatBody* ptr = *it;
			if (ptr)
				ptr->update();



			++it;
		}
	}
}

void RenIMatManager::useDevice(const RenDevice&)
{
	/* I don't believe this is necessary now that we do our own lighting.
	if (bodies_)
	{
		RenIMatManager::Bodies::iterator it = bodies_->begin();
		while (it !=bodies_->end())
		{
			RenIMatBody* ptr = *it;
			if (ptr)
				ptr->updateD3DHandle(dev);

			++it;
		}
	}
	*/
}

void RenIMatManager::CLASS_INVARIANT
{
	INVARIANT(bodies_);
}

ostream& operator <<( ostream& o, const RenIMatManager& t )
{
	o << "----------------------------------------------------\n";
	o << "Material manager contains " << t.bodies_->size() << " materials:" << std::endl;

	double totalHandles = 0;
	ulong  totalBodies  = 0;
	RenIMatManager::Bodies::const_iterator it = t.bodies_->begin();
	while (it != t.bodies_->end())
	{
		const RenIMatBody* body = *it;

		if (!body)
			o << "- empty slot" << std::endl;
		else
		{
			o << "  " << *body << std::endl;
			totalHandles += body->refCount();
			++totalBodies;
		}

		++it;
	}

	o << "----------------------------------------------------\n";
	o << "Total bodies  = " << totalBodies  << "\n";
	o << "Total handles = " << totalHandles << "\n";
	o << "Sharing = " << 100.0 * totalHandles / totalBodies << "%" << "\n";
	o << "----------------------------------------------------\n";
	o << "Memory used:\n";
	o << "  bodies  = " << totalBodies  * sizeof(RenIMatBody) << " bytes\n";
	o << "  handles = " << totalHandles * sizeof(RenMaterial) << " bytes\n";
	o << "----------------------------------------------------" << std::endl << std::endl;

    return o;
}

// This comparison may not be universally useful, so it is not in colour.hpp.
inline bool operator<(const RenColour& c1, const RenColour& c2)
{
	return c1.r() < c2.r() || c1.g() < c2.g() || c1.b() < c2.b() || c1.packedAlpha() < c2.packedAlpha();
}

void RenIMatManager::clearUnusedMatBodyList()
{
    while( unusedBodies_->size() != 0 )
    {
        RenIMatBody* pBody = unusedBodies_->back();
        _DELETE( pBody );
        unusedBodies_->pop_back();
    }
}

void RenIMatManager::addUnusedMatBody( RenIMatBody* pBody )
{
    PRE( pBody->sharable() );
    unusedBodies_->push_back( pBody );
}

void RenIMatManager::checkCoplanarValue(short newPriority)
{
	if (newPriority > maxPriority_)
	{
		maxPriority_ = newPriority;
		RENDER_STREAM("New maximum coplanar priority=" << maxPriority_ << "\n");
	}

	if (newPriority < minPriority_)
	{
		minPriority_ = newPriority;
		RENDER_STREAM("New minimum coplanar priority=" << minPriority_ << "\n");
	}

	// This value shouldn't really be hardcoded here.
	ASSERT_DATA(const int maxDifference = 16);
	ASSERT_INFO(minPriority_);
	ASSERT_INFO(maxPriority_);
	ASSERT_INFO(newPriority);
	ASSERT(maxPriority_ - minPriority_ <= maxDifference, "All inter-mesh coplanar values must differ by no more than 16.");
}

const RenMaterialTransform* RenIMatManager::globalMaterialTransform() const
{
	return globalMaterialXform_;
}

void RenIMatManager::globalMaterialTransform(const RenMaterialTransform* xform)
{
	globalMaterialXform_ = xform;
}

/* End MATMGR.CPP ***************************************************/
