/*
 * D A M A G E I . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/damagei.hpp"
#include "world4d/entity.hpp"
#include "world4d/garbcoll.hpp"
#include "machphys/constron.hpp"
#include "machphys/stflames.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/splat.hpp"
#include "machphys/consdata.hpp"
#include "machphys/random.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysEntityDamageImpl );

static void flameParameters( ctl_vector<MexTransform3d>* pPositions,
                             ctl_vector<MATHEX_SCALAR>* pWidths,
                             ctl_vector<MATHEX_SCALAR>* pHeights,
							 MachPhys::ConstructionType type,
							 const size_t& hwLevel );

static void flameParameters( ctl_vector<MexTransform3d>* pPositions,
                             ctl_vector<MATHEX_SCALAR>* pWidths,
                             ctl_vector<MATHEX_SCALAR>* pHeights,
							 MachPhys::ConstructionType type,
							 int subType,
							 const size_t& hwLevel );

MachPhysEntityDamageImpl::MachPhysEntityDamageImpl( W4dEntity* pTarget):
	pTarget_( pTarget ),
	ptargetConstruction_( NULL ),
	pSplat_( NULL ),
	pSmoke1_( NULL ),
	pSmoke2_( NULL ),
	pFlame1_( NULL ),
	pFlame2_( NULL ),
	pFlame3_( NULL ),
	damageLevel_( 0 ),
	smokeDuration_( 60 ),
	lastSmoke1Time_( 0 ),
	lastSmoke2Time_( 0 ),
	transform_( pTarget->localTransform() ),
	smoke1Type_( PUFF_3 ),
	smoke2Type_( PUFF_4 ),
	targetType_( MachPhysEntityDamageImpl::NON_CONSTRUCTION ),
	targetVolume_( pTarget_->isComposite()? pTarget_->asComposite().compositeBoundingVolume():
			                                pTarget_->boundingVolume() )
{
	PRE( damageLevel_ >= 0 );

	const MATHEX_SCALAR sizeX = targetVolume_.xLength();
	const MATHEX_SCALAR sizeY = targetVolume_.yLength();
	const MATHEX_SCALAR sizeZ = targetVolume_.zLength();
	const MexPoint3d centroid = targetVolume_.centroid();

	flameTransforms_.reserve(3);
	flameWidths_.reserve(3);
	flameHeights_.reserve(3);

	if( sizeX/sizeY > 1.8 )
	{
		MATHEX_SCALAR offset = 0.25*sizeX;

		flameTransforms_.push_back( MexTransform3d() );
		flameTransforms_.push_back( MexTransform3d(MexPoint3d(centroid.x()-offset, centroid.y(), 0.0)) );
		flameTransforms_.push_back( MexTransform3d(MexPoint3d(centroid.x()+offset, centroid.y(), 0.0)) );

		flameWidths_.push_back(0);
		flameWidths_.push_back(sizeY);
		flameWidths_.push_back(sizeY);

		flameHeights_.push_back(0);
		flameHeights_.push_back(sizeZ*MachPhysRandom::randomDouble(1.2, 1.8));
		flameHeights_.push_back(sizeZ*MachPhysRandom::randomDouble(1.2, 1.8));
	}
	else if( sizeY/sizeX >1.8 )
	{
		MATHEX_SCALAR offset = 0.25*sizeY;

		flameTransforms_.push_back( MexTransform3d() );
		flameTransforms_.push_back( MexTransform3d(MexPoint3d(centroid.x(), centroid.y()-offset, 0.0)) );
		flameTransforms_.push_back( MexTransform3d(MexPoint3d(centroid.x(), centroid.y()+offset, 0.0)) );

		flameWidths_.push_back(0);
		flameWidths_.push_back(sizeX);
		flameWidths_.push_back(sizeX);

		flameHeights_.push_back(0);
		flameHeights_.push_back(sizeZ*MachPhysRandom::randomDouble(1.2, 1.8));
		flameHeights_.push_back(sizeZ*MachPhysRandom::randomDouble(1.2, 1.8));
	}
	else
	{
		flameTransforms_.push_back( MexTransform3d() );
		flameTransforms_.push_back( MexTransform3d() );
		flameTransforms_.push_back( MexTransform3d(MexPoint3d(centroid.x(), centroid.y(), 0.0)) );

		flameWidths_.push_back(0);
		flameWidths_.push_back(0);
		flameWidths_.push_back(std::min(sizeX, sizeY));

		flameHeights_.push_back(0);
		flameHeights_.push_back(0);
		flameHeights_.push_back(sizeZ*MachPhysRandom::randomDouble(1.2, 1.8));
	}

    TEST_INVARIANT;
}

MachPhysEntityDamageImpl::MachPhysEntityDamageImpl( MachPhysConstruction* pTarget):
	pTarget_( pTarget ),
	ptargetConstruction_( pTarget ),
	pSplat_( NULL ),
	pSmoke1_( NULL ),
	pSmoke2_( NULL ),
	pFlame1_( NULL ),
	pFlame2_( NULL ),
	pFlame3_( NULL ),
	damageLevel_( 0 ),
	smokeDuration_( 60 ),
	transform_( pTarget->localTransform() ),
	smoke1Type_( PUFF_3 ),
	smoke2Type_( PUFF_4 ),
	targetType_( MachPhysEntityDamageImpl::CONSTRUCTION ),
	targetVolume_( pTarget_->isComposite()? pTarget_->asComposite().compositeBoundingVolume():
			                                pTarget_->boundingVolume() )
{
	PRE( damageLevel_ >= 0 );
	PRE( targetType_ == CONSTRUCTION );
	PRE( ptargetConstruction_ != NULL );

	MachPhys::ConstructionType type = ptargetConstruction_->constructionData().constructionType();
	const size_t hwLevel = ptargetConstruction_->constructionData().hwLevel();

	switch( type )
	{
		case  MachPhys::BEACON:
		case MachPhys::POD:
		case MachPhys::GARRISON:
		case MachPhys::SMELTER:
		case MachPhys::MINE:
			flameParameters( &flameTransforms_, &flameWidths_, &flameHeights_, type,  hwLevel );
			break;

		case MachPhys::FACTORY:
		case MachPhys::HARDWARE_LAB:
		case MachPhys::MISSILE_EMPLACEMENT:
		{
			int	 subType = ptargetConstruction_->constructionData().subType();
			flameParameters( &flameTransforms_, &flameWidths_, &flameHeights_, type, subType,  hwLevel );
			break;
		}
		default:
			ASSERT_BAD_CASE_INFO(type );
			break;
	}

    TEST_INVARIANT;
}

MachPhysEntityDamageImpl::MachPhysEntityDamageImpl( PerConstructor )
{
}

MachPhysEntityDamageImpl::~MachPhysEntityDamageImpl()
{
	if(pSplat_  != NULL )
    	W4dGarbageCollector::instance().add( pSplat_, lastSmoke2Time_ + smokeDuration_ );

	if(pSmoke1_ != NULL )
    	W4dGarbageCollector::instance().add( pSmoke1_, lastSmoke1Time_ + smokeDuration_ );

	if(pSmoke2_ != NULL )
    	W4dGarbageCollector::instance().add( pSmoke2_, lastSmoke2Time_ + smokeDuration_ );

	_DELETE( pFlame1_ );
	_DELETE( pFlame2_ );
	_DELETE( pFlame3_ );

    TEST_INVARIANT;

}


void MachPhysEntityDamageImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysEntityDamageImpl& t )
{

    o << "MachPhysEntityDamageImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysEntityDamageImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& str, const MachPhysEntityDamageImpl& t )
{
    str << t.pTarget_;
	str << t.pSplat_;
	str << t.pSmoke1_;
	str << t.pSmoke2_;
	str << t.damageLevel_;
	str << t.pFlame1_;
	str << t.pFlame2_;
	str << t.pFlame3_;
	str << t.lastSmoke1Time_;
	str << t.lastSmoke2Time_;
	str << t.smokeDuration_;
	str << t.transform_;
	str << t.smoke1Type_;
	str << t.smoke2Type_;
	str << t.targetType_;
	str << t.ptargetConstruction_;
	str << t.targetType_;
	str << t.targetVolume_;
	str << t.flameTransforms_;
	str << t.flameWidths_;
	str << t.flameHeights_;
}

void perRead( PerIstream& str, MachPhysEntityDamageImpl& t )
{
    str >> t.pTarget_;
	str >> t.pSplat_;
	str >> t.pSmoke1_;
	str >> t.pSmoke2_;
	str >> t.damageLevel_;
	str >> t.pFlame1_;
	str >> t.pFlame2_;
	str >> t.pFlame3_;
	str >> t.lastSmoke1Time_;
	str >> t.lastSmoke2Time_;
	str >> t.smokeDuration_;
	str >> t.transform_;
	str >> t.smoke1Type_;
	str >> t.smoke2Type_;
	str >> t.targetType_;
	str >> t.ptargetConstruction_;
	str >> t.targetType_;
	str >> t.targetVolume_;
	str >> t.flameTransforms_;
	str >> t.flameWidths_;
	str >> t.flameHeights_;
}


static void flameParameters( ctl_vector<MexTransform3d>* pPositions,
                             ctl_vector<MATHEX_SCALAR>* pWidths,
                             ctl_vector<MATHEX_SCALAR>* pHeights,
							 MachPhys::ConstructionType type,
							 const size_t& hwLevel )
{
    switch( type )
    {
        case MachPhys::POD:
		{
			static ctl_vector<MexTransform3d> positions;
			static ctl_vector<MATHEX_SCALAR> widths;
			static ctl_vector<MATHEX_SCALAR> heights;
			static bool first = true;
			if( first )
			{
				first = false;
				positions.reserve(3);
				widths.reserve(3);
				heights.reserve(3);

				positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 29 )) );
				positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
				positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 20 )) );

				widths.push_back(10);
				widths.push_back(40);
				widths.push_back(20);

				heights.push_back(10);
				heights.push_back(40);
				heights.push_back(20);
			}

			*pPositions = positions;
			*pWidths = widths;
			*pHeights = heights;

            break;
		}
        case MachPhys::GARRISON:
		{
			static ctl_vector<MexTransform3d> positions;
			static ctl_vector<MATHEX_SCALAR> widths;
			static ctl_vector<MATHEX_SCALAR> heights;
			static bool first = true;
			if( first )
			{
				first = false;

				positions.reserve(3);
				widths.reserve(3);
				heights.reserve(3);

				positions.push_back( MexTransform3d(MexPoint3d( 10, 10, 8 )) );
				positions.push_back( MexTransform3d(MexPoint3d( -10, 0, 8 )) );
				positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

				widths.push_back(8);
				widths.push_back(8);
				widths.push_back(40);

				heights.push_back(30);
				heights.push_back(30);
				heights.push_back(50);
			}

			*pPositions = positions;
			*pWidths = widths;
			*pHeights = heights;

            break;
		}
        case MachPhys::SMELTER:

			switch( hwLevel )
			{
				case 1:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 12, 0, 6 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 2, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 18.5, 9.5, 6 )) );

						widths.push_back(15);
						widths.push_back(25);
						widths.push_back(3);

						heights.push_back(18);
						heights.push_back(50);
						heights.push_back(15);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				case 3:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 11 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 15 )) );

						widths.push_back(36);
						widths.push_back(12);
						widths.push_back(6);

						heights.push_back(36);
						heights.push_back(15);
						heights.push_back(15);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
			break;

        case MachPhys::MINE:
			switch( hwLevel )
			{
				case 1:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 5.6, -4.6, 3 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 5.6, 4.6, 3 )) );

						widths.push_back(2);
						widths.push_back(12);
						widths.push_back(2);

						heights.push_back(8);
						heights.push_back(28);
						heights.push_back(8);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				case 3:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( -1.65, -10, 3 )) );
						positions.push_back( MexTransform3d(MexPoint3d( -1.65, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( -1.65, 10, 3 )) );

						widths.push_back(5);
						widths.push_back(10);
						widths.push_back(7);

						heights.push_back(30);
						heights.push_back(35);
						heights.push_back(10);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				case 5:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( -3.5, 0, 28 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 4.5, -8, 33 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

						widths.push_back(8);
						widths.push_back(8);
						widths.push_back(40);

						heights.push_back(25);
						heights.push_back(20);
						heights.push_back(50);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
		break;
        case MachPhys::BEACON:
			switch( hwLevel )
			{
				case 1:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 13 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

						widths.push_back(2.5);
						widths.push_back(0);
						widths.push_back(8);

						heights.push_back(10);
						heights.push_back(0);
						heights.push_back(25);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				case 3:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( -1, 0, 22 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 7, 0, 24 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

						widths.push_back(5);
						widths.push_back(0.5);
						widths.push_back(10);

						heights.push_back(8);
						heights.push_back(4);
						heights.push_back(30);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
				default:
					ASSERT_BAD_CASE_INFO(hwLevel );
					break;
			}
            break;
        default:
            ASSERT_BAD_CASE_INFO( type );
            break;
    }

	POST( pPositions->size() == 3 );
	POST( pWidths->size() == 3 );
	POST( pHeights->size() == 3 );
}

static void flameParameters( ctl_vector<MexTransform3d>* pPositions,
                             ctl_vector<MATHEX_SCALAR>* pWidths,
                             ctl_vector<MATHEX_SCALAR>* pHeights,
							 MachPhys::ConstructionType type,
							 int subType,
							 const size_t& hwLevel )
{
	switch( type )
	{
		case MachPhys::FACTORY:

		    switch( subType )
		    {
		        case 0: //MachPhys::CIVILIAN:

					switch( hwLevel )
					{
						case 1:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( -1.5, 13.5, 18 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 5, -4, 7 )) );

								widths.push_back(5);
								widths.push_back(20);
								widths.push_back(8);

								heights.push_back(20);
								heights.push_back(40);
								heights.push_back(16);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 9 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -2.7, 7, 22 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 10, 10, 8 )) );

								widths.push_back(26);
								widths.push_back(9);
								widths.push_back(10);

								heights.push_back(26);
								heights.push_back(20);
								heights.push_back(20);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 5:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 10, 10, 10 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 1.84, 2.95, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -3, 8, 25 )) );

								widths.push_back(14);
								widths.push_back(50);
								widths.push_back(10);

								heights.push_back(20);
								heights.push_back(60);
								heights.push_back(40);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;

		        case 1: //MachPhys::MILITARY:

					switch( hwLevel )
					{
						case 1:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 1, 6, 8 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 6.5, 0, 3.8 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 1, -5, 0 )) );

								widths.push_back(5);
								widths.push_back(13);
								widths.push_back(15);

								heights.push_back(25);
								heights.push_back(20);
								heights.push_back(30);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 5.5, 0, 5 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -3, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -2, 0, 20 )) );

								widths.push_back(14);
								widths.push_back(20);
								widths.push_back(3);

								heights.push_back(18);
								heights.push_back(45);
								heights.push_back(10);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 4:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 6.8, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 20, 0, 14 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -4, 0, 0 )) );

								widths.push_back(10);
								widths.push_back(14);
								widths.push_back(15);

								heights.push_back(30);
								heights.push_back(16);
								heights.push_back(50);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 5:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, 17, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, -17, 0 )) );

								widths.push_back(20);
								widths.push_back(30);
								widths.push_back(20);

								heights.push_back(50);
								heights.push_back(40);
								heights.push_back(50);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;

		        default:
		            ASSERT_BAD_CASE_INFO( subType );
		            break;
		    }

			break;

		case MachPhys::HARDWARE_LAB:

		    switch( subType )
		    {
		        case 0: //MachPhys::LAB_CIVILIAN:

					switch( hwLevel )
					{
						case 1:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 5, 0, 6 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -6, 0, 8 )) );

								widths.push_back(8);
								widths.push_back(20);
								widths.push_back(5);

								heights.push_back(25);
								heights.push_back(30);
								heights.push_back(10);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 12, 0, 7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 12 )) );

								widths.push_back(8);
								widths.push_back(40);
								widths.push_back(10);

								heights.push_back(16);
								heights.push_back(60);
								heights.push_back(20);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;

		        case 1: //MachPhys::LAB_MILITARY:

					switch( hwLevel )
					{
						case 1:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 4, 1, 7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( -4, -1, 7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

								widths.push_back(5);
								widths.push_back(8);
								widths.push_back(20);

								heights.push_back(10);
								heights.push_back(20);
								heights.push_back(35);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( -3, -4, 6.6 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 3, 4, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

								widths.push_back(8);
								widths.push_back(20);
								widths.push_back(16);

								heights.push_back(16);
								heights.push_back(35);
								heights.push_back(32);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;

		        default:
		            ASSERT_BAD_CASE_INFO( subType );
		            break;
		    }
		break;

		case MachPhys::MISSILE_EMPLACEMENT:

		   switch( subType )
		    {
		        case 0: //MachPhys::TURRET:
				{
					switch( hwLevel )
					{
						case 1:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 1.7 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 1.8, 0, 1.4 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );

								widths.push_back(1);
								widths.push_back(0.2);
								widths.push_back(2.5);

								heights.push_back(2);
								heights.push_back(1);
								heights.push_back(3);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 2:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 2 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 1.5, 1.5, 2 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 1.5, -1.5, 2 )) );

								widths.push_back(1.5);
								widths.push_back(0.6);
								widths.push_back(0.6);

								heights.push_back(2);
								heights.push_back(3);
								heights.push_back(3);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, 1, 2.5 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, -1, 2.5 )) );

								widths.push_back(1.5);
								widths.push_back(3);
								widths.push_back(1.5);

								heights.push_back(3);
								heights.push_back(9);
								heights.push_back(3);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;
				}
		        case 1: //MachPhys::SENTRY:

					switch( hwLevel )
					{
						case 3:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

	 							positions.push_back( MexTransform3d(MexPoint3d( 0, -1.6, 3 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 1.6, 3 )) );

								widths.push_back(1);
								widths.push_back(2);
								widths.push_back(1);

								heights.push_back(4);
								heights.push_back(10);
								heights.push_back(4);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						case 4:
						{
							static ctl_vector<MexTransform3d> positions;
							static ctl_vector<MATHEX_SCALAR> widths;
							static ctl_vector<MATHEX_SCALAR> heights;
							static bool first = true;
							if( first )
							{
								first = false;

								positions.reserve(3);
								widths.reserve(3);
								heights.reserve(3);

								positions.push_back( MexTransform3d(MexPoint3d( 0, -1.7, 0 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0.5 )) );
								positions.push_back( MexTransform3d(MexPoint3d( 0, 1.7, 0 )) );

								widths.push_back(3);
								widths.push_back(5);
								widths.push_back(3);

								heights.push_back(10);
								heights.push_back(12);
								heights.push_back(15);
							}

							*pPositions = positions;
							*pWidths = widths;
							*pHeights = heights;

							break;
						}
						default:
							ASSERT_BAD_CASE_INFO(hwLevel );
							break;
					}
					break;


		        case 2: //MachPhys::LAUNCHER:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;
					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 5 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 3, 0, 0 )) );

						widths.push_back(4);
						widths.push_back(8);
						widths.push_back(1);

						heights.push_back(8);
						heights.push_back(18);
						heights.push_back(5);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
		        case 3: //MachPhys::ICBM:
				{
					static ctl_vector<MexTransform3d> positions;
					static ctl_vector<MATHEX_SCALAR> widths;
					static ctl_vector<MATHEX_SCALAR> heights;
					static bool first = true;

					if( first )
					{
						first = false;

						positions.reserve(3);
						widths.reserve(3);
						heights.reserve(3);

						positions.push_back( MexTransform3d(MexPoint3d( 7, 0, 5 )) );
						positions.push_back( MexTransform3d(MexPoint3d( 0, 0, 0 )) );
						positions.push_back( MexTransform3d(MexPoint3d( -15, 0, 0 )) );

						widths.push_back(10);
						widths.push_back(20);
						widths.push_back(5);

						heights.push_back(10);
						heights.push_back(40);
						heights.push_back(10);
					}

					*pPositions = positions;
					*pWidths = widths;
					*pHeights = heights;

					break;
				}
		        default:

		            ASSERT_BAD_CASE_INFO( subType );

		            break;
		    }

			break;
	    default:

	        ASSERT_BAD_CASE_INFO( type );

	        break;
	}

	POST( pPositions->size() == 3 );
	POST( pWidths->size() == 3 );
	POST( pHeights->size() == 3 );
}

/* End DAMAGEI.CPP **************************************************/
