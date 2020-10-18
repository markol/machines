/*
 * M U L T I C O L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dMultiColourPulseData

    defines the data for a text cycling animation specified by a LOD/CDF file
*/

#ifndef _WORLD4D_MULTICOL_HPP
#define _WORLD4D_MULTICOL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "render/render.hpp"
#include "render/texture.hpp"
#include "ctl/pvector.hpp"
#include "world4d/internal/animdata.hpp"

class W4dEntity;
class W4dColourPulseData;

class W4dMultiColourPulseData: public  W4dAnimationData
// Canonical form revoked
{
public:
    W4dMultiColourPulseData( const size_t& nData );

    virtual ~W4dMultiColourPulseData( void );

	//apply the texture cucling animation to pEntity  start at statTime
	virtual void apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime);

	void add( W4dColourPulseData* pData );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL(W4dMultiColourPulseData);
    PER_FRIEND_READ_WRITE(W4dMultiColourPulseData);

private:
    friend ostream& operator <<( ostream& o, const W4dMultiColourPulseData& t );

    W4dMultiColourPulseData( const W4dMultiColourPulseData& );
    W4dMultiColourPulseData& operator =( const W4dMultiColourPulseData& );

	ctl_pvector< W4dColourPulseData > colourPulseDataVec_;
};

PER_DECLARE_PERSISTENT(W4dMultiColourPulseData);

#endif

/* End MULTICOL.HPP **************************************************/
