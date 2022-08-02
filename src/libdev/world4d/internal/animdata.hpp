/*
 * A N I M D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dAnimationData

    Abstract class. defines the animations specified in a LOD/CDF file
*/

#ifndef _WORLD4D_ANIMDATA_HPP
#define _WORLD4D_ANIMDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
//#include "stdlib/string.hpp"
#include "stdlib/strfwd.hpp"
#include "world4d/world4d.hpp"

class W4dEntity;

class W4dAnimationData
// Canonical form revoked
{
public:
    W4dAnimationData( const string& textureName, W4dLOD maxLod );
    virtual ~W4dAnimationData();

	const string& name() const;
	void name( const string& name);
	const W4dLOD& maxLod() const;

	//apply the animation to entity
	virtual void apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime) = 0;

	//all the animations are meant to last forever
	static const PhysRelativeTime& forever();

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_ABSTRACT( W4dAnimationData );
    PER_FRIEND_READ_WRITE( W4dAnimationData );

private:
    friend ostream& operator <<( ostream& o, const W4dAnimationData& t );

    W4dAnimationData( const W4dAnimationData& );
    W4dAnimationData& operator =( const W4dAnimationData& );

	string* textureName_;
	W4dLOD maxLod_;
};

PER_DECLARE_PERSISTENT( W4dAnimationData );

#endif

/* End ANIMDATA.HPP *************************************************/
