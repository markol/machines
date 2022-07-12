/*
 * T E X D A T A . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dCycleTextureData

    defines the data for a text cycling animation specified by a LOD/CDF file
*/

#ifndef _WORLD4D_TEXDATA_HPP
#define _WORLD4D_TEXDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "ctl/vector.hpp"
#include "render/texture.hpp"
#include "stdlib/string.hpp"

typedef basic_string< char > string;

class W4dCycleTextureData
// Canonical form revoked
{
public:
    W4dCycleTextureData( const string& textureName, 
                         const size_t& startTexture,
                         const size_t& endTexture,
                         const size_t& nRepetations);

    virtual ~W4dCycleTextureData();

    PER_MEMBER_PERSISTENT(W4dCycleTextureData);
    PER_FRIEND_READ_WRITE(W4dCycleTextureData);

private:
    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const W4dCycleTextureData& t );
	friend class W4dCycleMultiTextureData;

    W4dCycleTextureData( const W4dCycleTextureData& );
    W4dCycleTextureData& operator =( const W4dCycleTextureData& );

	ctl_vector<RenTexture> keyTextures() const;

	string* textureName_;
	size_t startTexture_;
	size_t endTexture_;
	size_t nRepetations_;
};

PER_DECLARE_PERSISTENT(W4dCycleTextureData);

#endif

/* End TEXDATA.HPP **************************************************/
