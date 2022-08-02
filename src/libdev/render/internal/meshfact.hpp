/*
 * M E S H F A C T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MESHFACT_HPP
#define _RENDER_MESHFACT_HPP

#include "base/base.hpp"
#include <string>
#include "system/pathname.hpp"
#include "render/internal/cpfactry.hpp"
#include "render/internal/meshid.hpp"
#include "render/render.hpp"
#include "ctl/countptr.hpp"

class RenMesh;

// Both this and RenID3DMeshLoader are factories of a sort; why are there two?
// RenID3DMeshLoader deals with a Direct3D specific problem -- it ensures that
// you don't have to repeatedly load a .x file if you wish to load more than
// one mesh from the same file.  This is a factory in the UtlFactory sense --
// it ensures that unrelated clients who ask for the same mesh receive a
// const shared instance, rather than a copy.  This class implements
// RenMesh::createShared.
class RenIMeshFactory : public UtlCPFactory<RenIMeshID, Ren::ConstMeshPtr>
{
public:
    //  Singleton class
    static RenIMeshFactory& instance();
    ~RenIMeshFactory();

	// These methods don't actually destory the mesh instances, they just
	// release the counted pointers held by the factory.
	void removeFromFactory(Ren::ConstMeshPtr);
	void removeAllFromFactory();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenIMeshFactory& t );

private:
	virtual Ren::ConstMeshPtr doCreatePart( const RenIMeshID& id );

    RenIMeshFactory();

    // Operations deliberately revoked
    RenIMeshFactory( const RenIMeshFactory& );
    RenIMeshFactory& operator =( const RenIMeshFactory& );
    bool operator ==( const RenIMeshFactory& );
};

#endif

/* End MESHFACT.HPP *************************************************/
