#pragma once

#include "base/persist.hpp"

#include "world4d/sprite3d.hpp"
#include "render/material.hpp"
#include "render/render.hpp"
#include "world4d/visplan.hpp"

// I'm attempting to adhere to the structure of other MachPhys*** classes to avoid unpleasant surprises.
class MachPhysMoveIndicator : public W4dSprite3d
{
public:
    MachPhysMoveIndicator(W4dEntity* pParent, const W4dTransform3d& localTransform, MATHEX_SCALAR size);
    ~MachPhysMoveIndicator();

    // NON-COPYABLE /////////////////////////////////////////////////////////////
    MachPhysMoveIndicator( const MachPhysMoveIndicator& ) = delete;
    MachPhysMoveIndicator& operator =( const MachPhysMoveIndicator& ) = delete;
    bool operator ==( const MachPhysMoveIndicator& ) = delete;
    /////////////////////////////////////////////////////////////////////////////

    static constexpr PhysRelativeTime DisplayTime{ 1 };
    void startFadeOut(const PhysAbsoluteTime& startTime);

    /////////////////////////////////////////////////////////////////////////////
    friend ostream& operator <<( ostream& o, const MachPhysMoveIndicator& t );

    PER_MEMBER_PERSISTENT( MachPhysMoveIndicator );
    PER_FRIEND_READ_WRITE( MachPhysMoveIndicator );

private:
    static const W4dVisibilityPlanPtr& getFadeAwayPlan();
    static RenMaterial getMaterial();

    static RenTexture createTexture();
    static RenMaterial createMaterial();
};

PER_DECLARE_PERSISTENT( MachPhysMoveIndicator );
