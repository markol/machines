/*
 * L E G C O M B O . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/legcombo.hpp"
#include "machphys/levels.hpp"

// static
MachPhysLegalWeaponCombos& MachPhysLegalWeaponCombos::instance()
{
    static MachPhysLegalWeaponCombos instance_;
    return instance_;
}

MachPhysLegalWeaponCombos::MachPhysLegalWeaponCombos()
{

    TEST_INVARIANT;
}

MachPhysLegalWeaponCombos::~MachPhysLegalWeaponCombos()
{
    TEST_INVARIANT;

}

void MachPhysLegalWeaponCombos::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachPhysLegalWeaponCombos::LegalWeaponCombos MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::MachineType type, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( type, hwLevel ) );

	LegalWeaponCombos legalWeaponCombos;
	legalWeaponCombos.reserve(0);

    switch( type )
    {
        case MachPhys::GEO_LOCATOR:
        case MachPhys::SPY_LOCATOR:
        case MachPhys::RESOURCE_CARRIER:
        case MachPhys::APC:
			break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::AdministratorSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::BOSS:

			switch( hwLevel )
			{
				case 1:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::T_WASP_LIGHT_STING);
					legalWeaponCombos.push_back(MachPhys::T_WASP_METAL_STING);
					break;

				case 2:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::T_BEE_BOMB);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}

			break;

        case MachPhys::OVERSEER:

			switch( hwLevel )
			{
				case 2:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_PULSE_RIFLE_X2);
					legalWeaponCombos.push_back(MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED);
					break;

				case 3:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_PULSE_RIFLE_X2);
					legalWeaponCombos.push_back(MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}

			break;


        case MachPhys::COMMANDER:

			switch( hwLevel )
			{
				case 4:
					legalWeaponCombos.reserve(3);
					legalWeaponCombos.push_back(MachPhys::LR_MULTI_LAUNCHER5_X2);
					legalWeaponCombos.push_back(MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER);
					legalWeaponCombos.push_back(MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY);
					break;

				case 5:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_PULSE_CANNON_X2);
					legalWeaponCombos.push_back(MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;

        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::AggressorSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::GRUNT:

			switch( hwLevel )
			{
				case 1:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::R_BOLTER);
					legalWeaponCombos.push_back(MachPhys::R_ELECTRIC_CHARGE);
					break;

				case 2:
					legalWeaponCombos.reserve(3);
					legalWeaponCombos.push_back(MachPhys::L_BOLTER);
					legalWeaponCombos.push_back(MachPhys::L_ELECTRIC_CHARGE);
					legalWeaponCombos.push_back(MachPhys::L_FLAME_THROWER2);
					break;

				case 3:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_HEAVY_BOLTER1_X2);
					break;

				default:
					ASSERT_BAD_CASE_INFO( hwLevel );
					break;
			}

			break;

        case MachPhys::ASSASSIN:

			switch( hwLevel )
			{
				case 2:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::L_AUTO_CANNON);
					legalWeaponCombos.push_back(MachPhys::L_PLASMA_RIFLE);
					break;

				case 3:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_AUTO_CANNON_X2);
					legalWeaponCombos.push_back(MachPhys::LR_PLASMA_RIFLE_X2);
					break;

				default:
					ASSERT_BAD_CASE_INFO( hwLevel );
					break;
			}
			break;

        case MachPhys::KNIGHT:

			switch( hwLevel )
			{
				case 3:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_HEAVY_BOLTER2_X2);
					legalWeaponCombos.push_back(MachPhys::LR_PLASMA_CANNON1_X2);
					break;

				case 4:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LR_MULTI_LAUNCHER3_X2);
					legalWeaponCombos.push_back(MachPhys::LR_PLASMA_CANNON2_X2);
					break;

				case 5:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::LRT_MULTI_LAUNCHER4_X3);
					legalWeaponCombos.push_back(MachPhys::LRT_PLASMA_CANNON2_X3);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;


        case MachPhys::BALLISTA:

			switch( hwLevel )
			{
				case 3:
					legalWeaponCombos.reserve(2);
					legalWeaponCombos.push_back(MachPhys::T_MULTI_LAUNCHER1);
					legalWeaponCombos.push_back(MachPhys::T_VORTEX);
					break;

				case 4:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_MULTI_LAUNCHER2_X2);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;


          case MachPhys::NINJA:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7);
			break;

          default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::ConstructorSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::DOZER:
        case MachPhys::BUILDER:
        case MachPhys::BEHEMOTH:
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::TechnicianSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::LAB_TECH:
        case MachPhys::TECH_BOY:
        case MachPhys::BRAIN_BOX:
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::ConstructionType type, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( type, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( type )
    {
        case MachPhys::POD:
		{
			legalWeaponCombos.reserve(1);
			legalWeaponCombos.push_back(MachPhys::T_ION_ORBITAL_CANNON);
            break;
		}
        case MachPhys::SMELTER:
        case MachPhys::GARRISON:
        case MachPhys::MINE:
        case MachPhys::BEACON:
            break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::FactorySubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::CIVILIAN:
        case MachPhys::MILITARY:
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::HardwareLabSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::LAB_CIVILIAN:
        case MachPhys::LAB_MILITARY:
            break;
        default:
            ASSERT_BAD_CASE_INFO( subType );
            break;
    }
	return legalWeaponCombos;
}

MachPhysLegalWeaponCombos::LegalWeaponCombos  MachPhysLegalWeaponCombos::legalWeaponCombos( MachPhys::MissileEmplacementSubType subType, const size_t& hwLevel ) const
{
	PRE( MachPhysLevels::instance().levelValid( subType, hwLevel ) );

	MachPhysLegalWeaponCombos::LegalWeaponCombos legalWeaponCombos;

    switch( subType )
    {
        case MachPhys::TURRET:

			switch( hwLevel )
			{
				case 1:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::T_FLAME_THROWER1);
					break;

				case 2:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_AUTO_CANNON_X2);
					break;

				case 3:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_PULSE_RIFLE_X2);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;

        case MachPhys::SENTRY:

			switch( hwLevel )
			{
				case 3:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_MULTI_LAUNCHER5_X2);
					break;

				case 4:
					legalWeaponCombos.reserve(1);
					legalWeaponCombos.push_back(MachPhys::LR_MULTI_LAUNCHER6_X2);
					break;

				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;


        case MachPhys::LAUNCHER:

			legalWeaponCombos.reserve(1);
			legalWeaponCombos.push_back(MachPhys::LR_LARGE_MISSILE_X2);

			break;

        case MachPhys::ICBM:
			legalWeaponCombos.reserve(1);
			legalWeaponCombos.push_back(MachPhys::T_NUCLEAR_MISSILE);
			break;
        default:

            ASSERT_BAD_CASE_INFO( subType );

            break;
    }
	return legalWeaponCombos;
}

ostream& operator <<( ostream& o, const MachPhysLegalWeaponCombos& t )
{

    o << "MachPhysLegalWeaponCombos " << (void*)&t << " start" << std::endl;
    o << "MachPhysLegalWeaponCombos " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LEGCOMBO.CPP ****************************************************/
