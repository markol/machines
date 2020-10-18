/*
 * S U R F M G R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <string>
#include "stdlib/string.hpp"
#include "render/internal/surfmgri.hpp"
#include "render/texture.hpp"
#include "render/internal/texbody.hpp"
#include "render/internal/surfbody.hpp"

// static
const Ren::TexId RenISurfaceManagerImpl::firstValidId_ = Ren::NullTexId + 1;

static void lowerString( std::string* pString )
{
	if (!pString || pString->length() == 0)
		return;

    char ch;
    for( size_t i = 0; (ch = (*pString)[i]) != '\0'; ++i )
        (*pString)[i] = (char)tolower( ch );
}

RenISurfaceManagerImpl::RenISurfaceManagerImpl():
	handleDevice_(NULL),
	stateChanges_(0),
	handleSets_(0)
{
    //  Always search the current directory for textures by default
    directorySearchList_.reserve( 4 );
    directorySearchList_.push_back( SysPathName( "." ) );

	// Create a default entry corresponding to Ren::NullTexId.
	RenISurfBody* defaultEntry = _NEW(RenITexBody);

	ASSERT(entries_.size() == 0, logic_error());
    entries_.reserve( 1024 );
	entries_.push_back(defaultEntry);
	ASSERT(entries_[Ren::NullTexId] == defaultEntry, logic_error());

	// Add an extra reference to the default body, so that it doesn't get
	// deleted for the lifetime of this manager object.
	incRefCount(Ren::NullTexId);
}

RenISurfaceManagerImpl::~RenISurfaceManagerImpl()
{
	if (entries_[Ren::NullTexId] && entries_[Ren::NullTexId]->refCount() > 0)
		decRefCount(Ren::NullTexId);

	// When this dtor is called, all clients should have already deleted, any
	// textures which they are holding.  However, there are some D3D MeshBuilder
	// objects in the render lib which Direct3D doesn't appear to delete properly.
	// So, this dtor checks for extant textures and deletes them all.
	for (Ren::TexId id=0; id!=entries_.size(); ++id)
	{
		// Derefernce all remaining surfaces.  This assumes that no client will
		// actually use a surface after the manager is destroyed.
		if (entries_[id])
		{
			const uint remainingCount = entries_[id]->refCount();
			if (remainingCount> 0)
			{
				if (id == Ren::NullTexId)
					RENDER_STREAM("Tex mgr removing default tex body, ref=" << remainingCount << std::endl);
				else
					RENDER_STREAM("Tex mgr removing " << *(entries_[id]) << ", ref=" << remainingCount << std::endl);
			}

			for (uint i=0; i!=remainingCount; ++i)
				decRefCount(id);
		}
	}
}

Ren::TexId RenISurfaceManagerImpl::createSurfOrTex
(
	const std::string& name,
	bool createTex,
	const RenISurfBody* surf
)
{
	PRE(name.length() > 0);
	PRE(implies(!createTex, surf));

	//const SysPathName pathName = name;
	const SysPathName pathName ( name );

	bool searchLeaf = false;
	if( createTex or pathName.components().size() == 1 )
		searchLeaf = true;

	std::string searchName;
	if( searchLeaf )
	{
		searchName = pathName.components().back();
	}
	else
	{
		searchName = name;
	}

	// Check to see if the given file has already been loaded.
	const Ren::TexId extantId = findSharedSurface(searchName);

	if (extantId != Ren::NullTexId)
		return extantId;

	// The requested surface does not exist already, so load it using the
	// search path.
	RenISurfBody* newBody = NULL;

	// Only use the texture search path if the given name is a leaf file name with
	// no directory components.
	if (pathName.components().size() > 1)
	{
        newBody = loadActualSurface(name, createTex, surf);
        if( not newBody )
        {
            RENDER_STREAM("Could not load surface " << name << std::endl);
        }
	}
	else
	{
        PathNames texturePathNames = directorySearchList_;
	    for( PathNames::iterator i = texturePathNames.begin(); i != texturePathNames.end(); ++i )
	        (*i).combine(string(name));
	        //(*i).combine(name);

        newBody = loadSurface(texturePathNames, createTex, surf);

        if( not newBody )
        {
            RENDER_STREAM("Could not load surface " << name << std::endl);
            RENDER_STREAM("Searched files:" << std::endl);

            for( PathNames::iterator i = texturePathNames.begin(); i != texturePathNames.end(); ++i )
                RENDER_STREAM("    " << (*i) << std::endl);
        }
	}

    if (newBody)
    {
        RENDER_STREAM("Loaded " << *newBody << std::endl);
    }
    else
    {
		return Ren::NullTexId;
    }

	newBody->shareLeafName( searchLeaf );
	const Ren::TexId newId = addEntry( newBody );

	// Textures are always read-only and sharable.
	newBody->makeReadOnlySharable();

	return newId;
}

RenSurface RenISurfaceManagerImpl::createDisplaySurface
(
	const RenDevice* dev,
	RenI::DisplayType type
)
{
    TEST_INVARIANT;

	RenISurfBody* newSurf = _NEW(RenISurfBody(dev, type));

	Ren::TexId newId = addAnonymousEntry( newSurf );

	RenSurface retval(newId);

    TEST_INVARIANT;
	POST(!retval.sharable() && !retval.readOnly());
	POST(retval.name().length() == 0);
	return retval;
}

// Find an existing empty slot in the entries_ array, or if there
// isn't one already, create space for one.
Ren::TexId RenISurfaceManagerImpl::findFreeSlot()
{
    TEST_INVARIANT;

	// First, scan for released slots.
	Ren::TexId id = 0;
	for (; id!=entries_.size(); ++id)
	{
		if (!entries_[id])
			return id;
	}

	// There are no NULL slots, so extend the array.
	entries_.push_back(NULL);

    TEST_INVARIANT;
	POST(id < entries_.size());
	POST(entries_[id] == NULL);
	return id;
}

RenISurfBody* RenISurfaceManagerImpl::loadSurface
(
	const PathNames& pathNames,
	bool createTex,
	const RenISurfBody* surf
) const
{
	PRE(implies(!createTex, surf));
    TEST_INVARIANT;

    SysPathName texturePathName;
    bool        foundFile = false;

    // Find the directory that the texture exists in.
    for( PathNames::const_iterator i = pathNames.begin(); i != pathNames.end() and not foundFile; ++i )
    {
        ASSERT_INFO( *i );
        if( (*i).existsAsFile() )
        {
            texturePathName = (*i);
            foundFile = true;
        }
    }

    ASSERT( foundFile, "Did not find texture file" );

	RenISurfBody* body = loadActualSurface( texturePathName.pathname(), createTex, surf);

	POST(implies(body && !createTex, !body->castToTexBody()));
	POST(implies(body &&  createTex,  body->castToTexBody()));
	return body;
}

// Search to see if a requested texture exists already.
Ren::TexId RenISurfaceManagerImpl::findSharedSurface(const PathNames& pathNames) const
{
    TEST_INVARIANT;

	Ren::TexId id = Ren::NullTexId;
    for( PathNames::const_iterator i = pathNames.begin(); i != pathNames.end(); ++i )
    {
    	id = findSharedSurface((*i).pathname());
		if (id != Ren::NullTexId)
	    	break;
    }

	return id;
}

// Search to see if a requested texture exists already.
Ren::TexId RenISurfaceManagerImpl::findSharedSurface(const std::string& name) const
{
    TEST_INVARIANT;

	std::string searchName = name;
	lowerString( &searchName );

	RENDER_STREAM("Looking for existing texture " << searchName << std::endl);

	NameMap::const_iterator it = nameMap_.find( searchName );
	if( it != nameMap_.end() and entries_[ (*it).second ]->sharable() )
	{
   		RENDER_STREAM("  returning id " << (*it).second << std::endl);
   	   	return (*it).second;
	}

	RENDER_STREAM("  returning null id " << std::endl << std::endl);
	return Ren::NullTexId;
}

void RenISurfaceManagerImpl::incRefCount(Ren::TexId id)
{
	TEST_INVARIANT;
	PRE(id < entries_.size());
	PRE(entries_[id]);

	// Although the entries may contain both a surface and a texture, the
	// reference count is in the surface.
	entries_[id]->incRefCount();

	// Detailed, very inefficient debugging.
	// RENDER_STREAM("Mgr inc'd count to " << entries_[id]->refCount() <<
	//	          " for " << *(entries_[id]) << std::endl);
	TEST_INVARIANT;
}

void RenISurfaceManagerImpl::decRefCount(Ren::TexId id)
{
	TEST_INVARIANT;
	PRE_INFO(id);
	PRE_INFO(entries_.size());
	PRE(id < entries_.size());
	PRE(entries_[id]);
	PRE(entries_[id]->refCount() > 0);

	// Although the entries may contain both a surface and a texture, the
	// reference count is in the surface.
	entries_[id]->decRefCount();

	// Detailed, very inefficient debugging.
	// RENDER_STREAM("Mgr dec'd count to " << entries_[id]->refCount()	<<
	//	          " for " << *(entries_[id]) << std::endl);

	// If the reference count is zero, this texture is no longer used by
	// any client.  It might be desirable to delay the delete until the memory
	// used by the texture is required for something else, just on the off
	// chance that the texture might be reloaded.
	if (entries_[id]->refCount() == 0)
	{
		// Remove the body and mark the slot as free.
		deleteEntry( id );
	}

	TEST_INVARIANT;
}

RenITexBody* RenISurfaceManagerImpl::getTexBody(Ren::TexId id)
{
	PRE(id < entries_.size());
	PRE(entries_[id]);
	PRE(entries_[id]->castToTexBody());
	TEST_INVARIANT;
	return entries_[id]->castToTexBody();
}

RenISurfBody* RenISurfaceManagerImpl::getSurface(Ren::TexId id)
{
	PRE(id < entries_.size());
	PRE(entries_[id]);
	TEST_INVARIANT;
	return entries_[id];
}

Ren::TexId RenISurfaceManagerImpl::getBodyId(const RenISurfBody* body)
{
	if (body)
	{
		for (Ren::TexId id = firstValidId_; id!=entries_.size(); ++id)
			if (entries_[id] == body)
				return id;
	}

	return Ren::NullTexId;
}

void RenISurfaceManagerImpl::useDevice
(
	const RenDevice* dev,
	RenI::UpdateType updateType
)
{
	PRE(dev);
	TEST_INVARIANT;

	// Only update the handles if this is a different device from the last
	// one.  (Typically, there's only one device.)	The device may have changed
	// its internals, if so, it will set the force update flag.
	if (updateType == RenI::FORCE_UPDATE || dev != handleDevice_)
	{
		handleDevice_ = dev;

		for (Ren::TexId id = firstValidId_; id!=entries_.size(); ++id)
		{
			RenISurfBody* entry = entries_[id];
			//if (entry)
				//entry->useDevice(dev);
		}
	}

	TEST_INVARIANT;
}

// For debugging, put this into the render stream for context.  Also, put it
// into Daniel's stream because that's mostly empty and can be turned on for
// large games without creating a huge file.
#define TEX_HANDLE_STREAM(x)	{ RENDER_STREAM(x); DANIEL_STREAM(x); }

Ren::TexId RenISurfaceManagerImpl::addAnonymousEntry( RenISurfBody* newSurf )
{
	PRE( newSurf );

	Ren::TexId newId = findFreeSlot();
	ASSERT(newId < entries_.size(), logic_error());
	ASSERT(entries_[newId] == NULL, logic_error());

	entries_[newId] = newSurf;
	return newId;
}

Ren::TexId RenISurfaceManagerImpl::addEntry( RenISurfBody* newSurf )
{
	PRE( newSurf );

	Ren::TexId newId = findFreeSlot();
	ASSERT(newId < entries_.size(), logic_error());
	ASSERT(entries_[newId] == NULL, logic_error());

	entries_[newId] = newSurf;
	std::string searchName = newSurf->sharedName();
	lowerString( &searchName );

	ASSERT_INFO( newSurf->sharedName() );
	ASSERT_DATA( NameMap::iterator it = nameMap_.find( searchName ) );
	ASSERT( it == nameMap_.end(), "texture has already been loaded (possibly from another directory)" );

	RENDER_STREAM( newSurf->name() << " saved in search map as " << searchName << std::endl);
	nameMap_.insert( NameMap::value_type( searchName, newId ) );

	TEST_INVARIANT;
	return newId;
}

void RenISurfaceManagerImpl::deleteEntry( Ren::TexId id )
{
	TEST_INVARIANT;

	std::string searchName = entries_[id]->sharedName();
	lowerString( &searchName );

	if( searchName.length() > 0 )
	{
		NameMap::iterator i = nameMap_.find( searchName );
		if( i != nameMap_.end() )
		{
			ASSERT_INFO( id );
			ASSERT_INFO( entries_[id]->sharedName() );
			ASSERT_INFO( (*i).first );
			ASSERT( entries_[id]->matches( (*i).first ), "Consistency error between entries_ and nameMap_ " );
			nameMap_.erase(i);
		}
	}

	_DELETE( entries_[id] );
	entries_[id] = NULL;

	TEST_INVARIANT;
}

void RenISurfaceManagerImpl::CLASS_INVARIANT
{
	// All textures are shared and read-only.
	for (Ren::TexId id = RenISurfaceManagerImpl::firstValidId_; id!=entries_.size(); ++id)
	{
		RenISurfBody* entry = entries_[id];

		if (entry)
			INVARIANT(implies(entry->castToTexBody(), entry->sharable() && entry->readOnly()));
	}

	for( NameMap::const_iterator it = nameMap_.begin(); it != nameMap_.end(); ++it )
	{
		INVARIANT( entries_[ (*it).second ] );
		INVARIANT(  entries_[ (*it).second ]->matches( (*it).first ) );
	}
}

ostream& RenISurfaceManagerImpl::write(ostream& o)
{
	uint total = 0;
	for (Ren::TexId id = firstValidId_; id!= entries_.size(); ++id)
	{
		RenISurfBody* entry = entries_[id];

		if (!entry)
			o << id << "\tempty slot" << std::endl;
		else
		{
			RenITexBody* pTexBoby = entry->castToTexBody();
			if( pTexBoby != NULL  ) total += pTexBoby->memoryUsed();
			o << id << "\t" << *entry << " refcount " << entry->refCount() << std::endl;
		}
	}

	o << " Total texture memory used = " << total << std::endl;

    return o;
}

RenISurfBody* RenISurfaceManagerImpl::loadActualSurface
(
	const std::string& pathName,
	bool createTex,
	const RenISurfBody* surf
) const
{
	PRE(implies(!createTex, surf));
    ASSERT_FILE_EXISTS( pathName.c_str() );
    TEST_INVARIANT;

	RenISurfBody* body = NULL;
	if (createTex)
		body = _NEW(RenITexBody);
	else
		body = _NEW(RenISurfBody(surf->pixelFormat()));

	ASSERT(body, runtime_error("Out of memory."));

	// Attempt to read the texture from file.
	if (!body->read(pathName))
	{
		_DELETE(body);
		body = NULL;
	}

	POST(implies(body && !createTex, !body->castToTexBody()));
	POST(implies(body &&  createTex,  body->castToTexBody()));
	return body;
}

/* End SURFMGR.CPP ***************************************************/
