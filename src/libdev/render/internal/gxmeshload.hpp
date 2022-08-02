/*
 * G X M E S H L O A D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _GXMESHLOAD_HPP
#define _GXMESHLOAD_HPP

#include "base/base.hpp"
//#include "stdlib/string .hpp"
#include <string>
#include "ctl/map.hpp"
#include "ctl/pvector.hpp"
#include "system/pathname.hpp"

#include "gxin/gx.hpp"
#include "gxin/gxerror.hpp"
#include "gxin/gxfile.hpp"
//#include "gxin/gxuserdata.hpp"
#include "gxin/gxmesh.hpp"
#include "gxin/gxhier.hpp"

class SysPathName;
class RenHierarchyBuilder;

class RenIGXMeshLoader {

public:
   static RenIGXMeshLoader &instance();
   virtual ~RenIGXMeshLoader();

   // returns pointer to GAMUT-X mesh having name
   GXMesh *load(const SysPathName &pathname, const std::string & name);
   // POST(result==NULL||result->name().str()==name);

    //  Load all meshes from the given file and use the hierarchy
    //  builder to construct the hierarchy given in the file.
    void load( const SysPathName& pathName, RenHierarchyBuilder* pBuilder );

	// When the application has finished reading meshes, this method ought
	// to be called so that the factory can delete all its mesh builders.
	void deleteAll();

    void CLASS_INVARIANT;

private:
	RenIGXMeshLoader();		// Singleton
   typedef ctl_vector<GXMesh*> GXMeshes;
   typedef ctl_map<std::string , GXMeshes*, std::less<std::string > > GXMeshesMap;
   typedef ctl_map<std::string , GXHier*, std::less<std::string > > 	GXHierMap;

   // Load all the meshes of a file in meshesMap_
   // Creates a set of (GX) meshes, loads it,
   // and insert it in meshesMap_
   // Create a (GX) hierarchy structure loads it
   // and insert it in hierarchyMap_
   void load( const SysPathName& pathName );
   // PRE(not meshesBeingLoaded_);

   // load the @Objects field of a file and put the meshes
   // at the top of files_
   GXError loadObjects(GXFile&);
   // PRE(gxfile.isOpen());
   // PRE(meshesBeingLoaded_);

   // convert the hierarchy from Gamut-X to render
   void convertHierarchy(const GXHier&, RenHierarchyBuilder*);
   void convertNode(const GXHier&, RenHierarchyBuilder*);

   // search for the mesh having the seeked named in the set of meshes
   GXMesh* searchForMesh( const std::string &, const GXMeshes&);

   GXMeshesMap meshesMap_;
   GXHierMap   hierarchyMap_;

   RenHierarchyBuilder*    pHierarchyBuilder_;

   GXMeshes* meshesBeingLoaded_;
   // temporary variable used by a set of consecutive routines
   // Typically allocated at beginning of load( const SysPathName& pathName )
   // used in GXError loadObjects(const GXFile& gxfile)
   // pointer released at end of load( const SysPathName& pathName )

    SysPathName fileBeingLoaded_;

};
//static RenIGXMeshLoader::GXMeshesMap::const_iterator linearFind
//(const RenIGXMeshLoader::GXMeshesMap& , const std::string & );

#endif
