/***********************************************************

  S U R F M G R I . H P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifndef _RENDER_INTERNAL_SURFACE_MANAGER_IMPL
#define _RENDER_INTERNAL_SURFACE_MANAGER_IMPL

#include <string>
#include "render/render.hpp"
#include "system/pathname.hpp"
#include "ctl/vector.hpp"
#include "ctl/map.hpp"
#include "render/internal/internal.hpp"

class RenDevice;
class RenITexBody;
class RenISurfBody;
class RenTexture;
class RenSurface;

// Pattern: Pimple-Implementation class for a Singleton client.
class RenISurfaceManagerImpl
// Cannonical form revoked.
{
public:
	RenISurfaceManagerImpl();
	~RenISurfaceManagerImpl();
    void CLASS_INVARIANT;

	void stateChange()		{ ++stateChanges_; }

	ostream& write(ostream& o);

private:
	typedef ctl_vector<SysPathName> PathNames;

	friend class RenSurface;
	friend class RenTexture;
	void incRefCount(Ren::TexId);
	void decRefCount(Ren::TexId);
	RenITexBody*  getTexBody(Ren::TexId);	// TBD: inline
	RenISurfBody* getSurface(Ren::TexId);	// TBD: inline
	Ren::TexId    getBodyId(const RenISurfBody*);

	// add an entry to entries_ and nameMap_
	Ren::TexId addEntry( RenISurfBody* newSurf );

	// add an entry to entries_ but not to nameMap_
	Ren::TexId addAnonymousEntry( RenISurfBody* newSurf );

	// delete entry_[id] and the corresponding nameMap_ element if any
	void deleteEntry( Ren::TexId id );

	friend class RenDevice;
	friend class RenIDeviceImpl;

	RenSurface createDisplaySurface(const RenDevice* dev, RenI::DisplayType t);
    void useDevice(const RenDevice* dev,
				   RenI::UpdateType updateType);

	// Internal convenience methods.
    // Search for existing named surfaces.
	friend class RenSurfaceManager;
	Ren::TexId findSharedSurface(const PathNames& pathNames) const;
	Ren::TexId findSharedSurface(const std::string& name) const;

	// Does the work common to createSharedSurface and createTexture.
	Ren::TexId createSurfOrTex(const std::string& name, bool createTex, const RenISurfBody*);

    // Try and load the list of names in order.
	// POST(implies( createTex,  body->castToTexBody()));
	// POST(implies(!createTex, !body->castToTexBody()));
	RenISurfBody* loadSurface(const PathNames& pathNames, bool createTex, const RenISurfBody*) const;

    //Explicitly load a surface or texture with given pathname.
    RenISurfBody* loadActualSurface( const std::string& pathName, bool createTex, const RenISurfBody* surf) const;

	Ren::TexId findFreeSlot();

	typedef ctl_map< std::string, Ren::TexId, std::less< std::string > > NameMap;

	// This is the first slot after Ren::NullTexId.
	static const Ren::TexId firstValidId_;
	Ren::TexId lastUsedId_;

	// All the textures loaded into memory.  Can contain NULL entries
	// for textures that were loaded and then released.  Entries are
	// never moved, so if a client RenTexture holds a TexId corresponding
	// to its entry in the array, it won't change.  However, when all
	// references expire, the texture may be released and the slot reused.
	ctl_vector<RenISurfBody*> entries_;

	// maps names to ids (thus for a give name we should always have:
	// entries_ [ nameMap_( name ).second ]->name() == name
	NameMap nameMap_;

    //  The list of directories to search for textures
    PathNames directorySearchList_;

	// The last device given to the useDevice method.
	const RenDevice* handleDevice_;

	// Statistic gathering.
	ulong		handleSets_, stateChanges_;
};

#endif /* _RENDER_INTERNAL_SURFACE_MANAGER_IMPL ***********/

