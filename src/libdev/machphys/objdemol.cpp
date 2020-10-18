/*
 * O B J D E M O L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/map.hpp"
#include "ctl/countptr.hpp"
#include "stdlib/string.hpp"

#include "phys/lsclplan.hpp"

#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"

#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/emcolplan.hpp"
#include "world4d/compplan.hpp"

#include "machphys/objdemol.hpp"
#include "machphys/private/weapper.hpp"

#include "mathex/transf3d.hpp"
#include "system/pathname.hpp"

const string genericDemolitionFileName="models/destroy/demolish/demolish.cdf";

const string civilianHardwareLabDemolitionFileName="models/hardlab/civilian";
const string militaryHardwareLabDemolitionFileName="models/hardlab/military";

const string smelterDemolitionFileName="models/smelter";

const string civilianFactoryDemolitionFileName="models/factory/civilian";
const string militaryFactoryDemolitionFileName="models/factory/military";

const string turretMissileEmplacementDemolitionFileName="models/missilee/turret";
const string sentryMissileEmplacementDemolitionFileName="models/missilee/sentry";
const string launcherMissileEmplacementDemolitionFileName="models/missilee/launcher";
const string icbmMissileEmplacementDemolitionFileName="models/missilee/icbm";

const string garrisonDemolitionFileName="models/garrison";

const string mineDemolitionFileName="models/mine";

const string beaconDemolitionFileName="models/beacon";

const string podDemolitionFileName="models/pod";

const string demolishName="demolish/demolish.cdf";
const string levelName="level";

PER_DEFINE_PERSISTENT( MachPhysObjDemolish );

//one time ctor
//MachPhysObjDemolish::MachPhysObjDemolish(  )
//:   W4dComposite( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID )
//{
//
//	readCompositeFile( genericCompositeFileName( ) );
//
//}

//one time ctor
MachPhysObjDemolish::MachPhysObjDemolish(  MachPhys::DemolitionType type )
:   W4dComposite( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID ),
 isGeneric_(false)
{
	readCompositeFile( compositeFileName( type ) );
	if ( type.objectType==MachPhys::NOT_CONSTRUCTION )
      isGeneric_=true;
}

MachPhysObjDemolish::MachPhysObjDemolish( const MachPhysObjDemolish& copyMe, W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( copyMe, pParent, localTransform )
{

	isGeneric_=copyMe.isGeneric_;

    TEST_INVARIANT;
}

MachPhysObjDemolish::MachPhysObjDemolish( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::DemolitionType type )
:   W4dComposite( exemplar( type ), pParent, localTransform )
{
	isGeneric_=exemplar( type ).isGeneric_;

    TEST_INVARIANT;
}

//static
const MachPhysObjDemolish& MachPhysObjDemolish::exemplar( MachPhys::DemolitionType type )
{
   MachPhysObjDemolish* result = NULL;

//   MachPhysObjDemolish** result;
   if (type.objectType==MachPhys::NOT_CONSTRUCTION)
   {
     result=_CONST_CAST(MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().objDemolishExemplar( type ) );
   }
   else
   {
     switch ( type.constructionType )
     {
	   case MachPhys::HARDWARE_LAB:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().hardwareLabDemolishExemplar( type ) );
		break;
	   case MachPhys::SMELTER:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().smelterDemolishExemplar( type ) );
		break;
	   case MachPhys::FACTORY:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().factoryDemolishExemplar( type ) );
		break;
	   case MachPhys::MISSILE_EMPLACEMENT:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().missileEmplacementDemolishExemplar( type ) );
		break;
	   case MachPhys::GARRISON:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().garrisonDemolishExemplar( type ) );
		break;
	   case MachPhys::MINE:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().mineDemolishExemplar( type ) );
		break;
	   case MachPhys::BEACON:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().beaconDemolishExemplar( type ) );
		break;
	   case MachPhys::POD:
         result=_CONST_CAST( MachPhysObjDemolish*, &MachPhysWeaponPersistence::instance().podDemolishExemplar( type ) );
		break;
       DEFAULT_ASSERT_BAD_CASE( type.constructionType );
	 }
   }
     return *result;
}

//const MachPhysObjDemolish& MachPhysObjDemolish::exemplar( )
//{
//
//    return MachPhysWeaponPersistence::instance().objDemolishExemplar( MachPhys::FACTORY );
//}

MachPhysObjDemolish::~MachPhysObjDemolish()
{
    TEST_INVARIANT;

}

MachPhysObjDemolish::MachPhysObjDemolish( PerConstructor con )
: W4dComposite( con ),
  isGeneric_( false )
{

}

bool MachPhysObjDemolish::useGenericDemolition( MachPhys::DemolitionType type )
{
  SysPathName fileName;

  if ( type.objectType==MachPhys::NOT_CONSTRUCTION )
  {
  	fileName=genericDemolitionFileName;
  }
  else
  {
   switch ( type.constructionType )
   {
	  case MachPhys::HARDWARE_LAB:
        switch ( type.subType )
		{
		  case 0:
		   fileName=civilianHardwareLabDemolitionFileName;
		   break;
		  case 1:
		   fileName=militaryHardwareLabDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::SMELTER:
		fileName=smelterDemolitionFileName;
		break;
	  case MachPhys::FACTORY:
        switch ( type.subType )
		{
		  case 0:
		   fileName=civilianFactoryDemolitionFileName;
		   break;
		  case 1:
		   fileName=militaryFactoryDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::MISSILE_EMPLACEMENT:
        switch ( type.subType )
		{
		  case 0:
		   fileName=turretMissileEmplacementDemolitionFileName;
		   break;
		  case 1:
		   fileName=sentryMissileEmplacementDemolitionFileName;
		   break;
		  case 2:
		   fileName=launcherMissileEmplacementDemolitionFileName;
		   break;
		  case 3:
		   fileName=icbmMissileEmplacementDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::GARRISON:
		fileName=garrisonDemolitionFileName;
		break;
	  case MachPhys::MINE:
		fileName=mineDemolitionFileName;
		break;
	  case MachPhys::BEACON:
		fileName=beaconDemolitionFileName;
		break;
	  case MachPhys::POD:
		fileName=podDemolitionFileName;
		break;
      DEFAULT_ASSERT_BAD_CASE( type.constructionType );
	 }
	// add the hardware level
    string level=levelName;
    char buffer[20];
//	level+=itoa(type.level, buffer, 10);
	sprintf(buffer, "%d", type.level);
	level+=buffer;
    fileName.combine(SysPathName(level));
    fileName.combine(demolishName);

  }

  bool result=not fileName.existsAsFile( );
  return result;
}

SysPathName MachPhysObjDemolish::compositeFileName( MachPhys::DemolitionType type )
{

   SysPathName result;

  if ( type.objectType==MachPhys::NOT_CONSTRUCTION )
  {
  	result=genericDemolitionFileName;
  }
  else
  {
   switch ( type.constructionType )
   {
	  case MachPhys::HARDWARE_LAB:
        switch ( type.subType )
		{
		  case 0:
		   result=civilianHardwareLabDemolitionFileName;
		   break;
		  case 1:
		   result=militaryHardwareLabDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::SMELTER:
		result=smelterDemolitionFileName;
		break;
	  case MachPhys::FACTORY:
        switch ( type.subType )
		{
		  case 0:
		   result=civilianFactoryDemolitionFileName;
		   break;
		  case 1:
		   result=militaryFactoryDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::MISSILE_EMPLACEMENT:
        switch ( type.subType )
		{
		  case 0:
		   result=turretMissileEmplacementDemolitionFileName;
		   break;
		  case 1:
		   result=sentryMissileEmplacementDemolitionFileName;
		   break;
		  case 2:
		   result=launcherMissileEmplacementDemolitionFileName;
		   break;
		  case 3:
		   result=icbmMissileEmplacementDemolitionFileName;
		   break;
          DEFAULT_ASSERT_BAD_CASE( type.subType );
		 }
		break;
	  case MachPhys::GARRISON:
		result=garrisonDemolitionFileName;
		break;
	  case MachPhys::MINE:
		result=mineDemolitionFileName;
		break;
	  case MachPhys::BEACON:
		result=beaconDemolitionFileName;
		break;
	  case MachPhys::POD:
		result=podDemolitionFileName;
		break;
      DEFAULT_ASSERT_BAD_CASE( type.constructionType );
	 }
	// add the hardware level
    string level=levelName;
    char buffer[20];
//	level+=itoa(type.level, buffer, 10);
    sprintf(buffer, "%d", type.level);
    level+=buffer;
    result.combine(SysPathName(level));
    result.combine(demolishName);

  }

   ASSERT(result.existsAsFile( ), "No demolition file");
   return result;
}


void perWrite( PerOstream& ostr, const MachPhysObjDemolish& demolish )
{
    const W4dComposite& base = demolish;

    ostr << base;
	ostr << demolish.isGeneric_;
}

void perRead( PerIstream& istr, MachPhysObjDemolish& demolish )
{
    W4dComposite& base = demolish;

    istr >> base;
	istr >> demolish.isGeneric_;
}

void MachPhysObjDemolish::startDemolition
(
 const PhysAbsoluteTime& startTime,
 const PhysRelativeTime& demolitionDuration,
 MATHEX_SCALAR ObjectToGenericRatio,
 MATHEX_SCALAR objectExpansion
)
{

	W4dCompositePlanPtr demolishPlan;
	if( cycleAnims( &demolishPlan ) )
       plan( *demolishPlan, startTime );


	// the scaling depends on two factors :
	// 1) the final size of the building before this animation is played
	// 2) if the demolition is generic the size ratio between the object being destroyed
	// and the object on which the generic animation has been created and
	MATHEX_SCALAR demolitionScaling=objectExpansion;

	if (isGeneric_)
	{
  	  demolitionScaling*=ObjectToGenericRatio;
	}

	// create a scaling plan  only if we are playing a generic (not object dedicated) demolition
    W4dSimpleUniformScalePlan *pScalePlan = _NEW( W4dSimpleUniformScalePlan( demolitionScaling, demolitionScaling, demolitionDuration ) );
    propogateScalePlan( W4dScalePlanPtr( pScalePlan ), startTime );

	// create a white fading material plan
	const PhysAbsoluteTime whiteToBlackDuration = 0.22;
	const PhysAbsoluteTime blackFadingDuration = 0.23;
	const PhysAbsoluteTime blackFadingOffset = whiteToBlackDuration;

	PRE(demolitionDuration==blackFadingDuration+whiteToBlackDuration);

	static RenMaterial blackMaterial;
	static RenMaterial whiteToBlackMaterial;
    const int reasonableSize = 100;

    static bool firstTime = true;
    if( firstTime )
    {
		whiteToBlackMaterial.diffuse(RenColour::black());
		whiteToBlackMaterial.emissive(RenColour::black());
		blackMaterial.diffuse(RenColour::black());
		blackMaterial.emissive(RenColour::black());
        firstTime = false;
    }

	// create a white to black material plan
    W4dColourPlanData::ScalarVec whiteToBlackLinearTimes;
    	whiteToBlackLinearTimes.reserve(1);
		whiteToBlackLinearTimes.push_back( whiteToBlackDuration );

    W4dColourPlanData::ColourVec whiteToBlackColours;
    	 whiteToBlackColours.reserve(2);
         whiteToBlackColours.push_back(RenColour::white());
         whiteToBlackColours.push_back(RenColour::black());

    W4dColourPlanData* pWhiteToBlackData = _NEW( W4dColourPlanData( whiteToBlackLinearTimes, whiteToBlackColours ) );
    W4dColourPlanDataPtr whiteToBlackDataPtr( pWhiteToBlackData );

    W4dMaterialPlan* pWhiteToBlackPlan =
                    _NEW( W4dSimpleEmColPlan( whiteToBlackMaterial, reasonableSize, whiteToBlackDataPtr, whiteToBlackDuration ) );

    W4dMaterialPlanPtr pWhiteToBlackMaterialPlanPtr( pWhiteToBlackPlan );

	entityPlanForEdit().materialPlan( pWhiteToBlackMaterialPlanPtr, startTime );
    for( W4dEntityIter it( this ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( pWhiteToBlackMaterialPlanPtr, startTime );
	}

	// create a black fading material plan
    PhysLinearScalarPlan::ScalarVec blackLinearTimes;
    	blackLinearTimes.reserve(1);
		blackLinearTimes.push_back( blackFadingDuration );

    PhysLinearScalarPlan::ScalarVec blackScales;
    	blackScales.reserve(2);
        blackScales.push_back(1);
        blackScales.push_back(0);

    PhysLinearScalarPlan* pBlackAlphaPlan = _NEW( PhysLinearScalarPlan( blackLinearTimes, blackScales ) );
    PhysScalarPlanPtr blackAlphaPlanPtr( pBlackAlphaPlan );

    W4dSimpleAlphaPlan* pBlackPlan =
                    _NEW( W4dSimpleAlphaPlan( blackMaterial, reasonableSize, blackAlphaPlanPtr, blackFadingDuration ) );

    W4dMaterialPlanPtr pBlackMaterialPlanPtr( pBlackPlan );

	entityPlanForEdit().materialPlan( pBlackMaterialPlanPtr, startTime+blackFadingOffset );
    for( W4dEntityIter it( this ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().materialPlan( pBlackMaterialPlanPtr, startTime+blackFadingOffset );
	}

}

//virtual
bool MachPhysObjDemolish::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysObjDemolish::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysObjDemolish& t )
{

    o << "MachPhysObjDemolish " << (void*)&t << " start" << std::endl;
    o << "MachPhysObjDemolish " << (void*)&t << " end" << std::endl;

    return o;
}

/* End OBJDEMOL.CPP ************************************************/
