/*
 * E N T Y I M P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "ctl/list.hpp"

#include "mathex/line3d.hpp"
#include "mathex/vec2.hpp"

#include "render/meshinst.hpp"
#include "render/mesh.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"
#include "render/device.hpp"

#include "world4d/root.hpp"
#include "world4d/domain.hpp"
#include "world4d/subject.hpp"
#include "world4d/camera.hpp"
#include "world4d/manager.hpp"
#include "world4d/entscale.hpp"
#include "world4d/lllist.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/internal/animdata.hpp"
#include "world4d/internal/texdata.hpp"
#include "world4d/internal/multitex.hpp"
#include "world4d/internal/uvdata.hpp"
#include "world4d/coldata.hpp"
#include "world4d/internal/multicol.hpp"
#include "world4d/internal/lightdat.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/entity.hpp"
#include "world4d/internal/nolight.hpp"
#include "world4d/internal/entityi.hpp"

#ifndef _INLINE
#include "world4d/internal/entityi.ipp"
#endif

W4dEntityImpl::W4dEntityImpl( W4dEntity* pParent, const W4dTransform3d& newLocalTransform, W4dEntity::Solidity solid )
: localTransform_( newLocalTransform ),
  pParent_( pParent ),
  childList_( NULL ),
  meshes_(_NEW(Meshes)),
  intersectingDomains_( NULL ),
  pHolder_( NULL ),
  pEntityScale_( NULL ),
  pPlan_( NULL ),
  pPlanUpdateTimes_( NULL ),
  pSubject_( NULL ),
  passId_( 0 ),
  timeGlobalTransformLastUpdated_( -1.0 ),
  checkId_( 0 ),
  id_( 0 ),
  boundingVolume_( NULL ),
  solid_( solid ),
  isDomain_( false ),
  ownedByParent_( true ),
  visible_( true ),
  isStationary_( true ),
  notScaled_( true ),
  isComposite_( false ),
  isGarbage_( false ),
  isOwnedByCountedPtr_( false ),
  hasMesh_( false ),
  pLocalLightList_( NULL ),
  name_( NULL ),
  pFilterColour_( NULL ),
  pAnimationDataPtrs_( NULL ),
  pAnimationLightDataPtrs_( NULL ),
  nLinkedSounds_( 0 ),
  doNotLight_(false),
  clientData_( 0 )
{
    PRE( pParent != NULL );
    LOG_CONSTRUCTION;

    meshes_->reserve( 4 );

    TEST_INVARIANT;
}

W4dEntityImpl::W4dEntityImpl( const W4dEntityImpl& copyMe,
							  W4dEntity* pParent,
    						  const W4dTransform3d& newLocalTransform )
: localTransform_( newLocalTransform ),
  pParent_( pParent ),
  childList_( NULL ),
  meshes_(_NEW(Meshes)),
  intersectingDomains_( NULL ),
  pHolder_( NULL ),
  pEntityScale_( NULL ),
  pPlan_( NULL ),
  pPlanUpdateTimes_( NULL ),
  pSubject_( NULL ),
  passId_( copyMe.passId_ ),
  timeGlobalTransformLastUpdated_( -1.0 ),
  checkId_( 0 ),
  id_( 0 ),
  boundingVolume_( NULL ),
  solid_( copyMe.solid_ ),
  isDomain_( copyMe.isDomain_ ),
  ownedByParent_( copyMe.ownedByParent_ ),
  visible_( copyMe.visible_ ),
  isStationary_( copyMe.isStationary_ ),
  notScaled_( true ),
  isComposite_( copyMe.isComposite_ ),
  isGarbage_( false ),
  isOwnedByCountedPtr_( false ),
  hasMesh_( false ),
  pLocalLightList_(NULL),
  name_( NULL ),
  pAnimationDataPtrs_( NULL ),
  pAnimationLightDataPtrs_( NULL ),
  pFilterColour_( NULL ),
  nLinkedSounds_( 0 ),
  doNotLight_(copyMe.doNotLight_),
  clientData_(copyMe.clientData_)
{
    if( copyMe.pFilterColour_ != NULL )
        pFilterColour_ = _NEW( RenColour(*copyMe.pFilterColour_ ) );

    if( copyMe.pAnimationDataPtrs_ != NULL )
        pAnimationDataPtrs_ = _NEW( AnimationDataPtrs( *copyMe.pAnimationDataPtrs_ ) );

    if( copyMe.pAnimationLightDataPtrs_ != NULL )
        pAnimationLightDataPtrs_ = _NEW( AnimationDataPtrs( *copyMe.pAnimationLightDataPtrs_ ) );

    if( copyMe.name_ != NULL )
        name_ = _NEW( string( *copyMe.name_ ) );

    TEST_INVARIANT;
}

W4dEntityImpl::W4dEntityImpl()
: pParent_( NULL ),
  childList_( NULL ),
  meshes_(_NEW(Meshes)),
  intersectingDomains_( NULL ),
  isDomain_( false ),
  isComposite_( false ),
  isGarbage_( false ),
  isOwnedByCountedPtr_( false ),
  hasMesh_( false ),
  solid_( W4dEntity::NOT_SOLID ),
  pHolder_( NULL ),
  pEntityScale_( NULL ),
  pPlan_( NULL ),
  pPlanUpdateTimes_( NULL ),
  pSubject_( NULL ),
  passId_( 0 ),
  timeGlobalTransformLastUpdated_( -1.0 ),
  checkId_( 0 ),
  id_( 0 ),
  boundingVolume_( NULL ),
  ownedByParent_( true ),
  visible_( true ),
  isStationary_( true ),
  notScaled_( true ),
  pLocalLightList_(NULL),
  name_( NULL ),
  pFilterColour_( NULL ),
  pAnimationDataPtrs_( NULL ),
  pAnimationLightDataPtrs_( NULL ),
  nLinkedSounds_( 0 ),
  doNotLight_(false),
  clientData_( 0 )
{
    TEST_INVARIANT;
}

W4dEntityImpl::W4dEntityImpl
(
	W4dEntity* pParent,
	const W4dTransform3d& newLocalTransform,
	W4dEntity::Solidity solid,
	W4dEntity::NotAChild
):
  pParent_( pParent ),
  childList_( NULL ),
  meshes_(_NEW(Meshes)),
  intersectingDomains_( NULL ),
  isDomain_( false ),
  isComposite_( false ),
  isGarbage_( false ),
  isOwnedByCountedPtr_( false ),
  hasMesh_( false ),
  solid_( solid ),
  pHolder_( NULL ),
  pEntityScale_( NULL ),
  pPlan_( NULL ),
  pPlanUpdateTimes_( NULL ),
  pSubject_( NULL ),
  passId_( 0 ),
  timeGlobalTransformLastUpdated_( -1.0 ),
  checkId_( 0 ),
  id_( 0 ),
  boundingVolume_( NULL ),
  ownedByParent_( false ),					// NB: different
  visible_( true ),
  isStationary_( true ),
  notScaled_( true ),
  pLocalLightList_(NULL),
  name_( NULL ),
  pFilterColour_( NULL ),
  pAnimationDataPtrs_( NULL ),
  pAnimationLightDataPtrs_( NULL ),
  nLinkedSounds_( 0 ),
  doNotLight_(false),
  localTransform_( newLocalTransform ),
  clientData_( 0 )
{
    TEST_INVARIANT;
}

W4dEntityImpl::~W4dEntityImpl()
{
    ASSERT_INFO( nLinkedSounds_ );
    ASSERT( nLinkedSounds_ == 0, "W4dEntityImpl dtor still has non-zero sound count" );

	_DELETE( pEntityScale_ );
	_DELETE( pPlan_ );
	_DELETE( pPlanUpdateTimes_ );

	_DELETE(childList_);
	_DELETE(intersectingDomains_);
	_DELETE(boundingVolume_);

	_DELETE(pLocalLightList_);
	_DELETE( name_ );
	_DELETE( pFilterColour_ );
	_DELETE( pAnimationDataPtrs_ );
	_DELETE( pAnimationLightDataPtrs_ );

    for( Meshes::iterator i = meshes_->begin(); i != meshes_->end(); ++i )
    {
  	    _DELETE( (*i).mesh );
    }
	_DELETE(meshes_);
}

void W4dEntityImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void W4dEntityImpl::name(const string& name)
{
    if( name_ == NULL )
        name_ = _NEW( string( name ) );
    else
    	*name_ = name;
}

const string& W4dEntityImpl::name() const
{
	return ( name_ == NULL ? dummyName() : *name_);
}

void W4dEntityImpl::updateHasMeshFlag()
{
    hasMesh_= meshes_->size() >0   and   (*meshes_)[0].mesh;
}

const W4dEntity::W4dEntities& W4dEntityImpl::children() const
{
    //This is used to export an empty child list for entities with no child list allocated
    static W4dEntity::W4dEntities dummyChildList;

        return (childList_ == NULL ? dummyChildList : *childList_);
}

void W4dEntityImpl::addAnimationData( W4dAnimationData* pAnimData )
{
    if( pAnimationDataPtrs_ == NULL )
    {
        pAnimationDataPtrs_ = _NEW( AnimationDataPtrs );
        pAnimationDataPtrs_->reserve( 2 );
    }

	pAnimationDataPtrs_->push_back( AnimationDataPtr(pAnimData) );
}

void W4dEntityImpl::addLightAnimationData( W4dLightData* pAnimData )
{
    if( pAnimationLightDataPtrs_ == NULL )
    {
        pAnimationLightDataPtrs_ = _NEW( AnimationDataPtrs );
        pAnimationLightDataPtrs_->reserve( 2 );
    }

	pAnimationLightDataPtrs_->push_back( AnimationDataPtr(pAnimData) );
}

void W4dEntityImpl::applyLightAnimations( W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const
{
    if( pAnimationLightDataPtrs_ != NULL )
    {
    	size_t nAnimations = pAnimationLightDataPtrs_->size();
    	for(size_t i = 0; i<nAnimations; ++i)
    	{
    		(*pAnimationLightDataPtrs_)[i]->apply(pEntity, startTime);
    	}
    }
}

void W4dEntityImpl::applyNonLightAnimations( W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const
{
	PRE( pEntity->hasMesh() );

    if( pAnimationDataPtrs_ != NULL )
    {
    	size_t nAnimations = pAnimationDataPtrs_->size();
    	for(size_t i = 0; i<nAnimations; ++i)
    	{
    		(*pAnimationDataPtrs_)[i]->apply(pEntity, startTime);
    	}
    }
}

void W4dEntityImpl::applyAllAnimations(W4dEntity* pEntity, const PhysAbsoluteTime& startTime ) const
{
	applyLightAnimations(pEntity, startTime);
	applyNonLightAnimations(pEntity, startTime);
}

PER_DEFINE_PERSISTENT( W4dEntityImpl );

void perWrite( PerOstream& ostr, const W4dEntityImpl& t )
{
    ostr << t.localTransform_;
    ostr << t.globalTransform_;
    ostr << t.pParent_;
    ostr << t.childList_;
    ostr << t.meshes_;

    ostr << t.intersectingDomains_;
    ostr << t.pHolder_;
    ostr << t.pEntityScale_;
	ostr << t.pPlan_;
    ostr << t.pPlanUpdateTimes_;
    ostr << t.pSubject_;
	ostr << t.passId_;
    ostr << t.checkId_;
    ostr << t.id_;
    ostr << t.boundingVolume_;

    int iTemp = (t.solid_ == W4dEntity::SOLID)? 1: 0;
    PER_WRITE_RAW_OBJECT( ostr, iTemp );

    bool    bTemp;
    bTemp = t.isDomain_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.ownedByParent_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.visible_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.isStationary_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.notScaled_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.isComposite_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.isGarbage_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );
    bTemp = t.hasMesh_;
    PER_WRITE_RAW_OBJECT( ostr, bTemp );

	ostr << t.pLocalLightList_;
	ostr << t.name_;
	ostr << t.pAnimationDataPtrs_;
	ostr << t.pAnimationLightDataPtrs_;
	ostr << t.pFilterColour_;
	ostr << t.doNotLight_;
	ostr << t.clientData_;
}

void perRead( PerIstream& istr, W4dEntityImpl& t )
{
    istr >> t.localTransform_;

    istr >> t.globalTransform_;
    istr >> t.pParent_;
    istr >> t.childList_;
    istr >> t.meshes_;

    istr >> t.intersectingDomains_;
    istr >> t.pHolder_;
    istr >> t.pEntityScale_;
	istr >> t.pPlan_;
    istr >> t.pPlanUpdateTimes_;
    istr >> t.pSubject_;
	istr >> t.passId_;
    istr >> t.checkId_;
    istr >> t.id_;
    istr >> t.boundingVolume_;

    int    iTemp;
    PER_READ_RAW_OBJECT( istr, iTemp );
    t.solid_ = (iTemp == 1)? W4dEntity::SOLID: W4dEntity::NOT_SOLID;

    bool    bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.isDomain_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.ownedByParent_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.visible_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.isStationary_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.notScaled_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.isComposite_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.isGarbage_ = bTemp;
    PER_READ_RAW_OBJECT( istr, bTemp );
    t.hasMesh_ = bTemp;

	istr >> t.pLocalLightList_;

    //  This call must be made here to make sure that the entity plan
    //  update times are properly set up
	istr >> t.name_;
	istr >> t.pAnimationDataPtrs_;
	istr >> t.pAnimationLightDataPtrs_;
	istr >> t.pFilterColour_;
	istr >> t.doNotLight_;
	istr >> t.clientData_;
}

void perWrite( PerOstream& ostr, const W4dEntityImpl::MeshData& data )
{
	ostr << data.mesh;
	ostr << data.distance; // This is distance for mesh details
}

void perRead( PerIstream& istr, W4dEntityImpl::MeshData& data )
{
	istr >> data.mesh;
	istr >> data.distance;

	//char val;
	//PER_READ_RAW_OBJECT( istr, val );

	//PER_READ_RAW_OBJECT( istr, val );
	//PER_READ_RAW_OBJECT( istr, val );
}

void perWrite( PerOstream& ostr, const W4dEntityImpl::PlanUpdateTimes& times )
{
    ostr << times.transformTime;
    ostr << times.visibilityTime;
    ostr << times.scaleTime;
    ostr << times.uvTime;
    ostr << times.materialTime;
}

void perRead( PerIstream& istr, W4dEntityImpl::PlanUpdateTimes& times )
{
    istr >> times.transformTime;
    istr >> times.visibilityTime;
    istr >> times.scaleTime;
    istr >> times.uvTime;
    istr >> times.materialTime;
}

void W4dEntityImpl::filterColour( const RenColour& colour )
{
	if( not filterColourIsSet() )
		pFilterColour_ = _NEW( RenColour( colour ) );
	else
		*pFilterColour_ = colour;

	POST( filterColourIsSet() );
}

const RenColour& W4dEntityImpl::filterColour() const
{
	PRE( filterColourIsSet() );
	return *pFilterColour_;
}

bool W4dEntityImpl::filterColourIsSet() const
{
	return pFilterColour_ != NULL;
}

//static
const string& W4dEntityImpl::dummyName()
{
    static string emptyString;
    return emptyString;
}

W4dLocalLightList& W4dEntityImpl::lightListForEdit()
{
        if( pLocalLightList_ == NULL)
                pLocalLightList_ = _NEW(W4dLocalLightList());

        return *pLocalLightList_;
}

W4dEntityImpl::W4dEntityImpl( PerConstructor )
: pParent_( NULL ),
  childList_( NULL ),
  meshes_( NULL ),
  intersectingDomains_( NULL ),
  isDomain_( false ),
  isComposite_( false ),
  isGarbage_( false ),
  isOwnedByCountedPtr_( false ),
  hasMesh_( true ),
  solid_( W4dEntity::NOT_SOLID ),
  pHolder_( NULL ),
  pEntityScale_( NULL ),
  pPlan_( NULL ),
  pPlanUpdateTimes_( NULL ),
  pSubject_( NULL ),
  passId_( 0 ),
  timeGlobalTransformLastUpdated_( -1.0 ),
  checkId_( 0 ),
  id_( 0 ),
  boundingVolume_( NULL ),
  ownedByParent_( true ),
  visible_( true ),
  isStationary_( true ),
  notScaled_( true ),
  pLocalLightList_(NULL),
  name_( NULL ),
  pFilterColour_( NULL ),
  pAnimationDataPtrs_( NULL ),
  pAnimationLightDataPtrs_( NULL ),
  nLinkedSounds_( 0 ),
  doNotLight_(false),
  clientData_( 0 )
{
    TEST_INVARIANT;
}

/* End ENTYIMPL.CPP *************************************************/
