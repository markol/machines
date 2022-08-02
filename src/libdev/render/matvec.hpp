/*
 * M A T V E C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_MATVEC_HPP
#define _RENDER_MATVEC_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/mmvector.hpp"
#include "render/material.hpp"

// A material set represents all the materials used by a particular RenMesh 
// object.
class RenMaterialVec : public ctl_min_memory_vector<RenMaterial>
{
public:
	// The assignment op. and copy ctor. are available.  The ones 
	// generated by the compiler are fine.
    RenMaterialVec(size_t nMats);
    ~RenMaterialVec();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenMaterialVec& t );

	typedef ctl_min_memory_vector<RenMaterial> Base;

    PER_MEMBER_PERSISTENT( RenMaterialVec );
private:
    // Operations deliberately revoked
    bool operator ==( const RenMaterialVec& );
};

PER_READ_WRITE( RenMaterialVec );
PER_DECLARE_PERSISTENT( RenMaterialVec );

#endif

/* End MATVEC.HPP ***************************************************/