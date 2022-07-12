/*
 * S U R F M G R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/surfmgr.hpp"

#include <string>

#include "render/internal/surfmgri.hpp"
#include "render/texture.hpp"
#include "render/device.hpp"
#include "render/internal/texbody.hpp"
#include "render/internal/surfbody.hpp"
#include "render/internal/devicei.hpp"

#include "system/pathname.hpp"
#include "afx/resource.hpp"

#include <stdio.h>
#include <climits>
#include <algorithm>

////////////////////////////////////////////////////////////

#define CB_REN_SURFACE_MANAGER_DEPIMPL                       \
            CB_DEPIMPL(ctl_vector<RenISurfBody*>, entries_); \
            CB_DEPIMPL(RenISurfaceManagerImpl::NameMap, nameMap_); \
			CB_DEPIMPL(PathNames, directorySearchList_);     \
			CB_DEPIMPL(const Ren::TexId, firstValidId_);     \
			CB_DEPIMPL(const RenDevice*, handleDevice_)

#define CB_PEER_PTR_DEPIMPL(objectPtr, name) objectPtr->pImpl_->name
#define CB_PEER_REF_DEPIMPL(objectRef, name) objectRef.pImpl_->name

////////////////////////////////////////////////////////////

// static
RenSurfaceManager::RenSurfaceManager():
	pImpl_(_NEW(RenISurfaceManagerImpl()))
{
	PRE(Ren::initialised());
    TEST_INVARIANT;
}

RenSurfaceManager::~RenSurfaceManager()
{
    TEST_INVARIANT;
	_DELETE(pImpl_);
}

// As an anonymous surface, this cannot be shared, so we don't search for a
// matching name in the existing surfaces.
RenSurface RenSurfaceManager::createAnonymousSurface
(
	size_t width,
	size_t height,
	const RenSurface& takePixelFormatFrom
)
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	PRE(!takePixelFormatFrom.isNull());
	PRE(width > 0 && height > 0);
    TEST_INVARIANT;

	if (width == 0 || height == 0)
		return RenSurface();

	const RenIPixelFormat& rqFormat = entries_[takePixelFormatFrom.myId_]->pixelFormat();
	RenISurfBody* newSurf = _NEW(RenISurfBody(width, height, rqFormat, RenISurfBody::SYSTEM));

	Ren::TexId newId = pImpl_->addAnonymousEntry( newSurf );

	RenSurface retval(newId);


    TEST_INVARIANT;
	POST(!retval.sharable() && !retval.readOnly());
	POST(retval.name().length() == 0);
	POST(retval.width() == width && retval.height() == height);
	return retval;
}

RenSurface RenSurfaceManager::createAnonymousVideoSurface(
	size_t width,
	size_t height,
	const RenSurface& pixelFmt)
{
	PRE(!pixelFmt.isNull());
	PRE(width > 0 and height > 0);

	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;

	if(width == 0 or height == 0)
		return RenSurface();

	const RenIPixelFormat& rqFormat = entries_[pixelFmt.myId_]->pixelFormat();
	RenISurfBody* newSurf = _NEW(RenISurfBody(width, height, rqFormat, RenISurfBody::VIDEO));

	Ren::TexId newId = pImpl_->addAnonymousEntry( newSurf );

	RenSurface result(newId);

	POST(!result.sharable() and !result.readOnly());
	POST(result.name().length() == 0);

	return result;
}

RenSurface RenSurfaceManager::createSharedSurface
(
	const std::string& bitmapName,
	const AfxResourceLib& lib,
	const RenSurface& takePixelFormatFrom
)
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;
    TEST_INVARIANT;
	PRE(bitmapName.length() > 0);
	PRE(!takePixelFormatFrom.isNull());
	PRE(entries_[takePixelFormatFrom.myId_]);

	const std::string name = lib.fileName().filename() + ":" + bitmapName;

	// Check to see if the given bitmap is already loaded.
	Ren::TexId returnId = pImpl_->findSharedSurface(name);
	if (returnId == Ren::NullTexId)
	{
		
	}

	RenSurface retval(returnId);
	POST(retval.sharable() && retval.readOnly());
    TEST_INVARIANT;
	return retval;
}

RenSurface RenSurfaceManager::createSharedSurface(const std::string& name, const RenSurface& surf)
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

    TEST_INVARIANT;

	const Ren::TexId newId = pImpl_->createSurfOrTex(name, false, surf.internals());

	RenSurface retval(newId);
	POST(retval.sharable() && retval.readOnly());
	POST(implies(!retval.isNull(), retval.name().length() > 0));
    TEST_INVARIANT;
	return retval;
}

RenTexture RenSurfaceManager::createTexture(const std::string& pathName)
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

    TEST_INVARIANT;

    std::string pathNameLower(pathName);
    std::transform(pathNameLower.begin(), pathNameLower.end(), pathNameLower.begin(), ::tolower);
	const Ren::TexId newId = pImpl_->createSurfOrTex(pathNameLower, true, NULL);

	// The D3D handles are only updated if the current device changes.  If
	// a texture is created and the device doesn't change before it is used,
	// then it won't have the correct handle.
	RenISurfBody* entry = entries_[newId];
	if (entry && handleDevice_)
	{
		RenITexBody* tex = entry->castToTexBody();
	}

	RenTexture retval(newId);
	POST(retval.sharable() && retval.readOnly());
	POST(implies(!retval.isNull(), retval.name().length() > 0));
    TEST_INVARIANT;
	return retval;
}

// There can be null slots, so this isn't necessarily entries_.size().
uint RenSurfaceManager::nTexturesLoaded() const
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;

	uint count = 0;
	for (Ren::TexId id = firstValidId_; id!=entries_.size(); ++id)
	{
		RenISurfBody* entry = entries_[id];
		if (entry && entry->castToTexBody())
			++count;
	}

	return count;
}

uint RenSurfaceManager::memoryUsed() const
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;

	uint mem = 0;
	for (Ren::TexId id = firstValidId_; id!=entries_.size(); ++id)
	{
		RenISurfBody* entry = entries_[id];
		if (entry)
			mem += entry->memoryUsed();
	}

	return mem;
}

void RenSurfaceManager::startFrame()
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;

	if (pImpl_->handleSets_ > 0)
	{
		const double percent = 100.0 * pImpl_->stateChanges_ / pImpl_->handleSets_;
		RENDER_STREAM("Texture handle requests=" << pImpl_->handleSets_ << "\n");
		RENDER_STREAM("Actual state changes   =" << pImpl_->stateChanges_ << " (" << percent << "%)\n");
	}

	pImpl_->handleSets_ = pImpl_->stateChanges_ = 0;

	// Mark this as not used at all.
	pImpl_->lastUsedId_ = UINT_MAX;


	TEST_INVARIANT;
}

bool RenSurfaceManager::restoreAll()
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;

	// Stop trying if any one of the textures does not reload.
	bool success = true;
	for (Ren::TexId id = firstValidId_; id!=entries_.size() && success; ++id)
	{
		RenISurfBody* entry = entries_[id];
		if (entry)
			success = entry->restoreToVRAM();
	}

	TEST_INVARIANT;
	return success;
}

const RenSurfaceManager::PathNames&  RenSurfaceManager::searchList()
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;
    return directorySearchList_;
}

void  RenSurfaceManager::searchList( const PathNames& newSearchList )
{
	CB_REN_SURFACE_MANAGER_DEPIMPL;

	TEST_INVARIANT;
    directorySearchList_ = newSearchList;
}

// static
RenSurfaceManager& RenSurfaceManager::instance()
{
    static RenSurfaceManager instance_;
    return instance_;
}

ostream& operator <<( ostream& o, const RenSurfaceManager& t )
{
    o << "RenSurfaceManager." << std::endl;
	t.pImpl_->write(o);
    o << "Total memory used " << t.memoryUsed() << std::endl;

    return o;
}

void RenSurfaceManager::CLASS_INVARIANT
{
	INVARIANT(this);
	INVARIANT(pImpl_);
}

RenISurfaceManagerImpl& RenSurfaceManager::impl()
{
	PRE(pImpl_);
	return *pImpl_;
}

const RenISurfaceManagerImpl& RenSurfaceManager::impl() const
{
	PRE(pImpl_);
	return *pImpl_;
}

inline bool myIsPrint(int c)
{
	return isprint(c) || isspace(c);
}

// static
bool RenSurfaceManager::extractBMPText(const SysPathName& pathName, std::string* result)
{
	PRE(result);
	const int END_OF_FILE = 0xFF;
	std::string& resultString = *result;

	FILE* imgFile = fopen(pathName.pathname().c_str(), "r+b");
	if (!imgFile)
	{
		RENDER_STREAM("Failed to open file " << pathName << "\n");
		return false;
	}

	return true;
}

/* End SURFMGR.CPP ***************************************************/
