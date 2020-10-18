/*
 * U V D A T A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dUVTranslateData

    defines the UVAnimationdata specified in a LOD/CDF file
*/

#ifndef _WORLD4D_UVDATA_HPP
#define _WORLD4D_UVDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "world4d/internal/animdata.hpp"
#include "mathex/vec2.hpp"

class W4dEntity;

class W4dUVTranslateData: public  W4dAnimationData
// Canonical form revoked
{
public:
    W4dUVTranslateData( const string& textureName, const MexVec2& speedVec, W4dLOD maxLod );
    virtual ~W4dUVTranslateData( void );

	//apply the uv animation to pEntity	 start at statTime
	virtual void apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime);

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dUVTranslateData);
    PER_FRIEND_READ_WRITE(W4dUVTranslateData);

private:
    friend ostream& operator <<( ostream& o, const W4dUVTranslateData& t );

    W4dUVTranslateData( const W4dUVTranslateData& );
    W4dUVTranslateData& operator =( const W4dUVTranslateData& );

	MexVec2 speedVec_;
};

PER_DECLARE_PERSISTENT(W4dUVTranslateData);

#endif

/* End UVDATA.HPP ***************************************************/
