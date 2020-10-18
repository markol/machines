/*
 * R A C E C H N G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "render/matvec.hpp"
#include "render/colour.hpp"
#include "render/hsv.hpp"
#include "machphys/racechng.hpp"
#include "machphys/internal/racechbo.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"

// static
MachPhysRaceChanger& MachPhysRaceChanger::instance()
{
    static MachPhysRaceChanger instance_;
    return instance_;
}

MachPhysRaceChanger::MachPhysRaceChanger()
{
    //Construct the body
    pBody_ = _NEW( MachPhysRaceChangerBody );
    TEST_INVARIANT;
}

MachPhysRaceChanger::~MachPhysRaceChanger()
{
    TEST_INVARIANT;
    _DELETE( pBody_ );
}

void MachPhysRaceChanger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysRaceChanger& t )
{

    o << "MachPhysRaceChanger " << (void*)&t << " start" << std::endl;
    o << "MachPhysRaceChanger " << (void*)&t << " end" << std::endl;

    return o;
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChanger::changer
(
    const MachPhysMachine& machine, MachPhys::Race toRace
) const
{
    return pBody_->changer( machine, machine.machineData(), toRace );
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChanger::changer
(
    const MachPhysMachine& machine, const MachPhysMachineData& data, MachPhys::Race toRace
) const
{
    return pBody_->changer( machine, data, toRace );
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChanger::changer
(
    const MachPhysConstruction& construction, MachPhys::Race toRace
) const
{
    return pBody_->changer( construction, construction.constructionData(), toRace );
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChanger::changer
(
    const MachPhysConstruction& construction, const MachPhysConstructionData& data,
    MachPhys::Race toRace
) const
{
    return pBody_->changer( construction, data, toRace );
}

const W4dCompositeMaterialVecChanger& MachPhysRaceChanger::changer
(
    const MachPhysWeapon& weapon, MachPhys::Race toRace
) const
{
    return pBody_->changer( weapon, toRace );
}

const RenColour& MachPhysRaceChanger::raceColour( MachPhys::Race race ) const
{
    static RenColour colour;

    switch( race )
    {
        case MachPhys::RED:
            colour = RenColour( 180.0/255.0, 0.0, 0.0 );
            break;
        case MachPhys::BLUE:
            colour = RenColour( 62/255.0, 86/255.0, 141/255.0 );
            break;
        case MachPhys::GREEN:
            colour = RenColour( 94/255.0, 114/255.0, 0.0 );
            break;
        case MachPhys::YELLOW:
            colour = RenColour( 229/255.0, 189/255.0, 0.0 );
            break;
    }

    return colour;
}

/*
const RenColour& MachPhysRaceChanger::raceColour( MachPhys::Race race ) const
{
    RenColour* colour;
    switch( race )
    {
        case MachPhys::RED:
		{
			static RenColour red;
			static bool first = true;
			if( first )
			{
				first = false;

				RenColourHSV hsvColour;
				hsvColour.hue( pBody_->hue( MachPhys::RED ) );
				red = hsvColour.rgb();
			}
            colour = &red;
            break;
		}
        case MachPhys::BLUE:
		{
			static RenColour blue;
			static bool first = true;
			if( first )
			{
				first = false;

				RenColourHSV hsvColour;
 				hsvColour.hue(pBody_->hue( MachPhys::BLUE ) );
			 	blue = hsvColour.rgb();
			}
            colour = &blue;
            break;
		}
        case MachPhys::GREEN:
		{
			static RenColour green;
			static bool first = true;
			if( first )
			{
				first = false;

				RenColourHSV hsvColour;
			 	hsvColour.hue( pBody_->hue( MachPhys::GREEN ) );
			 	green = hsvColour.rgb();
			}
            colour = &green;
            break;
		}
        case MachPhys::YELLOW:
		{
			static RenColour yellow;
			static bool first = true;
			if( first )
			{
				first = false;

				RenColourHSV hsvColour;
			 	hsvColour.hue( pBody_->hue( MachPhys::YELLOW ) );
			 	yellow = hsvColour.rgb();
			}
            colour = &yellow;
            break;
		}
    }

    return *colour;
}
*/
/* End RACECHNG.CPP *************************************************/
