/*
 * T E X S E T I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/internal/texseti.hpp"
#include <string>
#include "base/istrrep.hpp"
#include "system/pathname.hpp"
#include "system/fileenum.hpp"
#include "system/filedata.hpp"
#include "render/surfmgr.hpp"

#define TEXSET_STREAM(x)	RENDER_STREAM(x)
#define TEXSET_INDENT(x)	RENDER_INDENT(x)

RenTextureSetImpl::RenTextureSetImpl()
: isLoaded_( false )
{
    TEST_INVARIANT;
}

RenTextureSetImpl::RenTextureSetImpl( const SysPathName& directory )
: isLoaded_( false )
{
	load( directory );

    TEST_INVARIANT;
}

RenTextureSetImpl::RenTextureSetImpl( const SysPathName& directory, BaseProgressReporter* pReporter )
: isLoaded_( false )
{
	PRE( pReporter );

	load( directory, pReporter );

    TEST_INVARIANT;
}

RenTextureSetImpl::~RenTextureSetImpl()
{
    TEST_INVARIANT;
	TEXSET_STREAM("Unloading textures " << std::endl);
	TEXSET_INDENT(2);
	TEXSET_STREAM(RenSurfaceManager::instance());

	textures_.erase(textures_.begin(), textures_.end());

	TEXSET_STREAM("Unloaded" << std::endl);
	TEXSET_STREAM(RenSurfaceManager::instance());
	TEXSET_INDENT(-2);
}

static bool isAlphaMap( const SysPathName& pathname )
{
	bool result=false;
	const std::string & texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_a.bmp" ) == 0)
		result = true;
	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	if( strcasecmp( end2.c_str(), "_ba.bmp" ) == 0 )
		result = true;
	return result;
}

bool isColourMap( const SysPathName& pathname )
{
	bool result=false;
	const std::string & texName = pathname.filename();
	const std::string  end = texName.substr(texName.length() - 6, 6);
	if (strcasecmp( end.c_str(), "_c.bmp" ) == 0)
		result = true;
	const std::string  end2 = texName.substr(texName.length() - 7, 7);
	if( strcasecmp( end2.c_str(), "_bc.bmp" ) == 0 )
		result = true;

	return result;
}

void RenTextureSetImpl::load( const SysPathName& directory, BaseProgressReporter* pReporter )
{
	PRE( not isLoaded_ );

	TEXSET_STREAM("Preloading textures from directory " << directory << std::endl);
	TEXSET_INDENT(2);
	TEXSET_STREAM(RenSurfaceManager::instance());

	SysFileEnumerator fileFinder( directory, "*.bmp" );
	fileFinder.examineSubdirectories( true );
	fileFinder.find();
	const ctl_vector< SysFileData >& files = fileFinder.files();

	TEXSET_STREAM("Found " << files.size() << " .bmp files" << std::endl);
	textures_.reserve( files.size() );

	size_t filesRead = 0;
	size_t numFiles = files.size();
	size_t reportWhenFilesRead = 1;

	for( ctl_vector< SysFileData >::const_iterator it = files.begin(); it!=files.end(); ++it )
	{
		if ( pReporter )
		{
			++filesRead;
			if ( filesRead == reportWhenFilesRead )
			{
				size_t inc = pReporter->report( filesRead,numFiles );
				if ( inc == 0 )
					inc = 1;
				reportWhenFilesRead += inc;
			}
		}

		const SysPathName& fileName = (*it).pathName();
		// Check if the file has a _ba.bmp or _bc.bmp termination,
		// in which only load if
		// we are considering a color or a transparancy bitmap, do not load
		// RenSurfBody::read will sort out which bitmap effectively needs to be loaded
		if( not isAlphaMap( fileName ) and not isColourMap( fileName ) )
		{

			TEXSET_STREAM(" (" << textures_.size() << ") preloading texture "<< fileName.pathname() << std::endl);
			// load a texture and save the texture handle in textures_
			// Note: This call does not make use of the directory search
			// mechanism since (*i) refers to an absolute pathname (see
			// RenISurfaceManagerImpl::createSurfOrTex)
			RenTexture texture = RenSurfaceManager::instance(). createTexture( fileName.pathname() );
			textures_.push_back( texture );
		}
	}

	TEXSET_STREAM("Loaded " << textures_.size() << " bitmaps files" << std::endl);
	isLoaded_=true;

	TEXSET_STREAM(RenSurfaceManager::instance());
	TEXSET_INDENT(-2);
	TEXSET_STREAM("Done preloading textures from directory " << directory << std::endl);
}

void RenTextureSetImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


ostream& operator <<( ostream& o, const RenTextureSetImpl& t )
{

    o << "RenTextureSetImpl " << (void*)&t << " start" << std::endl;
    o << "RenTextureSetImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TEXSETI.CPP **************************************************/
