/*
 * U V T R A N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _WORLD4D_UVTRANS_HPP
#define _WORLD4D_UVTRANS_HPP

#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "world4d/uvplan.hpp"
#include "render/render.hpp"
#include "render/uvtrans.hpp"

class RenTexture;

// Animates texture co-ordinates by applying a constant 2D velocity to the 
// uv co-ordinates.  The velocity resets to zero once the duration is up.
class W4dUVTranslation : public W4dUVPlan
{
public:
    //Transform will only be applied up to level of detail maxLOD.
    W4dUVTranslation(const PhysRelativeTime& duration, W4dLOD maxLOD,
                     const MexVec2& velocity, const MexVec2& initialOffset);
    virtual ~W4dUVTranslation();

    virtual const Ren::UVTransformPtr& transform(const PhysRelativeTime& t);
	const MexVec2& position() const;
	virtual void print(ostream&) const;
	
	const MexVec2& velocity() const;
	void setFilterTexture( const RenTexture& texture );

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dUVTranslation );
    PER_FRIEND_READ_WRITE( W4dUVTranslation );
    
private:
    //Returns the owned RenUVTransform as a RenUVTranslation
    RenUVTranslation& uvTranslation();
    const RenUVTranslation& uvTranslation() const;

    //Data members
	MexVec2				velocity_, offset_;
	Ren::UVTransformPtr	positionPtr_;
};

#endif

PER_DECLARE_PERSISTENT( W4dUVTranslation );

/* End UVTRANS.HPP **************************************************/

