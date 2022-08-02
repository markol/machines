/*
 * G E N E R I C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _GENERIC_HPP
#define _GENERIC_HPP

#include "stdlib/string.hpp"
#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/entity.hpp"

class SysPathname;

// A class which represents simple, generic renderable objects.
// They are simple in the sense that they are not composites and
// they have a simple list of meshes for alternative LODs.
class W4dGeneric : public W4dEntity
{
public:
    W4dGeneric(W4dEntity* pParent, const W4dTransform3d& localXform, Solidity solidity = SOLID);
    W4dGeneric(const W4dGeneric& copyMe, W4dEntity* pParent, const W4dTransform3d& localXform);

    //Makes a generic copy from the W4dEntity level parts of copyMe - ie slices
    W4dGeneric(const W4dEntity& copyMe, W4dEntity* pParent, const W4dTransform3d& localXform);


    virtual ~W4dGeneric();

	// You can scale a single mesh when you load it.  You can't at present
	// scale the meshes in an LOD file because that would probably invalidate
	// the transition ranges.
    bool loadSingleMesh(const SysPathName& filename, const string& meshName, double scale = 1.0);
    bool loadLODFile   (const SysPathName& filename);

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dGeneric );
    PER_FRIEND_READ_WRITE( W4dGeneric );

private:
    // Operations deliberately revoked
    W4dGeneric( const W4dGeneric& );
    W4dGeneric& operator =( const W4dGeneric& );
    bool operator ==( const W4dGeneric& );
};

PER_DECLARE_PERSISTENT( W4dGeneric );

#endif

/* End GENERIC.HPP **************************************************/
