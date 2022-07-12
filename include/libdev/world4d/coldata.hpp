/*
 * C O L D A T A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dColourPulseData

    colour pulsing animation data
*/

#ifndef _WORLD4D_COLDATA_HPP
#define _WORLD4D_COLDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"
#include "stdlib/string.hpp"

class W4dColourPulseData
// Canonical form revoked
{
public:
    W4dColourPulseData( const string& textureName,
                        const RenColour& fromCol,
                        const RenColour& toCol,
                        const PhysRelativeTime& duration);

    ~W4dColourPulseData();

    void CLASS_INVARIANT;

	const RenTexture& keyTexture() const;
    const RenColour& fromColour() const;
    const RenColour& toColour() const;
    const PhysRelativeTime& duration() const;

    PER_MEMBER_PERSISTENT(W4dColourPulseData);
    PER_FRIEND_READ_WRITE(W4dColourPulseData);

private:
    friend ostream& operator <<( ostream& o, const W4dColourPulseData& t );

    W4dColourPulseData( const W4dColourPulseData& );
    W4dColourPulseData& operator =( const W4dColourPulseData& );

	RenTexture keyTexture_;
    RenColour fromColour_;
    RenColour toColour_;
    PhysRelativeTime duration_;
};


PER_DECLARE_PERSISTENT(W4dColourPulseData);

#endif

/* End COLDATA.HPP **************************************************/
