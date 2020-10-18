/*
 * C O N S T R O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "machphys/constron.hpp"
#include "machphys/internal/constroi.hpp"

#include "mathex/vec2.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/cvexpgon.hpp"

#include "world4d/soundman.hpp"
#include "world4d/domain.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/mvecccha.hpp"
#include "world4d/fdedapln.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/link.hpp"

#include "sim/manager.hpp"

#include "machphys/entrance.hpp"
#include "machphys/consdata.hpp"
#include "machphys/racechng.hpp"
#include "machphys/damage.hpp"
#include "machphys/objexplo.hpp"
#include "machphys/snddata.hpp"
#include "machphys/missemp.hpp"
#include "machphys/pod.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/attack.hpp"
#include "machphys/weapon.hpp"

#include "render/colour.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachPhysConstruction );

typedef MachPhysConstructionImpl::Entrances Entrances;
typedef MachPhysConstructionImpl::LinkTransforms LinkTransforms;
typedef MachPhysConstructionImpl::CompletionBand CompletionBand;

MachPhysConstruction::MachPhysConstruction(
    W4dEntity* pParent,
    const MexTransform3d& localTransform,
    const SysPathName& compositeFileName,
    const SysPathName& /*wireframeFileName*/,
    double height,
    size_t level,
    const MachPhysConstructionData& data )

: W4dComposite( pParent, localTransform, SOLID ),
    pImpl_( _NEW( MachPhysConstructionImpl( height, level, MachPhys::RED, data, globalTransform() ) ) )
{
    CB_DEPIMPL( double, height_ );

    ASSERT_FILE_EXISTS( compositeFileName.pathname().c_str() );

    readCompositeFile( compositeFileName );

    setupEntrances();

    //Set the height using the boundary from the mesh
    height_ = compositeBoundingVolume().maxCorner().z() + 0.25;

    TEST_INVARIANT;
}

MachPhysConstruction::MachPhysConstruction(
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    const SysPathName& compositeFileName,
    const SysPathName& /*wireframeFileName*/,
    const SysPathName& interiorCompositeFileName,
    double height,
    size_t level,
    const MachPhysConstructionData& data )
: W4dComposite( pParent, localTransform, SOLID ),
    pImpl_( _NEW( MachPhysConstructionImpl( height, level, MachPhys::RED, data, globalTransform() ) ) )
{
	CB_DEPIMPL( W4dDomain*, pInteriorDomain_ );
	CB_DEPIMPL( W4dGenericComposite*, pInteriorComposite_ );

    ASSERT_FILE_EXISTS( compositeFileName.pathname().c_str() );
    ASSERT_FILE_EXISTS( interiorCompositeFileName.pathname().c_str() );

    readCompositeFile( compositeFileName );

    MexTransform3d  identityTransform;

    pInteriorDomain_ = _NEW( W4dDomain( this, identityTransform ) );

    pInteriorComposite_ = _NEW( W4dGenericComposite( pInteriorDomain_, identityTransform, interiorCompositeFileName ) );

    setupEntrances();

    POST( hasInterior() );
}

MachPhysConstruction::MachPhysConstruction(
    const MachPhysConstruction& copyMe,
    W4dEntity* pParent,
    const W4dTransform3d& localTransform,
    size_t level,
    MachPhys::Race race )

: W4dComposite( copyMe, pParent, localTransform ),
    pImpl_( _NEW( MachPhysConstructionImpl( copyMe.pImpl_->height_, level, race,
                                            copyMe.pImpl_->constructionData_, globalTransform() ) ) )
{
    CB_DEPIMPL( double, percentageComplete_ );
	CB_DEPIMPL( W4dLinks, adornments_ );
	CB_DEPIMPL( MachPhys::Race, race_ );
	CB_DEPIMPL( W4dDomain*, pInteriorDomain_ );
	CB_DEPIMPL( W4dGenericComposite*, pInteriorComposite_ );

    percentageComplete_ = copyMe.pImpl_->percentageComplete_;

    for( W4dLinks::const_iterator i = copyMe.pImpl_->adornments_.begin(); i != copyMe.pImpl_->adornments_.end(); ++i )
    {
        adornments_.push_back( links()[ (*i)->id() ] );
    }

    if( race_ != MachPhys::RED )
        convertMaterials( copyMe.pImpl_->constructionData_, race_ );

    if( copyMe.hasInterior() )
    {
        MexTransform3d  identityTransform;

        pInteriorDomain_ = _NEW( W4dDomain( this, identityTransform ) );

        pInteriorComposite_ = _NEW(
          W4dGenericComposite( *copyMe.pImpl_->pInteriorComposite_, pInteriorDomain_, identityTransform ) );
    }

    setupEntrances();

    POST( hasInterior() == copyMe.hasInterior() );
    TEST_INVARIANT;
}

MachPhysConstruction::MachPhysConstruction( PerConstructor con )
: W4dComposite( con ),
    pImpl_( NULL )
{
}

MachPhysConstruction::~MachPhysConstruction()
{
	CB_DEPIMPL( MachPhysConstructionImpl::Entrances, entrances_ );
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );

	TEST_INVARIANT;

    //Cancel its sound, if any
    W4dSoundManager::instance().stop( this );

    for( Entrances::iterator i = entrances_.begin(); i != entrances_.end(); ++i )
        _DELETE( *i );

    if(pDamage_)
    	_DELETE( pDamage_ );

	deleteSavedPadsTransforms();

    _DELETE( pImpl_ );
}

void MachPhysConstruction::persistenceConstructionData( const MachPhysConstructionData& data )
{
	CB_DEPIMPL( MachPhysConstructionData, constructionData_ );
	constructionData_ = MachPhysConstructionData( data, globalTransform() );
}

void MachPhysConstruction::adornments( const W4dLinks& newAdornments )
{
	CB_DEPIMPL( W4dLinks, adornments_ );
    adornments_ = newAdornments;
}

void MachPhysConstruction::adornmentsVisible( bool visible )
{
	CB_DEPIMPL( W4dLinks, adornments_ );
    for( W4dLinks::iterator i = adornments_.begin(); i != adornments_.end(); ++i )
    {
        (*i)->visible( visible );
    }
}


double MachPhysConstruction::percentageComplete( void ) const
{
	CB_DEPIMPL( double, percentageComplete_ );

    double result = percentageComplete_;

    POST( result >= 0.0 and result <= 100.0 );

    return result;
}

void MachPhysConstruction::percentageComplete( double newPercentage )
{
	CB_DEPIMPL( double, percentageComplete_ );
	CB_DEPIMPL( bool, completionVisualised_ );

    PRE_INFO( newPercentage );
    PRE( newPercentage >= 0.0 and newPercentage <= 100.0 );

	if(	newPercentage != percentageComplete_  or  not completionVisualised_ )
	{
    	doPercentageComplete( newPercentage );
	    percentageComplete_ = newPercentage;
	}
}

static const W4dMaterialPlanPtr& bandPlanPtr( CompletionBand band );

//static
void MachPhysConstruction::preloadConstructingPlans()
{
	bandPlanPtr(MachPhysConstructionImpl::BAND1 );
	bandPlanPtr(MachPhysConstructionImpl::BAND2 );
	bandPlanPtr(MachPhysConstructionImpl::BAND3 );
	bandPlanPtr(MachPhysConstructionImpl::BAND4 );
	bandPlanPtr(MachPhysConstructionImpl::BAND5 );
	bandPlanPtr(MachPhysConstructionImpl::BAND6 );
	bandPlanPtr(MachPhysConstructionImpl::BAND7 );
	bandPlanPtr(MachPhysConstructionImpl::BAND8 );
	bandPlanPtr(MachPhysConstructionImpl::BAND9 );
	bandPlanPtr(MachPhysConstructionImpl::BAND10 );
}

static const W4dMaterialPlanPtr& bandPlanPtr( CompletionBand band )
{
	PRE( band < MachPhysConstructionImpl::NBANDS );

	static RenMaterial solidGreen(RenColour::green());
	static bool first = true;

	if( first )
	{
		first = false;
		solidGreen.diffuse(RenColour::black());
		solidGreen.emissive(RenColour::green());
	}

	static const uint nMaterialsInVector = 100; // a reasonablely large number
	W4dMaterialPlanPtr* pPlanPtr = NULL;
	switch( band )
	{
		case MachPhysConstructionImpl::BAND1:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            0, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0, //minAlpha
														0.25 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}

		case MachPhysConstructionImpl::BAND2:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            0, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0, //minAlpha
														0.4 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}

		case MachPhysConstructionImpl::BAND3:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            0, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0, //minAlpha
														0.55 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}

		case MachPhysConstructionImpl::BAND4:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            0, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0, //minAlpha
														0.85 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND5:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            3, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0, //minAlpha
														0.55 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND6:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            5, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0.1, //minAlpha
														1.0 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND7:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            10, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0.2, //minAlpha
														1.0 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND8:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            40, //nFadedFrames
	    	                                            20, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0.3, //minAlpha
														1.0 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND9:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            30, //nFadedFrames
	    	                                            30, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0.4, //minAlpha
														1.0 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}
		case MachPhysConstructionImpl::BAND10:
		{
	    	static W4dSolidFadedAlphaPlan* pThePlan = _NEW( W4dSolidFadedAlphaPlan(
	    	                                            2, //nFadedFrames
	    	                                            2, // nSolidFrames
														nMaterialsInVector,
														solidGreen,
														10,
														1000000,
														0.5, //minAlpha
														1.0 ) ); //maxAlpha
			static W4dMaterialPlanPtr planPtr(pThePlan);
			pPlanPtr = &planPtr;
			break;
		}

	}

	return *pPlanPtr;
}

// virtual
void MachPhysConstruction::doPercentageComplete( double newPercentage )
{
	CB_DEPIMPL( double, percentageComplete_ );
	CB_DEPIMPL( bool, completionVisualised_ );

	CompletionBand band;

	if( newPercentage != percentageComplete_  or  not completionVisualised_ )
	{
        completionVisualised_ = true;

		if(      newPercentage < 10 ) band =  MachPhysConstructionImpl::BAND1;
		else if( newPercentage < 20 ) band =  MachPhysConstructionImpl::BAND2;
		else if( newPercentage < 30 ) band =  MachPhysConstructionImpl::BAND3;
		else if( newPercentage < 40 ) band =  MachPhysConstructionImpl::BAND4;
		else if( newPercentage < 50 ) band =  MachPhysConstructionImpl::BAND5;
		else if( newPercentage < 60 ) band =  MachPhysConstructionImpl::BAND6;
		else if( newPercentage < 70 ) band =  MachPhysConstructionImpl::BAND7;
		else if( newPercentage < 80 ) band =  MachPhysConstructionImpl::BAND8;
		else if( newPercentage < 90 ) band =  MachPhysConstructionImpl::BAND9;
		else if( newPercentage < 100) band =  MachPhysConstructionImpl::BAND10;
		else band = MachPhysConstructionImpl::NBANDS;

		if( band != pImpl_->completionBand_ )
		{

		    for( W4dEntityIter it( this ); not it.isFinished(); ++it )
			{
				// Make sure animation is only applied to correct entities (i.e. don't apply to fire and markers)
				if ( (*it).clientData() != (int)MachPhys::ECD_DONTAPPLYANIMATION )
				{
				    (*it).entityPlanForEdit().clearAnimation(MachPhys::CONSTRUCTION_CONSTRUCTING);
			        // don't bother with ANY animation if we're now complete.
					if( newPercentage < 100 )
					{
			        	(*it).entityPlanForEdit().materialPlan( bandPlanPtr(band), SimManager::instance().currentTime(), 0, MachPhys::CONSTRUCTION_CONSTRUCTING );
					}
 				}
			}
			pImpl_->completionBand_ = band;
		}
	 }

    if( newPercentage < 95.0 )
        adornmentsVisible( false );
    else
	{
        adornmentsVisible( true );
		//No hum/idle sound on
		//- Pod
		//- Beacon HW1, HW3
		//- Turret HW1, HW2, HW3
		//- Sentry HW3, HW4
		//- Launcher HW4
		bool playHum = true;
		const MachPhysConstructionData& theData = constructionData();
		switch(theData.constructionType())
		{
			case MachPhys::MISSILE_EMPLACEMENT:
			{
				RICHARD_STREAM("Emplacement" << std::endl);
				switch(theData.subType())
				{
 					case MachPhys::TURRET:
					case MachPhys::SENTRY:
					case MachPhys::LAUNCHER:
					RICHARD_STREAM("No Hum" << std::endl);
						playHum = false;
					break;
					case MachPhys::ICBM:
					RICHARD_STREAM("Hum" << std::endl);
						playHum = true;
					break;
				}
				break;
			}
			case MachPhys::POD:
				playHum = false;
			break;
			case MachPhys::BEACON:
			{
				switch(theData.hwLevel())
				{
 					case 1:
					case 2:
						playHum = false;
					break;
				}
			}
			break;
		}
		if(playHum)
		{
			RICHARD_STREAM("Playing Hum" << std::endl);
			W4dSoundManager::instance().play(this, SID_IDLE_CONSTRUCTION, PhysAbsoluteTime(0), 0);
		}
	}

}

size_t  MachPhysConstruction::level( void )
{
	CB_DEPIMPL( size_t, level_ );
    return level_;
}

size_t  MachPhysConstruction::level( void ) const
{
	CB_DEPIMPL( size_t, level_ );
    return level_;
}

void MachPhysConstruction::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysConstruction::convertMaterials( const MachPhysConstructionData& data, MachPhys::Race r )
{
    //Use the race changer to do the job
    const W4dCompositeMaterialVecChanger& changer =
        MachPhysRaceChanger::instance().changer( *this, data, r );

    changer.applyOverrides( this );
}

bool MachPhysConstruction::hasCanAttack() const
{
    MachPhys::ConstructionType thisConstructionType = constructionData().constructionType();
    return (thisConstructionType == MachPhys::MISSILE_EMPLACEMENT || thisConstructionType == MachPhys::POD);
}

MachPhysCanAttack* MachPhysConstruction::canAttack() const
{
    PRE( hasCanAttack() );
    MachPhys::ConstructionType thisConstructionType = constructionData().constructionType();

    MachPhysCanAttack* pThisAttack = NULL;

    if( thisConstructionType == MachPhys::MISSILE_EMPLACEMENT )
        pThisAttack = &asMissileEmplacement();
    else if( thisConstructionType == MachPhys::POD )
        pThisAttack = &asPod();

    return pThisAttack;
}

MachPhysMissileEmplacement& MachPhysConstruction::asMissileEmplacement() const
{
    PRE( constructionData().constructionType() == MachPhys::MISSILE_EMPLACEMENT );

    const MachPhysConstruction& construction = *this;
    //MachPhysMissileEmplacement& result = _STATIC_CAST( MachPhysMissileEmplacement&, construction );
    MachPhysMissileEmplacement& result =
        _STATIC_CAST( MachPhysMissileEmplacement&, _CONST_CAST(MachPhysConstruction&, construction ) );

    return result;
}

MachPhysPod& MachPhysConstruction::asPod() const
{
    PRE( constructionData().constructionType() == MachPhys::POD );

    const MachPhysConstruction& construction = *this;
    //MachPhysPod& result = _STATIC_CAST( MachPhysPod&, construction );
    MachPhysPod& result = _STATIC_CAST( MachPhysPod&, _CONST_CAST(MachPhysConstruction&, construction) );

    return result;
}

void MachPhysConstruction::isWorking( bool doWork )
{
	CB_DEPIMPL( bool, isWorking_ );

	/*
    if( isWorking_ != doWork )
    {
        doWorking( doWork );
        isWorking_ = doWork;
    }
	*/

	doWorking( doWork );
	isWorking_ = doWork;
}

bool MachPhysConstruction::isWorking() const
{
	CB_DEPIMPL( bool, isWorking_ );
    return isWorking_;
}

//virtual
void MachPhysConstruction::doWorking( bool )
{
    //No default action
}

//virtual
uint MachPhysConstruction::nEntrances() const
{
	CB_DEPIMPL( MachPhysConstructionImpl::Entrances, entrances_ );
    return entrances_.size();
}

PhysRelativeTime MachPhysConstruction::isEntranceOpen( uint index, bool doOpen )
{
    PRE_INFO( index );
    PRE_INFO( nEntrances() );
    PRE( index < nEntrances() );
    return pEntrance( index )->isOpen( doOpen );
}

bool MachPhysConstruction::isEntranceOpen( uint index ) const
{
    PRE( index < nEntrances() );
    return pEntrance( index )->isOpen();
}

//virtual
MachPhysEntrance* MachPhysConstruction::pEntrance( uint index ) const
{
    PRE_INFO( index );
    PRE_INFO( nEntrances() );
    PRE( index < nEntrances() );

	CB_DEPIMPL( MachPhysConstructionImpl::Entrances, entrances_ );

    return entrances_[ index ];
}

// virtual
PhysRelativeTime    MachPhysConstruction::destroy( void )
{
    return doDestroy();
}

PhysRelativeTime    MachPhysConstruction::doDestroy( void )
{
    adornmentsVisible( false );

    //Construct and use an explosion
    MachPhysObjectExplosion exploder( this );
	MachPhys::DemolitionType type;
	type.objectType=MachPhys::CONSTRUCTION;
	type.constructionType=constructionData().constructionType();
	type.subType=constructionData().subType();
	type.level=constructionData().hwLevel();

    PhysRelativeTime animationDuration = exploder.explode( SimManager::instance().currentTime(), type );

    W4dSoundManager::instance().stop( this );

    return animationDuration;
}

bool    MachPhysConstruction::hasInterior( void ) const
{
	CB_DEPIMPL( W4dDomain*, pInteriorDomain_ );
    return pInteriorDomain_ != NULL;
}

W4dDomain* MachPhysConstruction::pInteriorDomain( void ) const
{
    PRE( hasInterior() );

	CB_DEPIMPL( W4dDomain*, pInteriorDomain_ );

    W4dDomain* result = pInteriorDomain_;

    POST( result != NULL );

    return result;
}

void    MachPhysConstruction::setupEntrances( void )
{
	CB_DEPIMPL( MachPhysConstructionData, constructionData_ );
	CB_DEPIMPL( Entrances, entrances_ );

    const MachPhysConstructionData::Entrances& entrances = constructionData_.entrances();
    entrances_.reserve( entrances.size() );

    for( MachPhysConstructionData::Entrances::const_iterator i = entrances.begin();
      i != entrances.end(); ++i )
    {
        W4dLink*    pLDoor = NULL;
        W4dLink*    pRDoor = NULL;

        //  See what doors are present in the model
        findLink( (*i).doorEntityName, &pLDoor );
        findLink( "l" + (*i).doorEntityName, &pLDoor );
        findLink( "r" + (*i).doorEntityName, &pRDoor );

        //  These are just commented out until we get the proper door models in place

//        ASSERT_INFO( (*i).doorEntityName );
//        ASSERT( not( pLDoor == NULL and pRDoor == NULL ), "door models not found" );

        size_t  nDoors = 0;

        if( pLDoor != NULL )
            ++nDoors;
        if( pRDoor != NULL )
            ++nDoors;

        //  We want the open vectors to be in the building's axis system so
        //  transform the door end points by the inverse of the building's transform. This
        //  is really a bit naff, I'm sure there is a better solution to this - Bob

        MexTransform3d  transform;

        globalTransform().invert( &transform );

        MexPoint2d  doorEnd1( (*i).doorLine.end1() );
        MexPoint2d  doorEnd2( (*i).doorLine.end2() );

        transform.transform( &doorEnd1 );
        transform.transform( &doorEnd2 );

        MexVec2 doorOpenVector( doorEnd1, doorEnd2 );
        MATHEX_SCALAR   doorOpenDistance = doorOpenVector.modulus();
        doorOpenVector.makeUnitVector();

        entrances_.push_back( _NEW( MachPhysEntrance ) );

        MexVec3 leftOpen( doorOpenVector );
        leftOpen *= doorOpenDistance / nDoors;
        MexVec3 rightOpen( doorOpenVector );
        rightOpen *= -doorOpenDistance / nDoors;

        if( pLDoor != NULL )
            entrances_.back()->addDoor( pLDoor, leftOpen, 1, 1 );

        if( pRDoor != NULL )
            entrances_.back()->addDoor( pRDoor, rightOpen, 1, 1 );
    }

}

bool MachPhysConstruction::intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                           Accuracy accuracy ) const
{
    //Just check against bounding volume
    return defaultCompositeIntersectsLine( line, pDistance, accuracy );
}

void MachPhysConstruction::changeRace( MachPhys::Race newRace )
{
	CB_DEPIMPL( MachPhys::Race, race_ );
	YUEAI_STREAM(" old race = " << race_ << " newRace = " << newRace << std::endl );
	race_ = newRace;
	convertMaterials( constructionData(), race_ );

	//also change the race of the weapon(s)
	if( hasCanAttack() )
	{
	    const uint nWeapons = canAttack()->nMountedWeapons();

		YUEAI_STREAM(" hasCanAttack():  " << nWeapons << std::endl );
		for( uint i=0; i<nWeapons; ++i )
		    canAttack()->weapon( i ).changeRace();
	}
}

void MachPhysConstruction::changeColour( MachPhys::Race newRace )
{
	convertMaterials( constructionData(), newRace );

	//also change the colour of the weapon(s)
	if( hasCanAttack() )
	{
	    const uint nWeapons = canAttack()->nMountedWeapons();

		for( uint i=0; i<nWeapons; ++i )
		    canAttack()->weapon( i ).changeColour( newRace );
	}
}

void perWrite( PerOstream& ostr, const MachPhysConstruction& construction )
{
    const W4dComposite& base = construction;

    ostr << base;

    ostr << construction.pImpl_;
}

void perRead( PerIstream& istr, MachPhysConstruction& construction )
{
    W4dComposite& base = construction;

    istr >> base;

    istr >> construction.pImpl_;
}

MachPhys::Race MachPhysConstruction::race() const
{
	CB_DEPIMPL( MachPhys::Race, race_ );
    return race_;
}

//virtual
void MachPhysConstruction::damageLevel( const double& percent )
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );

	if( pDamage_==NULL )
		pDamage_ = _NEW(MachPhysEntityDamage( this ) );

	pDamage_->damageLevel(percent);
}

const double& MachPhysConstruction::damageLevel() const
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );
	static const double zero = 0;
	return (pDamage_==NULL? zero : pDamage_->damageLevel() );
}

void MachPhysConstruction::updateDamageLevel()
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );
	if( pDamage_ )
	{
		pDamage_->update();
		if( damaged() )	destroy();
	}
}

bool MachPhysConstruction::damaged() const
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );
	return ( pDamage_==NULL? false:	pDamage_->damaged() );
}

void MachPhysConstruction::damageSmoke1Type( const MachPhysPuffType& type1 )
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );
	if( pDamage_ )
	{
		pDamage_->smoke1Type( type1 );
	}
}

void MachPhysConstruction::damageSmoke2Type( const MachPhysPuffType& type2 )
{
	CB_DEPIMPL( MachPhysEntityDamage*, pDamage_ );
	if( pDamage_ )
	{
		pDamage_->smoke2Type( type2 );
	}
}

static bool isPadName( const string& name )
{
	const string str = name.substr( 0, 3 );
	return ( name == "fpad" ) or ( str == "pad" );
}

bool MachPhysConstruction::hasSavedPadsTransforms() const
{
    CB_DEPIMPL( LinkTransforms*, pSavedPadsTransforms_ );
	return pSavedPadsTransforms_ != NULL;
}

void MachPhysConstruction::savePadsTransforms()
{
	// reset the array of transforms
	PRE( not hasSavedPadsTransforms() );
	pImpl_->createSavedPadsTransforms();
	// Go threough the links and save
	for( W4dLinks::const_iterator it = links().begin(); it != links().end(); ++it )
	{
		if( isPadName( (*it)->name() ) )
		{
			pImpl_->savedPadsTransforms().push_back( MachPhysConstructionImpl::LinkTransformData( (*it)->id(), (*it)->localTransform() ) );
		}
	}
}

// reset the transforms of the pads from the values saved in savedPadsTransforms_
void MachPhysConstruction::restorePadsTransformsFromSave()
{
	bool linkSaved;
	MexTransform3d linkTransform;
	for( W4dLinks::const_iterator linkIt = links().begin(); linkIt != links().end(); ++linkIt )
	{
		if( isPadName( (*linkIt)->name() ) )
		{
			linkSaved = pImpl_->savedPadTransform( (*linkIt)->id(), &linkTransform );
			ASSERT_INFO( (*linkIt)->name() );
			ASSERT( linkSaved, "link transform has not been saved" );
			(*linkIt)->localTransform( linkTransform );
		}
	}
}

// free the space at the end of the building placement : don't wait for the object destruction
void MachPhysConstruction::deleteSavedPadsTransforms()
{
    CB_DEPIMPL( LinkTransforms*, pSavedPadsTransforms_ );
	if( pSavedPadsTransforms_ )
	{
		_DELETE( pSavedPadsTransforms_ );
		pSavedPadsTransforms_=NULL;
	}
}

void MachPhysConstruction::layPadsOnTerrain( const MachPhysPlanetSurface& terrain )
{
	// iterate through links and lay the ones named padXXX and fpad
	for( W4dLinks::const_iterator i = links().begin(); i != links().end(); ++i )
    {
        if( isPadName( (*i)->name() ) )
        	layPadOnTerrain( terrain, **i );
	}
}


static void layPadAroundPivot( W4dLink& pad, const MexPoint3d& pivot, const MexTransform3d& pivotTransform )
{

	// transform L -> G (link to global : actual global transform of the link
	MexTransform3d transformLToG( pad.globalTransform() );

	// compute transform C -> L, L -> C (pivot corner to link, link to pivot corner)
	const MexTransform3d transformCToL( pivot );
	MexTransform3d transformLToC;
	transformCToL.invert( &transformLToC );

	// compute global tranform L" to global
	MexTransform3d transformLSecondToG( transformLToG );
	transformLSecondToG.transform( transformCToL );
	transformLSecondToG.transform( pivotTransform );
	transformLSecondToG.transform( transformLToC );

	// Apply new global transform to the pad
	pad.globalTransform( transformLSecondToG );
}

static MATHEX_SCALAR sqrDistancePadVertices( const MexPoint3d& point, const MexVec3& normal, const ctl_vector< MexPoint3d >& vertices )
{
	PRE( vertices.size() == 4);

	MexVec3 v0( point, vertices[0] );
	MexVec3 v1( point, vertices[1] );
	MexVec3 v2( point, vertices[2] );
	MexVec3 v3( point, vertices[3] );

	MATHEX_SCALAR d0 = v0.dotProduct( normal );
	MATHEX_SCALAR d1 = v1.dotProduct( normal );
	MATHEX_SCALAR d2 = v2.dotProduct( normal );
	MATHEX_SCALAR d3 = v3.dotProduct( normal );

	return d0*d0 + d1*d1 + d2*d2 + d3*d3;
}

static bool verticesBelowPlan( const MexPoint3d& planePoint, const MexVec3& planeNormal, const ctl_vector< MexPoint3d >& vertices )
{

	MATHEX_SCALAR eps = MexEpsilon::instance();
	for( int i=0; i<vertices.size(); ++i )
	{
		if( planeNormal.dotProduct( MexVec3( planePoint, vertices[i] ) ) > eps )
			return false;
	}
	return true;
}

enum LAYTYPE { ALL_ABOVE, NOTHING_ABOVE, VERTICES_ABOVE };

static bool tryToLayThreeVerticesOnGround(
	uint indexPivot,
	const MachPhysPlanetSurface& terrain,
	const ctl_vector<MexPoint3d>& padVertices,
	LAYTYPE layType,
	MexPoint3d* pPivot,
	MexVec3* pNormal,
	MATHEX_SCALAR* pDistance )
{
	uint nVertices = padVertices.size();
	PRE( nVertices == 4 );
	PRE( indexPivot < nVertices );
	PRE( pNormal );
	PRE( pPivot );
	PRE( pDistance );

	bool result = false;
	MATHEX_SCALAR eps = MexEpsilon::instance();

	// The points projected on the ground and their normals
	MexVec3 normalVertex0, normalVertex1, normalVertex2, normalVertex3;
	ctl_vector< MexPoint3d > groundVertices;
	groundVertices.reserve( 4 );
	groundVertices.push_back( MexPoint3d( padVertices[0].x(), padVertices[0].y(), terrain.terrainHeight( padVertices[0].x(), padVertices[0].y(), &normalVertex0 ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[1].x(), padVertices[1].y(), terrain.terrainHeight( padVertices[1].x(), padVertices[1].y(), &normalVertex1 ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[2].x(), padVertices[2].y(), terrain.terrainHeight( padVertices[2].x(), padVertices[2].y(), &normalVertex2 ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[3].x(), padVertices[3].y(), terrain.terrainHeight( padVertices[3].x(), padVertices[3].y(), &normalVertex3 ) ) );

	MexVec3 edge01( MexVec3( groundVertices[0], groundVertices[1] ) );
	MexVec3 edge12( MexVec3( groundVertices[1], groundVertices[2] ) );
	MexVec3 edge23( MexVec3( groundVertices[2], groundVertices[3] ) );
	MexVec3 edge30( MexVec3( groundVertices[3], groundVertices[0] ) );

	// The plane going through the three ground vertices and its normal
  	MexPlane3d planeVertices;
	planeVertices.definePlane( groundVertices[((indexPivot+3)%nVertices)], groundVertices[indexPivot], groundVertices[((indexPivot+1)%nVertices)] );
	MexVec3 normalPlan( planeVertices.normal() );
	normalPlan.makeUnitVector();

	// Is their any crest line on any of the edges ?
	uint indexOppositePivot = ( (indexPivot+2) % nVertices );
	LIONEL_STREAM("indexPivot " << indexPivot << std::endl);
	LIONEL_STREAM("indexOppositePivot " << indexOppositePivot << std::endl);
	bool noCrestLine = false;
 	switch( indexPivot )
	{
		case 0:
			noCrestLine = ( (edge01.dotProduct( normalVertex0 ) > - eps) and (edge01.dotProduct( normalVertex1 ) < eps)
	 				  	and (edge30.dotProduct( normalVertex3 ) > - eps) and (edge30.dotProduct( normalVertex0 ) < eps) );
			break;
		case 1:
			noCrestLine = ( (edge01.dotProduct( normalVertex0 ) > - eps) and (edge01.dotProduct( normalVertex1 ) < eps)
	 				  	and (edge12.dotProduct( normalVertex1 ) > - eps) and (edge12.dotProduct( normalVertex2 ) < eps)	);
			break;
		case 2:
			noCrestLine = ( (edge12.dotProduct( normalVertex1 ) > - eps) and (edge12.dotProduct( normalVertex2 ) < eps)
	 				  	and (edge23.dotProduct( normalVertex2 ) > - eps) and (edge23.dotProduct( normalVertex3 ) < eps) );
			break;
		case 3:
			noCrestLine = ( (edge23.dotProduct( normalVertex2 ) > - eps) and (edge23.dotProduct( normalVertex3 ) < eps)
	 				  	and (edge30.dotProduct( normalVertex3 ) > - eps) and (edge30.dotProduct( normalVertex0 ) < eps) );
			break;
		default:
			ASSERT_BAD_CASE_INFO( indexPivot );

	}


	switch( layType )
	{
		case ALL_ABOVE:
			if( noCrestLine )
			{
				// check all four ground points are below the plan going through pivot and having normal normalPlan
				// (we don't want the pad to be under the ground)
				if( verticesBelowPlan( groundVertices[indexPivot], normalPlan, groundVertices ) )
				{
					// we have a possible plan
					*pDistance = sqrDistancePadVertices( groundVertices[indexPivot], normalPlan, groundVertices);
					*pNormal = normalPlan;
					*pPivot = padVertices[indexPivot];
					result = true;
				}
			}
			break;
		case VERTICES_ABOVE:
			// check all four ground points are below the plan going through pivot and having normal normalPlan
			// (we don't want the pad to be under the ground)
			if( verticesBelowPlan( groundVertices[indexPivot], normalPlan, groundVertices ) )
			{
				// we have a possible plan
				*pDistance = sqrDistancePadVertices( groundVertices[indexPivot], normalPlan, groundVertices);
				*pNormal = normalPlan;
				*pPivot = padVertices[indexPivot];
				result = true;
			}
			break;
		case NOTHING_ABOVE:
			// we have a possible plan
			*pDistance = sqrDistancePadVertices( groundVertices[indexPivot], normalPlan, groundVertices);
			*pNormal = normalPlan;
			*pPivot = padVertices[indexPivot];
			result = true;
			break;
		default:
			ASSERT_BAD_CASE_INFO( layType );
	}

	return result;
}

static bool tryToLayOneVertexOnGround(
	uint indexPivot,
	const MachPhysPlanetSurface& terrain,
	const ctl_vector<MexPoint3d>& padVertices,
	LAYTYPE layType,
	MexPoint3d* pPivot,
	MexVec3* pNormal,
	MATHEX_SCALAR* pDistance )
{

	uint nVertices = padVertices.size();

	PRE( nVertices == 4 );
	PRE( indexPivot < nVertices );
	PRE( pNormal );
	PRE( pPivot );
	PRE( pDistance );

	bool result = false;

	ctl_vector< MexPoint3d > groundVertices;
	groundVertices.reserve( 4 );
	groundVertices.push_back( MexPoint3d( padVertices[0].x(), padVertices[0].y(), terrain.terrainHeight( padVertices[0].x(), padVertices[0].y() ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[1].x(), padVertices[1].y(), terrain.terrainHeight( padVertices[1].x(), padVertices[1].y() ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[2].x(), padVertices[2].y(), terrain.terrainHeight( padVertices[2].x(), padVertices[2].y() ) ) );
	groundVertices.push_back( MexPoint3d( padVertices[3].x(), padVertices[3].y(), terrain.terrainHeight( padVertices[3].x(), padVertices[3].y() ) ) );

	// Get the pivot normal
	MexVec3 normalPivot;
	terrain.terrainHeight( padVertices[indexPivot].x(), padVertices[indexPivot].y(), &normalPivot );
	normalPivot.makeUnitVector();

	switch( layType )
	{
		case ALL_ABOVE:
			if( verticesBelowPlan( groundVertices[indexPivot], normalPivot, groundVertices ) )
			{
				*pDistance = sqrDistancePadVertices( groundVertices[indexPivot], normalPivot, groundVertices);
				*pNormal = normalPivot;
				*pPivot = padVertices[indexPivot];
				result = true;
			}
			break;

		case NOTHING_ABOVE:
			*pDistance = sqrDistancePadVertices( groundVertices[indexPivot], normalPivot, groundVertices);
			*pNormal = normalPivot;
			*pPivot = padVertices[indexPivot];
			result = true;
			break;
		default:
			ASSERT_BAD_CASE_INFO( layType );
	}
	return result;
}

// Tries different strategies to lay the pad and the ground
// returns the pivot and pad normal for the most successfull
static void layVerticesOnTerrain( const MachPhysPlanetSurface& terrain, const ctl_vector< MexPoint3d >& padVertices, MexPoint3d* pPivot, MexVec3* pNormal )
{
	PRE(padVertices.size()==4);
	PRE( pPivot );
	PRE( pNormal );

	// Displace the four corners to the surface
	// normals at the vertices

	LIONEL_STREAM("Enter layVerticesOnTerrain()" << std::endl);
	LIONEL_INDENT(1);
	MATHEX_SCALAR eps = MexEpsilon::instance();

	MATHEX_SCALAR distance = 0 , minDistance = 0;
	bool matchFound = false;
	MexPoint3d tryPivot;
	MexVec3 tryNormal;

	// First pass we try any combination of three vertices
	// The best solution will have
	// 1) all its 4 vertices above the ground
	// 2) no crest along any of its edgees
	// 3) a smaller average distance of the ground points to the pad than any other match
	// This pass will fail if we have a crest line or a saddle on the ground
	for( int i=0; i< padVertices.size(); ++i )
	{
		// Second vertex
		LIONEL_STREAM("Laying pad on three vertices (with vertex 1 as pivot)" << std::endl);
		if( tryToLayThreeVerticesOnGround( i, terrain, padVertices, ALL_ABOVE, &tryPivot, &tryNormal, &distance ) )
		{
			LIONEL_STREAM("Match found with some vertices below the ground" << std::endl);
			if( matchFound )
			{
				if( distance < minDistance )
				{
					LIONEL_STREAM("Better match than previous, solution kept" << std::endl);
					*pPivot = tryPivot;
					*pNormal = tryNormal;
					minDistance = distance;
				}
			}
			else
			{
				LIONEL_STREAM("Solution initialised " << std::endl);
				*pPivot = tryPivot;
				*pNormal = tryNormal;
				minDistance = distance;
			}
			matchFound = true;
		}
	}


	// We havent found pad made up of three vertices which is above the ground
	// we then use a pad which if parallel to one of the vertices's local surface
	// The following piece of code is supposed to deal with a crest line situation
	// that's the kind of situation where the former will have failed.
	// In a second pass we try to find the best solution having the four vertices above the ground
	if( not matchFound )
	{
		for( int i=0; i< padVertices.size(); ++i )
		{
			// Second vertex
			LIONEL_STREAM("Laying pad on vertex " << i << std::endl);
			if( tryToLayOneVertexOnGround( i,  terrain, padVertices, ALL_ABOVE, &tryPivot, &tryNormal, &distance ) )
			{
				LIONEL_STREAM("Match found (with all vertices above ground)" << std::endl);
				if( matchFound )
				{
					if( distance < minDistance )
					{
						LIONEL_STREAM("Better match than previous, solution kept" << std::endl);
						*pPivot = tryPivot;
						*pNormal = tryNormal;
						minDistance = distance;
					}
				}
				else
				{
					LIONEL_STREAM("Solution initialised" << std::endl);
					*pPivot = tryPivot;
					*pNormal = tryNormal;
					minDistance = distance;
				}
				matchFound = true;
			}
		}

		// In a third pass we accept solutions having edges crossing crest lines
		// That's a saddle situation (opposite curvatures in x and y)	which unfortunately
		// happens sometimes in game.
		// TBD I should try to find a better solution for saddles but could not think of any
		if( not matchFound )
		{
			for( int i=0; i< padVertices.size(); ++i )
			{
				// Second vertex
				LIONEL_STREAM("Laying pad on vertex " << i << std::endl);
				if( tryToLayThreeVerticesOnGround( i,  terrain, padVertices, VERTICES_ABOVE, &tryPivot, &tryNormal, &distance ) )
				{
					LIONEL_STREAM("Match found (with some vertices below the ground)" << std::endl);
					if( matchFound )
					{
						if( distance < minDistance )
						{
							LIONEL_STREAM("Better match than previous, solution kept" << std::endl);
							*pPivot = tryPivot;
							*pNormal = tryNormal;
							minDistance = distance;
						}
					}
					else
					{
						LIONEL_STREAM("Solution initialised " << std::endl);
						*pPivot = tryPivot;
						*pNormal = tryNormal;
						minDistance = distance;
					}
					matchFound = true;
				}
			}
		}
	}

	LIONEL_STREAM("pivot " << *pPivot << std::endl);
	LIONEL_STREAM("normal " << *pNormal << std::endl);

	LIONEL_INDENT(-1);
	LIONEL_STREAM("Exit layVerticesOnTerrain()" << std::endl);
}

void MachPhysConstruction::layPadOnTerrain( const MachPhysPlanetSurface& terrain, W4dLink& pad )
{
	MexTransform3d linkTransform = pad.globalTransform();

	// Get the bounding box of the pad and display
	const MexAlignedBox3d& boundingVolume = pad.boundingVolume();
	const MexAlignedBox3d globalVolume(boundingVolume, pad.globalTransform());

	// If the pad is outside the planet surface, just skip the math and leave the
	// pad untransformed.  (The GUI appears to generate bad building positions.
	// Or perhaps positions that don't account for the pads?)
	if (terrain.contains(globalVolume))
	{
		const MexPoint3d& minCorner = boundingVolume.minCorner();
		const MexPoint3d& maxCorner = boundingVolume.maxCorner();

		ctl_vector< MexPoint3d > padVertices;
		padVertices.reserve(4);
		// Get the four corners of the pad
		padVertices.push_back(	MexPoint3d(	minCorner.x(), minCorner.y(), minCorner.z() ) );
		padVertices.push_back(	MexPoint3d(	maxCorner.x(), minCorner.y(), minCorner.z() ) );
		padVertices.push_back(	MexPoint3d(	maxCorner.x(), maxCorner.y(), minCorner.z() ) );
		padVertices.push_back(	MexPoint3d(	minCorner.x(), maxCorner.y(), minCorner.z() ) );

		linkTransform.transform( &(padVertices[0]) );
		linkTransform.transform( &(padVertices[1]) );
		linkTransform.transform( &(padVertices[2]) );
		linkTransform.transform( &(padVertices[3]) );

		MexPlane3d plane;
		MexVec3 padNormal;
	  	plane.definePlane( padVertices[0], padVertices[1], padVertices[3] );
		padNormal = plane.normal();
		linkTransform.transformInverse( &padNormal );

		// Get the pivot point and pad normal
		MexPoint3d pivot;
		MexVec3 newPadNormal;

		// This method tries different strategies to lay the pad on the ground
		// and for the most successfull (or less dreadful)
		//  returns the pivot of the pad (point to which the transform will be applied)
		// and the new normal of the pad
		layVerticesOnTerrain( terrain, padVertices, &pivot, &newPadNormal );

		// compute translation of pivot
		MATHEX_SCALAR translationCPrimeToC =  terrain.terrainHeight( pivot.x(), pivot.y() ) - pivot.z() + 0.070;
		MexTransform3d transformCPrimeToC( MexPoint3d(0, 0, translationCPrimeToC ) );

		// express normal and pivot in local coordinates
		linkTransform.transformInverse( &newPadNormal );
		linkTransform.transformInverse( &pivot );

		// compute the rotation axis and rotation angle of pivot
		MexRadians rotationAngleCSecondToCPrime = padNormal.angleBetween( newPadNormal );
		MexVec3 rotationAxisCSecondToCPrime(  MexVec3::crossProduct( padNormal, newPadNormal ) );
		MexTransform3d transformCSecondToCPrime;
		if(not rotationAxisCSecondToCPrime.isZeroVector() )
			transformCSecondToCPrime.rotate( MexQuaternion( rotationAxisCSecondToCPrime, rotationAngleCSecondToCPrime ) );

		// compute  transfrom C" -> C  (pivot transform)
		MexTransform3d transformCSecondToC;
		transformCPrimeToC.transform( transformCSecondToCPrime, &transformCSecondToC );

		// Apply the pivot transform to the pad
		layPadAroundPivot( pad, pivot, transformCSecondToC );
	}
}

W4dGenericComposite* MachPhysConstruction::pInteror()
{
	PRE( hasInterior() );
	return 	pImpl_->pInteriorComposite_;
}

MexAlignedBox2d MachPhysConstruction::largestBoundary() const
{
    MexAlignedBox2d boundary = localLargestBoundary();

    //  Transform into global coordinates
    globalTransform().transform( &boundary );

    return boundary;
}

MexAlignedBox2d MachPhysConstruction::localLargestBoundary() const
{
    //Get the outer shell border
    const MachPhysConstructionData& data = constructionData();
    MexAlignedBox2d boundary = data.localBoundary();

    //Get the composite all inclusive bounding volume and construct its 2d equivalent
    const MexAlignedBox3d& boundary3d = compositeBoundingVolume();
	const MexAlignedBox2d meshBoundary2d( boundary3d.minCorner().x(),
                                    boundary3d.minCorner().y(),
                                    boundary3d.maxCorner().x(),
                                    boundary3d.maxCorner().y() );

    //Ensure we cover both
	boundary.unionWith( meshBoundary2d );

    //  Include all entrance polygons

    for( size_t i = 0; i < data.entrances().size(); ++i )
    {
        boundary.unionWith( data.localEntrances()[ i ].exteriorPolygon() );
    }

    return boundary;
}

MexAlignedBox2d MachPhysConstruction::externalGlobalBoundary() const
{
    //Get the outer shell border
    const MachPhysConstructionData& data = constructionData();
    MexAlignedBox2d boundary = data.localBoundary();

    //  Transform into global coordinates
    globalTransform().transform( &boundary );

    return boundary;
}

/* End CONSTRON.CPP *************************************************/
