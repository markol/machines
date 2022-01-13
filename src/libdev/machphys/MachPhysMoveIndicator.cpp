#include "machphys/MachPhysMoveIndicator.hpp"

#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "render/colour.hpp"
#include "world4d/entyplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysMoveIndicator );

MachPhysMoveIndicator::MachPhysMoveIndicator(W4dEntity* pParent, const W4dTransform3d& localTransform, MATHEX_SCALAR size)
    : W4dSprite3d(pParent, localTransform, size, size, getMaterial())
{
    solid(W4dEntity::NOT_SOLID);
    doNotLight(true);
    visible(true);
}

MachPhysMoveIndicator::MachPhysMoveIndicator( PerConstructor con )
    : W4dSprite3d( con )
{
}

MachPhysMoveIndicator::~MachPhysMoveIndicator()
{

}

void MachPhysMoveIndicator::startFadeOut(const PhysAbsoluteTime& startTime)
{
    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.visibilityPlan(getFadeAwayPlan(), startTime);
}

ostream& operator <<( ostream& o, const MachPhysMoveIndicator& t )
{

    o << "MachPhysMoveIndicator " << (void*)&t << " start" << std::endl;
    o << "MachPhysMoveIndicator " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysMoveIndicator& indicator )
{
    const W4dSprite3d& base = indicator;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMoveIndicator& indicator )
{
    W4dSprite3d& base = indicator;

    istr >> base;
}

//static
const W4dVisibilityPlanPtr& MachPhysMoveIndicator::getFadeAwayPlan()
{
    static W4dVisibilityPlanPtr planPtr = new W4dVisibilityPlan(true);
    static bool doneOnce = false;

    if (not doneOnce)
    {
        doneOnce = true;
        constexpr auto duration = MachPhysMoveIndicator::DisplayTime;
        planPtr->add(false, duration);
    }

    return planPtr;
}

//static
RenMaterial MachPhysMoveIndicator::getMaterial()
{
    static RenMaterial indicatorMaterial_ = createMaterial();

    return indicatorMaterial_;
}
//static
RenTexture MachPhysMoveIndicator::createTexture()
{
    static RenTexture indicatorTexture_ = RenTexManager::instance().createTexture("move_indicator_t.bmp");

    return indicatorTexture_;
}

//static
RenMaterial MachPhysMoveIndicator::createMaterial()
{
    auto material = RenMaterial{ RenColour::magenta() };
    material.emissive(RenColour(1.0, 1.0, 1.0));
    material.texture(createTexture());
    material.diffuse(RenColour::magenta());

    return material;
}
