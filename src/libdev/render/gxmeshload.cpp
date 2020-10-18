/*
 * G X M E S H L O A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include <iostream>
#include <ctype.h>
#include "render/internal/gxmeshload.hpp"
#include "render/internal/meshid.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "render/hierbuil.hpp"

#define DEBUGFACT(x)


// declaration of local functions
#ifndef NDEBUG
void convertedHierarchyDB( const GXHier&, RenHierarchyBuilder *);
void sourceHierarchyDB(const GXHier& );
void destHierarchyDB(RenHierarchyBuilder *);
#endif

// static
RenIGXMeshLoader& RenIGXMeshLoader::instance()
{
    static RenIGXMeshLoader instance_;
    return instance_;
}

RenIGXMeshLoader::RenIGXMeshLoader():
	meshesBeingLoaded_(NULL)
{

    TEST_INVARIANT;
}

RenIGXMeshLoader::~RenIGXMeshLoader()
{
    TEST_INVARIANT;

	deleteAll();
}

void RenIGXMeshLoader::deleteAll()
{

	TEST_INVARIANT;

	for (GXMeshesMap::iterator meshesIt=meshesMap_.begin();
	 	 meshesIt!=meshesMap_.end(); ++meshesIt)
	{
      GXMeshes *meshes=(*meshesIt).second;
	  if (meshes!=NULL)
	  {
		for (int meshCount=0;meshCount<meshes->size(); meshCount++)
		{
		  GXMesh *mesh=(*meshes)[meshCount];
		  if (mesh!=NULL)
		  {
		   _DELETE(mesh);
		  }
		}
		_DELETE(meshes);
	  }
	}

	for (GXHierMap::iterator hierIt=hierarchyMap_.begin();
	 	 hierIt!=hierarchyMap_.end(); ++hierIt)
	{
      GXHier *hier=(*hierIt).second;
	  //if (hierIt!=NULL)
	  if (hierIt != hierarchyMap_.end() )
	  {
	    _DELETE(hier);
	  }
	}
}

void RenIGXMeshLoader::CLASS_INVARIANT
{
	INVARIANT(meshesMap_.size()==hierarchyMap_.size());
    INVARIANT( this != NULL );
}

//static RenIGXMeshLoader::GXMeshesMap::const_iterator linearFind
//(
//	const RenIGXMeshLoader::GXMeshesMap& map,
//	const string& key
//)
//{
//	DEBUGFACT(Diag::instance().renderStream() << "searching for " << key << std::endl);
//	DEBUGFACT(Diag::instance().renderStream() << "map size = " << map.size()  << std::endl);
//
//	int i = 0;
//	RenIGXMeshLoader::GXMeshesMap::const_iterator it = map.begin();
//	while (it != map.end())
//	{
//		DEBUGFACT(Diag::instance().renderStream() << i << "th key = " << (*it).first << std::endl);
//
//		if ((*it).first == key)
//		{
//			DEBUGFACT(Diag::instance().renderStream() << "Found." << std::endl);
//			return it;
//		}
//		++i;
//		++it;
//	}

//	DEBUGFACT(Diag::instance().renderStream() << "Not found." << std::endl);
//	return map.end();
//}

GXMesh* RenIGXMeshLoader::load
(
	const SysPathName& pathName,
	const std::string  &combinedName
)
{

	GXMeshes *meshes;
	GXMesh *result;

	PRE_INFO(pathName);
	PRE_INFO(combinedName);

	DEBUGFACT("RenIGXMeshLoader::load(" << pathName << "," << combinedName << ")" << std::endl);

	fileBeingLoaded_=pathName;

	// returns a pointer on the mesh once it has been
	// found and loaded (if needed) in the meshesMap_ map

	GXMeshesMap::const_iterator meshesMapIt;
	meshesMapIt = meshesMap_.find(pathName.pathname());

	if (meshesMapIt != meshesMap_.end())
	{
	   	meshes = (*meshesMapIt).second;
	}
	else
	{
      load(pathName);
      meshesMapIt = meshesMap_.find(pathName.pathname());
	  ASSERT(meshesMapIt!=meshesMap_.end(),"Meshes not found in meshesMap_");
      meshes = (*meshesMapIt).second;
	}

	// the mesh set has been pointed out, look for the mesh in the mesh set
//	string instanceName, meshName;
//	extractFromCombinedName(combinedName, instanceName, meshName);
	std::string  meshName=combinedName;

	if (meshName.length()!=0)
	{
	  result = searchForMesh(meshName, *meshes);
	}
	else
	{
	  // The combined name only holds a instance name i.e, no mesh is attached to
	  // node
	  DEBUGFACT("The combined name holds no mesh name (meshName.length()==0)" << std::endl);
	  result=NULL;
	}

	if (result==NULL)
	{
	 DEBUGFACT("Warning : no mesh has been loaded by RenIGXMeshLoader::load(" << pathName << "," << combinedName << ")" << std::endl);
	}

	POST(result==NULL||result->name().str()==meshName);

	return result;

}

void RenIGXMeshLoader::load(const SysPathName& pathName, RenHierarchyBuilder* pBuilder)
{
  GXHier *hierarchy;

  PRE_INFO( pathName );
  PRE( pBuilder != NULL );

  DEBUGFACT("RenIGXMeshLoader::load(" << pathName.pathname() << "," << "RenHierarchyBuilder*)" << std::endl);
  fileBeingLoaded_=pathName;

  GXHierMap::const_iterator hierMapIt;

  hierMapIt = hierarchyMap_.find(pathName.pathname());
  ASSERT_DATA(GXMeshesMap::const_iterator meshesMapIt = meshesMap_.find(pathName.pathname()));

  if (hierMapIt!=hierarchyMap_.end())
  {
	ASSERT(meshesMapIt!=meshesMap_.end(), "Discrepancy between loaded meshes and hierarchies");
    ASSERT((*meshesMapIt).first==(*hierMapIt).first, "Discrepancy between loaded meshes and hierarchies");
	hierarchy=(*hierMapIt).second;
  }
  else
  {
    ASSERT(meshesMapIt==meshesMap_.end(), "Discrepancy between loaded meshes and hierarchies");
	load( pathName );
    hierMapIt = hierarchyMap_.find(pathName.pathname());
    ASSERT(hierMapIt!=hierarchyMap_.end(), "Hierarchy not found in hierMap_");
    hierarchy = (*hierMapIt).second;

  }

  convertHierarchy(*hierarchy, pBuilder);

}

void RenIGXMeshLoader::load
(
  const SysPathName &pathname
)
{
	// Data for handling Gamut-X file reading
	GXLabel gxlabel;
    GXIdPos gxid;
	GXFile gxfile;
	GXError gxerror;
//	GXUserData gxud;
	GXTextures gxtextures;
	GXHier *gxhierarchy=_NEW(GXHier);

	PRE(not meshesBeingLoaded_);

	// Create a new set of meshes in which the meshes
	// read from the current file will be loaded
	meshesBeingLoaded_ = _NEW(GXMeshes);
    meshesBeingLoaded_->reserve( 32 );

	gxerror=gxfile.open(pathname.pathname());
	if (gxerror!=NO_ERROR_AGT) {
		gxfile.reportAGTError(FILE_OPEN_AGT,(void*)pathname.pathname().c_str());
	}
	else
	{
	   // Move the file cursor after the header section
	   gxerror = gxfile.skipHeader();
	   if (gxerror != NO_ERROR_AGT)
	   {
	     gxfile.reportAGTError(HEADER_NAME_AGT,NULL);
	   }
	   else
       {
		 do
		 {
		   gxerror=gxfile.readLabel(gxlabel,true);
		   if (gxerror==NO_ERROR_AGT)
		   {
		   	 switch (gxlabel.type())
		   	 {
		   	   case OBJECTS:
				// read the objects of the files, the
				// meshes are inserted in meshesBeingLoaded_
		   	   	 gxerror=loadObjects(gxfile);
		   	   	 break;
			   case ANIMATION:
				 gxerror=gxfile.skipSection();
				 break;
			   case TEXTURES:
			     gxerror=gxfile.readTextures(gxtextures);
				 break;
			   case HIERARCHY:
			     ASSERT(gxhierarchy,"");
				 gxerror=gxfile.readHierarchy(*gxhierarchy);
				 break;
			   default:
				 gxerror=gxfile.skipSection();
				 break;
			 } // switch
		   } // if (gxerror==NO_ERROR_AGT)
		 }while(gxerror==NO_ERROR_AGT);
	   } // if (gxerror == NO_ERROR_AGT)

    } // if (gxerror==NO_ERROR_AGT)

	gxfile.close();

    // Set the meshes textures and names.
	ASSERT(gxhierarchy,"");

    for (int i=0;i<meshesBeingLoaded_->size(); ++i)
    {
      (*meshesBeingLoaded_)[i]->setTextures(gxtextures);
	  (*meshesBeingLoaded_)[i]->setName(*gxhierarchy);
	  if ((*meshesBeingLoaded_)[i]->hasName()==false)
	  {
	    DEBUGFACT("Could not set name of mesh having id "<<   (*meshesBeingLoaded_)[i]->id() <<std::endl);
	  }
	  else
	  {
	    DEBUGFACT("Set name of mesh having id "<< (*meshesBeingLoaded_)[i]->id() << " to " << (*meshesBeingLoaded_)[i]->name() <<std::endl);
	  }
    }



	// register loaded meshes
	GXMeshesMap::value_type meshesPair(pathname.pathname(), meshesBeingLoaded_);
	meshesMap_.insert(meshesPair);

	// register loaded hierarchy
	GXHierMap::value_type hierPair(pathname.pathname(), gxhierarchy);
	hierarchyMap_.insert(hierPair);

	// release the pointer meshesBeingLoaded so that it can be used
	// by future call to this method
    meshesBeingLoaded_=NULL;

}

// Read the @Objects field from the file and put the meshes found
// in on the meshesBeingBuild_ stack
GXError RenIGXMeshLoader::loadObjects(GXFile& gxfile)
{
  GXError gxerror;
  GXIdPos gxid;
  GXLabel gxmeshtype;

  // Meshes are created when read from the @Objects
  // section and added to the meshesBeingBuild_, this is why
  // gxmesh is many times allocated and never deleted
  GXMesh *gxmesh;

  PRE(gxfile.isOpen());
  PRE(meshesBeingLoaded_);

  gxerror=gxfile.readGXIdPos(gxid,true,true,NULL);
  if (gxerror==NO_ERROR_AGT)
  {
    gxerror=gxfile.findTag(SECTION_START,true);
	if (gxerror==NO_ERROR_AGT)
	{
	  do
	  {
	    gxerror=gxfile.readLabel(gxmeshtype,true);
		if (gxerror==NO_ERROR_AGT)
		{
		  switch (gxmeshtype.type())
		  // Switch does handle meshes only
		  {
		    case TRIMESH:
			  gxmesh=_NEW(GXMesh);
			  gxerror=gxfile.readMesh(*gxmesh, gxmeshtype);
			  if (gxerror==NO_ERROR_AGT)
			  {
				meshesBeingLoaded_->push_back(gxmesh);
			  }
			  break;
			case QUADMESH:
			  gxmesh=_NEW(GXMesh);
			  gxerror=gxfile.readMesh(*gxmesh,gxmeshtype);
			  if (gxerror==NO_ERROR_AGT)
			  {
				meshesBeingLoaded_->push_back(gxmesh);
			  }
			  break;
			default:
			  gxerror=gxfile.skipSection();
			  break;
		  }	// switch(gxmeshtype())
		} // if (gxerror==NO_ERROR_AGT); gxerror=gxmeshtype.read(gxfile,true);
	  }while (gxerror==NO_ERROR_AGT);

	  if ((gxerror==NO_ERROR_AGT) || (gxerror==TOKEN_AGT))
		gxerror=gxfile.findTag(SECTION_END,true);
	} // if (gxerror==NO_ERROR_AGT); gxerror=gxfile.findTag(SECTION_START,true);
  }	// if (gxerror==NO_ERROR_AGT); gxerror=gxid.read(gxfile);


  return gxerror;

}

GXMesh* RenIGXMeshLoader::searchForMesh
(
	const std::string & searchedName,
	const GXMeshes& meshes
)
{
	std::string currentName;
	bool meshNotFound=true;
    GXMesh *result;

	DEBUGFACT("RenIGXMeshLoader::searchForMesh("  << searchedName << ")" << std::endl);

    int i=0;
	while(i<meshes.size() && meshNotFound)
	{
	  currentName=meshes[i]->name().str();
	  DEBUGFACT("Current mesh name " << currentName);
	  if (currentName==searchedName)
	  {
	    meshNotFound=false;
	  }
	  else
	  {
	    ++i;
	  }
	} // while(i<meshes.size() && meshNotFound)

	if (meshNotFound==true)
	{
	  DEBUGFACT("Could not find mesh with name " << searchedName  << std::endl);
	  result=NULL;
	}
	else
	{
	  DEBUGFACT("Mesh found and has id " << meshes[i]->id() << std::endl);
	  result=meshes[i];
	}
	return result;
}
#ifndef NDEBUG

void sourceHierarchyDB(const GXHier& source)
{
  DEBUGFACT("source.meshName() " << source.meshName() << std::endl);
  DEBUGFACT("source.nChildren() " << source.nChildren() << std::endl);
  for (int i=0; i<source.nChildren(); i++)
  {
    DEBUGFACT("source.child(" << i << ") {" << std::endl);
    sourceHierarchyDB(source.child(i));
	DEBUGFACT("}" << std::endl);
  }
}

void destHierarchyDB(RenHierarchyBuilder * dest)
{
  DEBUGFACT( *dest->pRoot() << std::endl);
}

void convertedHierarchyDB( const GXHier& source, RenHierarchyBuilder *dest)
{
  DEBUGFACT("Source hierarchy"<< std::endl);
  sourceHierarchyDB(source);
  DEBUGFACT("Dest hierarchy" << std::endl);
  destHierarchyDB(dest);
}

#endif

void RenIGXMeshLoader::convertHierarchy(const GXHier& source, RenHierarchyBuilder *dest)
{
  PRE(dest);

  // get and set the transform
  // Should we set the root node transform?
  GXMatrix3 gxTransform=source.transform();
  GXVect3 gxTranslate=source.translate();
  MexVec3 xBasis(gxTransform.xx(), gxTransform.xy(), gxTransform.xz());
  MexVec3 yBasis(gxTransform.yx(), gxTransform.yy(), gxTransform.yz());
  MexVec3 zBasis(gxTransform.zx(), gxTransform.zy(), gxTransform.zz());
  MexPoint3d position(gxTranslate.x(), gxTranslate.y(), gxTranslate.z());
  MexTransform3d frameTransform(xBasis, yBasis, zBasis, position);

  for (int i=0; i<source.nChildren(); ++i)
  {
    convertNode(source.child(i), dest);
  }

#ifndef NDEBUG
  DEBUGFACT("After conversion the source and dest hierarchies are " << std::endl);
  convertedHierarchyDB(source, dest);
#endif
}


void RenIGXMeshLoader::convertNode(const GXHier& source, RenHierarchyBuilder *dest)
{

  PRE(dest);

  // get and set the transform
  GXMatrix3 gxTransform=source.transform();
  GXVect3 gxTranslate=source.translate();
  // ?? Why do I have to flip yz
  MexVec3 xBasis(gxTransform.xx(), gxTransform.zx(), gxTransform.yx());
  MexVec3 yBasis(gxTransform.xz(), gxTransform.zz(), gxTransform.yz());
  MexVec3 zBasis(gxTransform.xy(), gxTransform.zy(), gxTransform.yy());
  MexPoint3d position(gxTranslate.x(), gxTranslate.z(), gxTranslate.y());

  xBasis.makeUnitVector();
  yBasis.makeUnitVector();
  zBasis.makeUnitVector();
  MexTransform3d frameTransform(xBasis, yBasis, zBasis, position);

  // get number of children
  ASSERT(source.allChildrenSupplied(), "Invalid hierarchy node");
  size_t nChildren=source.nChildren();

  // get and set meshId
  std::string  meshName=source.meshName().str();

  ASSERT(meshName.length()!=0, "meshName is not set");

  // TODO: find out what the frameScale factor should be
  // Since ther is no mesh name in GX, it is set to be the
  // instance name
  std::string  combinedName;

  if (source.hasObject())
  {
	// The source node has a mesh attached to it
	// set both instance and mesh name of the dest node
//    combinedName = meshName + separator() + meshName;
	combinedName=meshName;
  }
  else
  {  // There is no mesh attached to the source node
	// only set the instance name of the dest node
	combinedName=meshName;
  }

  RenIMeshID meshId(fileBeingLoaded_, combinedName, 1.0);
  dest->addFrame(frameTransform,  nChildren, meshId);

  for (int i=0; i<source.nChildren(); ++i)
  {
    convertNode(source.child(i), dest);
  }

}

//static void extractFromCombinedName(
//    const string& combinedName,
//    string& instanceName,
//    string& meshName)
//{
//
//   PRE(combinedName.length()!=0);
//
//   bool	separatorFound = false;
//   size_t  separatorPosition;
//
//	for( size_t i = 0; i < combinedName.length() and not separatorFound; ++i )
//	{
//		if( combinedName[ i ] == separator() )
//        {
//			separatorFound = true;
//            separatorPosition = i;
//        }
//	}
//
//	if( separatorFound )
//	{
//		instanceName = combinedName.substr( 0, separatorPosition );
//		meshName = combinedName.substr( separatorPosition + 1, combinedName.length() - separatorPosition );
//	}
//	else
//	{
//		instanceName = combinedName;
//	}
//}

// static
//static char separator( void )
//{
//    return 'X';
//}
