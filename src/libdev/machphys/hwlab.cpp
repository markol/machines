/*
 * H W L A B . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/ofactory.hpp"
#include "machphys/subid.hpp"
#include "machphys/hwlab.hpp"

#include "stdlib/string.hpp"
#include "ctl/countptr.hpp"
#include "ctl/pvector.hpp"
#include "system/pathname.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

#include "render/colour.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "render/matvec.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"

#include "world4d/entity.hpp"
#include "world4d/soundman.hpp"
#include "world4d/light.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/uvtrans.hpp"
#include "world4d/colpulse.hpp"
#include "world4d/matplan.hpp"
#include "world4d/matseqpl.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"

#include "sim/manager.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/harddata.hpp"
#include "machphys/levels.hpp"
#include "machphys/snddata.hpp"

PER_DEFINE_PERSISTENT( MachPhysHardwareLab );

MachPhysHardwareLab::MachPhysHardwareLab(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    MachPhys::HardwareLabSubType subType,
    size_t level,
    MachPhys::Race race )
: MachPhysConstruction( part( subType, level ), pParent, localTransform, level, race ),
  pData_( _NEW( MachPhysHardwareLabData( part( subType, level ).data(), globalTransform() ) ) ),
  subType_( subType ),
  level_(level)
{
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/hlab.wav" ),
//                                      PhysAbsoluteTime( 0 ), 100.0, 15.0,
//                                      W4dSoundManager::LOOP_CONTINUOUSLY );

	size_t nNeons = part( subType, level ).neons_.size();
	neons_.reserve(nNeons);

	for( size_t iNeon=0; iNeon<nNeons; ++iNeon)
    	neons_.push_back( links()[ (part( subType, level ).neons_)[iNeon]->id() ] );

    TEST_INVARIANT;
}

//  This is the constructor that is used by the factory. It is the
//  only constructor that actually builds a hardware lab from scratch

MachPhysHardwareLab::MachPhysHardwareLab( W4dEntity* pParent, Id id )
: MachPhysConstruction( pParent, W4dTransform3d(), compositeFileName( id.subType_, id.level_ ),
	wireframeFileName( id.subType_, id.level_ ),
    interiorCompositeFileName( id.subType_, id.level_ ), 10.0, id.level_,
    MachPhysData::instance().hardwareLabData( id.subType_, id.level_ ) ),
  pData_( _NEW( MachPhysHardwareLabData( MachPhysData::instance().hardwareLabData( id.subType_, id.level_ ), W4dTransform3d() ) ) ),
  subType_( id.subType_ ),
  level_(id.level_)
{

	neons_.reserve( 4 );
	for( int i = 1; i<= 4; ++i )
	{
		char textN[4];
//		itoa(i, textN, 10 );
        sprintf(textN, "%d", i);
		string neonLinkName = "neon" + string(textN);
		W4dLink* pLink = NULL;
 	    if( findLink(neonLinkName, &pLink) )
		{
	    	neons_.push_back( pLink );
		}
	}

    TEST_INVARIANT;
}

MachPhysHardwareLab::MachPhysHardwareLab( PerConstructor con )
: MachPhysConstruction( con ),
  pData_( NULL )
{
}

MachPhysHardwareLab::~MachPhysHardwareLab()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}
/*
// static
MachPhysHardwareLab& MachPhysHardwareLab::factory( MachPhys::HardwareLabSubType subType, size_t level )
{
    static  Factory   factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::HARDWARE_LAB ) );

    return factory_.part(
        Id( subType, level ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, level ) );
}
*/
// static
MachPhysHardwareLab& MachPhysHardwareLab::part( MachPhys::HardwareLabSubType subType, size_t level )
{
    return factory().part(
        Id( subType, level ),
        MachPhysLevels::instance().uniqueHardwareIndex( subType, level ) );
}

// static
MachPhysHardwareLab::Factory& MachPhysHardwareLab::factory()
{
    static  Factory factory_( MachPhysLevels::instance().nHardwareIndices( MachPhys::HARDWARE_LAB ) );

    return factory_;
}

MachPhys::HardwareLabSubType MachPhysHardwareLab::subType() const
{
    return subType_;
}

//virtual
const MachPhysConstructionData& MachPhysHardwareLab::constructionData() const
{
	return data();
}

const MachPhysHardwareLabData& MachPhysHardwareLab::data() const
{
	return *pData_;
}

SysPathName MachPhysHardwareLab::compositeFileName( MachPhys::HardwareLabSubType subType, size_t level ) const
{
    SysPathName result;

	switch( subType )
	{
		case MachPhys::LAB_MILITARY:
		{
			switch( level )
		    {
        		case 1:
		            result = "models/lab/military/level1/exterior/lam1e.cdf";
	            	break;

        		case 3:
		            result = "models/lab/military/level3/exterior/lam3e.cdf";
	            	break;

		        default:
        	    	ASSERT_BAD_CASE_INFO( level );
					break;
			}
            break;
		}

		case MachPhys::LAB_CIVILIAN:
		{
			switch( level )
		    {
        		case 1:
        		    result = "models/lab/civilian/level1/exterior/lac1e.cdf";
					break;

        		case 3:
        		    result = "models/lab/civilian/level3/exterior/lac3e.cdf";
					break;

				default:
        	    	ASSERT_BAD_CASE_INFO( level );
					break;
			}
			break;
		}

		default:
			ASSERT_BAD_CASE_INFO( subType );
			break;
    }

    return result;
}

SysPathName MachPhysHardwareLab::wireframeFileName( MachPhys::HardwareLabSubType subType, size_t level ) const
{
    SysPathName result;

	switch( subType )
	{
		case MachPhys::LAB_MILITARY:
		{
			switch( level )
		    {
        		case 1:
		            result = "models/lab/military/level1/wirefram/lam1w.cdf";
	            	break;

        		case 3:
		            result = "models/lab/military/level3/wirefram/lam3w.cdf";
	            	break;

		        default:
        	    	ASSERT_BAD_CASE_INFO( level );
					break;
			}
            break;
		}

		case MachPhys::LAB_CIVILIAN:
		{
			switch( level )
		    {
        		case 1:
		            result = "models/lab/civilian/level1/wirefram/lac1w.cdf";
					break;

        		case 3:
		            result = "models/lab/civilian/level3/wirefram/lac3w.cdf";
					break;

				default:
        	    	ASSERT_BAD_CASE_INFO( level );
					break;
			}
			break;
		}

		default:
			ASSERT_BAD_CASE_INFO( subType );
			break;
    }

    return result;
}

SysPathName MachPhysHardwareLab::interiorCompositeFileName( MachPhys::HardwareLabSubType subType, size_t level ) const
{
    SysPathName result;

	HAL_STREAM(" MPHWLab::interiorCompositeFileName " << subType << std::endl );
	switch( subType )
	{
	case MachPhys::LAB_MILITARY:
	    switch( level )
	    {
	        case 1:
	            result = "models/lab/military/level1/interior/lam1i.cdf";
	            break;

	        case 3:
	            result = "models/lab/military/level3/interior/lam3i.cdf";
	            break;

	        default:
	            ASSERT_BAD_CASE_INFO( level );
	            break;
	    }
		break;
	case MachPhys::LAB_CIVILIAN:
	    switch( level )
	    {
	        case 1:
	            result = "models/lab/civilian/level1/interior/lac1i.cdf";
	            break;


	        case 3:
	            result = "models/lab/civilian/level3/interior/lac3i.cdf";
	            break;

	        default:
	            ASSERT_BAD_CASE_INFO( level );
	            break;
	    }
		break;
    default:
        ASSERT_BAD_CASE_INFO( subType );
        break;

	}
    return result;
}

void MachPhysHardwareLab::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysHardwareLab::persistenceInitialiseData()
{
    pData_ = _NEW( MachPhysHardwareLabData( MachPhysData::instance().hardwareLabData( subType(), level() ), W4dTransform3d() ) );

    persistenceConstructionData( *pData_ );
}

void perWrite( PerOstream& ostr, const MachPhysHardwareLab& lab )
{
    const MachPhysConstruction& base = lab;

    ostr << base;

    ostr << lab.subType_;
	ostr << lab.level_;
	ostr << lab.neons_;
}

void perRead( PerIstream& istr, MachPhysHardwareLab& lab )
{
    MachPhysConstruction& base = lab;

    istr >> base;

    istr >> lab.subType_;
    istr >> lab.level_;
    istr >> lab.neons_;

    lab.persistenceInitialiseData();
}

//virtual
void MachPhysHardwareLab::doWorking( bool setWorking )
{
	// only do anything if situation has changed
	if( setWorking != isWorking() )
	{
		const PhysRelativeTime duration = 60;
		PhysAbsoluteTime now = SimManager::instance().currentTime();

	    static const RenTexture keyTexture1 = RenTexManager::instance().createTexture( "SCRN1.BMP" );
	    static const RenTexture keyTexture2 = RenTexManager::instance().createTexture( "STATIC.BMP" );
		static W4dUVPlanPtr uvPlanIsWorkingPtr;
		static W4dUVPlanPtr uvPlanNotWorkingPtr;
		static bool first = true;
		if( first )
		{
			first = false;

			W4dUVTranslation* pUVIsWorkingTranslate = _NEW( W4dUVTranslation(100000, 0, MexVec2(0.1, 0), MexVec2(0,0)) );
			pUVIsWorkingTranslate->setFilterTexture( keyTexture1 );
			uvPlanIsWorkingPtr = pUVIsWorkingTranslate;

			W4dUVTranslation* pUVNotWorkingTranslate = _NEW( W4dUVTranslation(100000, 0, MexVec2(0.87, 0.37), MexVec2(0,0)) );
			pUVNotWorkingTranslate->setFilterTexture( keyTexture1 );
			uvPlanNotWorkingPtr = pUVNotWorkingTranslate;
		}

		const size_t nLinks = links().size();
		const size_t nInterLinks = pInteror()->links().size();

	    if( not isWorking() )	// setWorking must == true
	    {
			//working animations on the interiors of both c and m labs
			pInteror()->clearAnimation( MachPhys::CONSTRUCTION_NOT_WORKING );

			for( size_t iLink=0; iLink<nInterLinks; ++iLink)
			{
				W4dLink* pLink = pInteror()->links()[iLink];
				if ( pLink->hasMesh() )
				{
					bool isChanged = W4dMaterialSequencePlan::changeTexture( pLink, keyTexture2, keyTexture1 );

					pLink->entityPlanForEdit().uvPlan(uvPlanIsWorkingPtr, now, 0, MachPhys::CONSTRUCTION_WORKING);
				}
			}

			switch( subType() )
			{
				case MachPhys::MILITARY:
				{
					//neon objects blink for all level mlabs
					size_t nNeons = neons_.size();
					for(size_t i = 0; i< nNeons; ++i )
					{
						ASSERT( neons_[i] != NULL, "" );
						W4dColourPulsePlan::changeAllColour( neons_[i], RenColour::black(), RenColour::white(), 0.2, now + i*0.25, 100000, MachPhys::CONSTRUCTION_WORKING);
					}

					W4dSoundManager::instance().play( this, SID_MILITARYLAB,
				                                  PhysAbsoluteTime( 0 ), 0 );

					break;
				}
				case MachPhys::CIVILIAN:
				{
					clearAnimation( MachPhys::CONSTRUCTION_NOT_WORKING );

					for( size_t iLink=0; iLink<nLinks; ++iLink)
					{
						W4dLink* pLink = links()[iLink];
						if ( pLink->hasMesh() )
						{
							bool isChanged = W4dMaterialSequencePlan::changeTexture( pLink, keyTexture2, keyTexture1 );

							pLink->entityPlanForEdit().uvPlan(uvPlanIsWorkingPtr, now, 0, MachPhys::CONSTRUCTION_WORKING);
						}
					}

					W4dSoundManager::instance().play( this, SID_CIVILIANLAB,
				                                  PhysAbsoluteTime( 0 ), 0 );
					break;
				}

				default:
				{
					ASSERT_BAD_CASE_INFO( subType() );
					break;
				}
			}
		}
		else if( isWorking() ) // setWorking must == false
		{
			clearAnimation( MachPhys::CONSTRUCTION_WORKING );

		    W4dSoundManager::instance().play( this, SID_IDLE_CONSTRUCTION,
	                                     PhysAbsoluteTime( 0 ),
	                                     W4dSoundManager::LOOP_CONTINUOUSLY );
			W4dSoundManager::instance().stop( this );

	        if( subType() == MachPhys::CIVILIAN )
	        {
				for( size_t iLink=0; iLink<nLinks; ++iLink)
				{
					W4dLink* pLink = links()[iLink];
					if ( pLink->hasMesh() )
					{
						bool isChanged = W4dMaterialSequencePlan::changeTexture( pLink, keyTexture1, keyTexture2 );

						pLink->entityPlanForEdit().uvPlan(uvPlanNotWorkingPtr, now, 0, MachPhys::CONSTRUCTION_NOT_WORKING);
					}
				}
			}

			//the interior
			pInteror()->clearAnimation( MachPhys::CONSTRUCTION_WORKING );
			for( size_t iLink=0; iLink<nInterLinks; ++iLink)
			{
				W4dLink* pLink = pInteror()->links()[iLink];
				if ( pLink->hasMesh() )
				{
					bool isChanged = W4dMaterialSequencePlan::changeTexture( pLink, keyTexture1, keyTexture2 );

					pLink->entityPlanForEdit().uvPlan(uvPlanNotWorkingPtr, now, 0, MachPhys::CONSTRUCTION_NOT_WORKING);
				}
			}
		}
	}
}
void MachPhysHardwareLab::damageLevel( const double& percent )
{
	MachPhysConstruction::damageLevel( percent );

	switch ( subType() )
	{

		case MachPhys::MILITARY:

			damageSmoke1Type( M_LAB_BLUE );
			damageSmoke2Type( M_LAB_BROWN );
			break;

		case MachPhys::CIVILIAN:

			damageSmoke1Type( C_LAB_BLUE );
			damageSmoke2Type( C_LAB_WHITE );
			break;
	}
}


/* End HWLAB.CPP *****************************************************/
