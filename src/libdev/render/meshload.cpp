/*
 * M E S H L O A D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "base/diag.hpp"
#include <ctype.h>
#include "render/internal/meshload.hpp"
#include "render/internal/meshid.hpp"
#include "system/pathname.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "render/meshinst.hpp"
#include "render/hierbuil.hpp"
#include "xin/XFileParser.hpp"
#include "xin/XFileHelper.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/coordsys.hpp"

#include "device/time.hpp"

// Debugging statements in this file.
//#define DEBUGFACT(x)	RENDER_STREAM(x)
#define DEBUGFACT(x)	;

//////////////////////////////////////////////////////////////////////////////////////////

//I can't believe I had to write this!
static void lowerString( std::string* pString )
{
	if (!pString || pString->length() == 0)
		return;

    char ch;
    for( size_t i = 0; (ch = (*pString)[i]) != '\0'; ++i )
        (*pString)[i] = (char)tolower( ch );
}
//////////////////////////////////////////////////////////////////////////////////////////

// static
RenID3DMeshLoader& RenID3DMeshLoader::instance()
{
    static RenID3DMeshLoader instance_;
    return instance_;
}

RenID3DMeshLoader::RenID3DMeshLoader():
	meshesBeingLoaded_(NULL),
	pHierarchyBuilder_(NULL),
	sceneBeingLoaded_(NULL),
	loadMeshes_(false)
{
	PRE(Ren::initialised());
    textures_.reserve( 64 );

    TEST_INVARIANT;
}

RenID3DMeshLoader::~RenID3DMeshLoader()
{
    TEST_INVARIANT;

	deleteAll();
}

void RenID3DMeshLoader::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

static RenID3DMeshLoader::FileMap::const_iterator linearFind
(
	const RenID3DMeshLoader::FileMap& map,
	const std::string& key
)
{
	DEBUGFACT(Diag::instance().renderStream() << "searching for " << key << std::endl);
	DEBUGFACT(Diag::instance().renderStream() << "map size = " << map.size()  << std::endl);

	int i = 0;
	RenID3DMeshLoader::FileMap::const_iterator it = map.begin();
	while (it != map.end())
	{
		DEBUGFACT(Diag::instance().renderStream() << i << "th key = " << (*it).first << std::endl);

		if ((*it).first == key)
		{
			DEBUGFACT(Diag::instance().renderStream() << "Found." << std::endl);
			return it;
		}
		++i;
		++it;
	}

	DEBUGFACT(Diag::instance().renderStream() << "Not found." << std::endl);
	return map.end();
}

RenID3DMeshLoader::MeshData RenID3DMeshLoader::load
(
	const SysPathName& pathName,
	const std::string& meshName
)
{
    PRE( pHierarchyBuilder_ == NULL );

    MeshData  result;


    MeshMap* meshes;

	FileMap::const_iterator fileIt = linearFind(files_, pathName.pathname());

	if (fileIt != files_.end())
	{
		// The file has been loaded already.  Search for the required mesh.
		meshes = (*fileIt).second;

	}
	else
	{
        loadMeshes_ = true;
        meshes = load( pathName );
    }

    result = searchForMesh( pathName, meshName, meshes );

    return result;
}

void RenID3DMeshLoader::load( const SysPathName& pathName, RenHierarchyBuilder* pBuilder )
{
    PRE_INFO( pathName );

    PRE( pBuilder != NULL );
    PRE( pHierarchyBuilder_ == NULL );

    pHierarchyBuilder_ = pBuilder;

    //  Only load the meshes if this file hasn't already been loaded
	FileMap::const_iterator fileIt = linearFind(files_, pathName.pathname());
    if (fileIt == files_.end())
    {
        loadMeshes_ = true;
        load( pathName );
    }
    else
    {
        loadMeshes_ = false;
        //  TBD: This _DELETE is really a bit of a hack. We should define who owns
        //  the pointer passed back from load and therefore who is responsible for
        //  deleting it. Sometimes the pointer gets put into files_, sometimes it
        //  doesn't in which case we need to delete it separately as is happening here.
        MeshMap* pMeshMap = load( pathName );
        _DELETE( pMeshMap );
    }
    pHierarchyBuilder_ = NULL;

    POST( pHierarchyBuilder_ == NULL );
}

//  Load in all of the meshes from a file. Return the map of those meshes
//  which have been loaded. This does not check whether the file has already
//  been loaded - it will load it anyway.
RenID3DMeshLoader::MeshMap* RenID3DMeshLoader::load
(
	const SysPathName& pathName
)
{
	// Create a new set of meshes to be populated when this file loads.
	meshesBeingLoaded_ = _NEW(MeshMap);
    fileBeingLoaded_ = pathName;

    //  Use XParser to load the file.
    std::ifstream xFile(pathName.pathname());
    std::vector<char> fContent;
    xFile.seekg(0, std::ios::end);
    fContent.reserve(xFile.tellg());
    xFile.seekg(0, std::ios::beg);

    fContent.assign((std::istreambuf_iterator<char>(xFile)),
                std::istreambuf_iterator<char>());
    try
    {
        XFileParser xParser(fContent);
        sceneBeingLoaded_ = xParser.GetImportedData();
        objectLoaded(sceneBeingLoaded_);
        //  Only register this file if we were loading the meshes for it. If we
        //  weren't loading the meshes it's because we have already loaded the
        //  file and we don't want to register them twice.
        if( loadMeshes_ )
        {
        	FileMap::value_type newValue(pathName.pathname(), meshesBeingLoaded_);
	    	files_.insert(newValue);
        }
    }
    catch (XFile::DeadlyImportError err)
    {
        std::cerr << "Could not parse the .x file: " << err.what() << std::endl;
    }

	/*// A cast is required because the declaration of _cdecl in our environment
	// does not appear to be correct (or something like that).
	D3DRMLOADTEXTURECALLBACK texCB = (D3DRMLOADTEXTURECALLBACK) loadTextureCB;
	D3DRMLOADCALLBACK CB = (D3DRMLOADCALLBACK) loadCB;

	// Load all stand alone frames and meshes in the given file.
	char* fileStr = (char*) pathName.pathname().c_str();
    ASSERT_INFO( pathName.pathname().c_str() );
	if (TRYRENDX(d3drm->Load(fileStr, NULL, guids, 4, D3DRMLOAD_FROMFILE,
			CB, this, texCB, this, NULL)))
    {
        //  Only register this file if we were loading the meshes for it. If we
        //  weren't loading the meshes it's because we have already loaded the
        //  file and we don't want to register them twice.
        if( loadMeshes_ )
        {
        	FileMap::value_type newValue(pathName.pathname(), meshesBeingLoaded_);
	    	files_.insert(newValue);
        }
    }*/


    return meshesBeingLoaded_;
}

void RenID3DMeshLoader::deleteAll()
{
	RenID3DMeshLoader::FileMap::iterator it = files_.begin();
	while (it != files_.end())
	{
		DEBUGFACT(Diag::instance().renderStream() << "deleting meshes in file " << (*it).first << std::endl);

		// The contents of the MeshMap are COMPTR objects.  These don't
		// need to be explicitly deleted -- deleting the map should do.
		MeshMap* meshes = (*it).second;
		_DELETE(meshes->begin()->second.scene);
		meshes->erase(meshes->begin(), meshes->end());		// ???
		_DELETE(meshes);

		RenID3DMeshLoader::FileMap::iterator tmpIt = it;
		++it;
		files_.erase(tmpIt);
	}

	DEBUGFACT(Diag::instance().renderStream() << "map size is now " << files_.size() << std::endl);

    //TBD: This is a crufty fix. destroyRMTextureCB ought to delete the texture,
    // but somehow the callback isn't being called.  Maybe there's a leak of an
    // RM object which references the texture.  Maybe there's a bug in RM.
    for( Textures::iterator i = textures_.begin(); i != textures_.end(); ++i )
        _DELETE( *i );
    textures_.erase( textures_.begin(), textures_.end() );
}

RenID3DMeshLoader::MeshData RenID3DMeshLoader::searchForMesh
(
	const SysPathName& pathName,
	const std::string& meshName,
	const MeshMap* meshes
)
{
    //Convert the meshname to lower case
    string lcMeshName( meshName );
    lowerString( &lcMeshName );

	MeshMap::const_iterator meshIt = meshes->find(lcMeshName);
	if (meshIt != meshes->end())
	{
		return (*meshIt).second;
	}
	else
	{
		// TBD: better error handling when a mesh isn't found in a file??
		std::cout << "Could not find mesh " << lcMeshName << " in file " << pathName << std::endl;
		MeshData result;
		result.mesh = NULL; result.scene = NULL;
		return result;
	}
}

void RenID3DMeshLoader::traverseFrame(XFile::Node* frame)
{
    ASSERT(frame->mName != "$dummy_root", "Up to one root frame is allowed!");
    //  Get the transform associated with this frame
	float (*rmMatrix)[4][4] = &frame->mTrafoMatrix.data;

    MexVec3 xBasis( (*rmMatrix)[ 0 ][ 0 ], (*rmMatrix)[ 0 ][ 1 ], (*rmMatrix)[ 0 ][ 2 ] );
    MexVec3 yBasis( (*rmMatrix)[ 1 ][ 0 ], (*rmMatrix)[ 1 ][ 1 ], (*rmMatrix)[ 1 ][ 2 ] );
    MexVec3 zBasis( (*rmMatrix)[ 2 ][ 0 ], (*rmMatrix)[ 2 ][ 1 ], (*rmMatrix)[ 2 ][ 2 ] );

	// Note y and z may be swapped during reading.  The conv3ds program appears
	// to swap the values in an attempt to switch from 3DS's right-handed
	// system to DirectX's left-handed.  If we require a right-handed system,
	// we need to undo this.
	const int yIdx = (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)? 2: 1;
	const int zIdx = (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)? 1: 2;
    MexPoint3d position((*rmMatrix)[3][0], (*rmMatrix)[3][yIdx], (*rmMatrix)[3][zIdx]);

    MATHEX_SCALAR   frameScale = xBasis.modulus();

    if( fabs( frameScale - 1.0 ) > 0.00001 )
    {
        //  The matrix has been scaled. Scale it down.

        xBasis /= frameScale;
        yBasis /= frameScale;
        zBasis /= frameScale;
    }

    MexTransform3d    frameTransform( xBasis, yBasis, zBasis, position );

	// Swap the y and z components of rotation.  This also seems to require
	// the sense of the rotation to be changed.  Why?
	if (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)
	{
		MexQuaternion q = frameTransform.rotationAsQuaternion();
		const MexQuaternion newQuat(q.vector().x(), q.vector().z(), q.vector().y(), -q.scalar());
		frameTransform.rotation(newQuat);
	}

    //  Get the mesh ( if any ) associated with this frame

    size_t  nVisuals = 0;
    string  meshName;
/*    COMPTR(IDirect3DRMMeshBuilder) meshBuilder;

	COMPTR(IDirect3DRMVisualArray) visuals;
	if (SUCCEEDED(frame->GetVisuals(visuals.address())))
	{
		for (int i=0; i!=visuals->GetSize(); ++i)
		{
			COMPTR(IDirect3DRMVisual) visual;
			if (SUCCEEDED(visuals->GetElement(i, visual.address())))
			{
				meshBuilder = visual;	// implict QueryInterface

				if (meshBuilder)
                {
                	DWORD bufSize = 1000;
                   	char name[1000];
                	TRYRENDX(meshBuilder->GetName(&bufSize, name));

                    meshName = name;

					addMesh(meshBuilder);
                    ++nVisuals;
                }
			}
		}
	}*/
	for (uint i = 0; i != frame->mMeshes.size(); ++i)
    {
        XFile::Mesh* mesh = frame->mMeshes[i];
        meshName = mesh->mName;
        addMesh(mesh);
        ++nVisuals;
    }

    ASSERT_INFO( fileBeingLoaded_ );
    ASSERT_INFO( meshName );

	RenIMeshID meshId( fileBeingLoaded_, meshName, frameScale );

    ASSERT_INFO( nVisuals );
    ASSERT( nVisuals <= 1, "Maximum of one mesh per frame" );

	if (frame->mChildren.size())
	{
        size_t nChildren = frame->mChildren.size();

        if( pHierarchyBuilder_ )
        {
            //  Pass on the information about this frame to the hierarchy builder

            if( nVisuals == 0 )
                pHierarchyBuilder_->addFrame( frameTransform, nChildren );
            else
                pHierarchyBuilder_->addFrame( frameTransform, nChildren, meshId );
        }

		for (uint i=0; i != nChildren; ++i)
		{
			traverseFrame(frame->mChildren[i]);

		}
	}
}

void RenID3DMeshLoader::objectLoaded(XFile::Scene* object)
{

    PRE(meshesBeingLoaded_);	// repositry for anything we find -- must be set


	if (object->mRootNode)
	{
		traverseFrame(object->mRootNode);

//		return;
	}

	// Mesh builder
	{

		if (object->mGlobalMeshes.size())
        {

            string meshName = object->mGlobalMeshes[0]->mName;

            ASSERT_INFO( fileBeingLoaded_ );
            ASSERT_INFO( meshName );

        	RenIMeshID meshId( fileBeingLoaded_, meshName, 1 );

            size_t  nChildren = 0;

			addMesh(object->mGlobalMeshes[0]);

            if( pHierarchyBuilder_ )
                pHierarchyBuilder_->addFrame( MexTransform3d(), nChildren, meshId );
        }
		return;
	}

	return;
}

void RenID3DMeshLoader::addMesh(XFile::Mesh* mesh)
{
	ASSERT(meshesBeingLoaded_, "");	// repositry for anything we find -- must be set

    //  I know it appears that if we are not loading meshes we don't have
    //  to do this get / set name stuff but if we don't we run into problems
    //  when trying to load the same file several times. After the third time
    //  we stop getting the names of the meshes. Doing this get / set makes
    //  the getName call work properly. I'm not sure why this should be the
    //  case although it's possible that Direct3D is keeping some sort of
    //  internal map of meshes loaded along with their names and
    //  something is going wrong there.

    std::string name = mesh->mName;
    string fullName = fileBeingLoaded_.pathname() + name;
    mesh->mName = fullName;

    if( loadMeshes_ )
    {
        //Convert the meshname to lower case
        std::string lcMeshName( name );
        lowerString( &lcMeshName );

    	// At present, all meshes must have valid names.  This obviates .x files
    	// created with the -m option of conv3ds.  It is conceivable that the maps
    	// could be made to work with anonymous meshes, if necessary.
    	ASSERT(name.size() > 0, runtime_error("All meshes must have names."));

    	DEBUGFACT(Diag::instance().renderStream() << "Loaded mesh builder " << lcMeshName);
    	DEBUGFACT(Diag::instance().renderStream() << " with " << mesh->mNormFaces.size() << " polys.\n");

    	MeshData meshData;
    	meshData.mesh = mesh;
    	meshData.scene = sceneBeingLoaded_;
    	MeshMap::value_type newValue(lcMeshName, meshData);
    	meshesBeingLoaded_->insert(newValue);
    }
}

/* End MESHLOAD.CPP *************************************************/
