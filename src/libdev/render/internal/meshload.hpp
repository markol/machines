/*
 * M E S H L O A D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MESHLOAD_HPP
#define _RENDER_MESHLOAD_HPP

#include "base/base.hpp"
#include "ctl/map.hpp"
#include "ctl/pvector.hpp"
#include "system/pathname.hpp"

class SysPathName;
class RenHierarchyBuilder;
class RenTexture;

namespace XFile
{
    class Scene;
    class Mesh;
    class Node;
};

// A class which uses Direct3D RM to load meshes.  Internally, it reads and
// stores the contents of a whole .X file even if asked for only a single
// mesh.
class RenID3DMeshLoader
{
public:
    //  Singleton class
    static RenID3DMeshLoader& instance();
    virtual ~RenID3DMeshLoader();

    typedef struct MeshData { XFile::Mesh* mesh; XFile::Scene* scene; } MeshData;

	// Load a named mesh from a file.
	MeshData load(const SysPathName& pathName, const std::string & meshName);

    //  Load all meshes from the given file and use the hierarchy
    //  builder to construct the hierarchy given in the file.
    void load( const SysPathName& pathName, RenHierarchyBuilder* pBuilder );

	// When the application has finished reading meshes, this method ought
	// to be called so that the factory can delete all its mesh builders.
	void deleteAll();

    void CLASS_INVARIANT;

    typedef ctl_map<std::string , MeshData, std::less<std::string > > MeshMap;
    typedef ctl_map<std::string , MeshMap*, std::less<std::string > > FileMap;

private:
	RenID3DMeshLoader();		// Singleton

	FileMap files_;

	// During loading the callback functions need to know which file was
	// loaded when DirectX invoked the callbacks.  This member must be set to
	// point to the MeshMap corresponding to the file which is being loaded.
	MeshMap* meshesBeingLoaded_;
    SysPathName fileBeingLoaded_;
    XFile::Scene* sceneBeingLoaded_;

    RenHierarchyBuilder*    pHierarchyBuilder_;
    bool                    loadMeshes_;

	MeshData searchForMesh(const SysPathName&, const std::string &, const MeshMap*);
	void addMesh(XFile::Mesh*);
    void traverseFrame(XFile::Node*);

	void objectLoaded(XFile::Scene*);

    MeshMap* load( const SysPathName& pathName );

    // Operations deliberately revoked
    RenID3DMeshLoader( const RenID3DMeshLoader& );
    RenID3DMeshLoader& operator =( const RenID3DMeshLoader& );
    bool operator ==( const RenID3DMeshLoader& );

    typedef ctl_pvector< RenTexture >  Textures;

    Textures    textures_;
};


#endif

/* End MESHLOAD.HPP *************************************************/
