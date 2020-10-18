/*
 * L I G H T D A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/internal/lightdat.hpp"

#include "stdlib/string.hpp"

#include "ctl/list.hpp"
#include "ctl/countptr.hpp"
#include "mathex/transf3d.hpp"
#include "phys/lsclplan.hpp"
#include "phys/lsclplan.hpp"


#include "world4d/entity.hpp"
#include "world4d/composit.hpp"
#include "world4d/light.hpp"
#include "world4d/domain.hpp"
#include "world4d/link.hpp"

PER_DEFINE_PERSISTENT(W4dLightData);

W4dLightData::W4dLightData( const string& dummyMeshName, W4dLOD maxLod ):
	W4dAnimationData( dummyMeshName, maxLod ),
	type_(UNIFORM),
	scope_(LOCAL_LIGHT),
	colour_( RenColour() ),
	position_( MexPoint3d() ),
	direction_( MexVec3( 1, 0, 0) ),
	pComposite_( NULL )
{
	times_.reserve( 10 );
	intensities_.reserve( 10 );

    TEST_INVARIANT;
}

W4dLightData::~W4dLightData()
{
    TEST_INVARIANT;

}

void W4dLightData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dLightData& t )
{

    o << "W4dLightData " << (void*)&t << " start" << std::endl;
    o << "W4dLightData " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dLightData::composite( W4dComposite* pComposite )
{
	PRE(implies(pComposite, pComposite->isComposite()));
	pComposite_ = pComposite;
}

//virtual
void W4dLightData::apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime )
{
	W4dLight* pLight = NULL;
	switch( type_ )
	{
		case POINT:
			W4dPointLight* pPointLight;
			pPointLight = _NEW( W4dPointLight(pEntity, position_, maxRange_) );
			pPointLight->constantAttenuation( attenuations_[0] );
			pPointLight->linearAttenuation( attenuations_[1] );
			pPointLight->quadraticAttenuation( attenuations_[2] );
			pLight = pPointLight;
			break;

		case UNIFORM:
			W4dUniformLight* pUniformLight;
			pUniformLight = _NEW( W4dUniformLight(pEntity, position_, maxRange_) );
			pUniformLight->constantAttenuation( attenuations_[0] );
			pUniformLight->linearAttenuation( attenuations_[1] );
			pUniformLight->quadraticAttenuation( attenuations_[2] );
			pLight = pUniformLight;
			break;

		case DIRECTIONAL:
			W4dDirectionalLight* pDirectionalLight;
			pDirectionalLight = _NEW( W4dDirectionalLight(pEntity, direction_) );
			pLight = pDirectionalLight;
	  		break;
	}

	pLight->colour( colour_ );

	switch( scope_ )
	{
		case GLOBAL_LIGHT:
			pLight->scope( W4dLight::GLOBAL );
			pLight->turnOn();
			break;

		case LOCAL_LIGHT:
			pLight->scope( W4dLight::LOCAL );
			pLight->illuminate( pEntity );
			break;

		case DYNAMIC_LIGHT:
			pLight->scope( W4dLight::DYNAMIC );
			break;

		case DOMAIN_LIGHT:
		{
			pLight->scope( W4dLight::LOCAL );

		    //the list of the domains having an intersect relation with  pEntity
			W4dEntity::W4dDomains domains = pEntity->intersectingDomains();

		    if( domains.size() != 0 )
			{
				W4dEntity::W4dDomains::iterator itDomain = domains.begin();
				while (++itDomain != domains.end() )
				{
					W4dDomain* pDomain = *itDomain;
					pLight->illuminate( pDomain );
				}
			}
			else
			{
				pLight->illuminate( pEntity->pParent() );
			}

			break;
		}
		case DOMAIN_COMPOSITE:
		{
			pLight->scope( W4dLight::LOCAL );

		    //True if this entity is attached to a domain. In this case returns the domain
		    //via ppDomain.
		    //the list of the domains having an intersect relation with  pEntity
			W4dEntity::W4dDomains domains = pEntity->intersectingDomains();

		    if( domains.size() != 0 )
			{
				W4dEntity::W4dDomains::iterator itDomain = domains.begin();
				while (++itDomain != domains.end() )
				{
					W4dDomain* pDomain = *itDomain;
				    const W4dEntity::W4dEntities& entities = (pDomain)->intersectingEntities();
				    for( W4dEntity::W4dEntities::const_iterator it = entities.begin(); it != entities.end(); ++it )
				    {
				        W4dEntity* pEntityInDomain = *it;
	 					pLight->illuminate( pEntityInDomain );
					}
				}
			}
			else
			{
				pLight->illuminate( pEntity->pParent() );
			}
			break;
		}
	}

	if( pComposite_ != NULL  )
	{
		ASSERT(  name() != "", " no dummy light mesh");
		// IAIN_STREAM("Attaching light to dummy mesh \"" << name() << "\"\n");

		W4dLink* pLink = NULL;
		const bool found = pComposite_->findLink(name(), &pLink);
		ASSERT(found, " light dummy Link not found");

//		pLight->attachTo(pLink);
		pComposite_->hold(pLight, pLink, MexTransform3d());
		// IAIN_STREAM("  dummy mesh at " << pLink->globalTransform().position() << "\n");
		// IAIN_STREAM("  light at " << pLight->globalTransform().position() << "\n");
	}

	//intensity plan
	size_t nTimes = times_.size();
	if( nTimes > 0 )
	{
		ASSERT( intensities_.size() - nTimes == 1, "" );

		PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times_, intensities_) );
		PhysScalarPlanPtr intensityPlanPtr = pPlan;
		pLight->intensityPlan(intensityPlanPtr, startTime, 1000000);
	}
}

W4dLightData::W4dLightData( PerConstructor con )
:	W4dAnimationData( con ),
	pComposite_( NULL )
{
}

void perWrite( PerOstream& str, const W4dLightData& t )
{
	const W4dAnimationData& data = t;
	perWrite( str, data );

	str << t.type_;
	str << t.scope_;
	str << t.colour_;
	str << t.position_;
	str << t.direction_;
	str << t.attenuations_[0];
	str << t.attenuations_[1];
	str << t.attenuations_[2];
	str << t.maxRange_;
	str << t.pComposite_;
	str << t.times_;
	str << t.intensities_;
}

void perRead( PerIstream& str, W4dLightData& t )
{
	W4dAnimationData& data = t;
	perRead( str, data );

	str >> t.type_;
	str >> t.scope_;
	str >> t.colour_;
	str >> t.position_;
	str >> t.direction_;
	str >> t.attenuations_[0];
	str >> t.attenuations_[1];
	str >> t.attenuations_[2];
	str >> t.maxRange_;
	str >> t.pComposite_;
	str >> t.times_;
	str >> t.intensities_;
}

void W4dLightData::attenuations( MATHEX_SCALAR a1, MATHEX_SCALAR a2, MATHEX_SCALAR a3  )
{
	attenuations_[0] = a1;
	attenuations_[1] = a2;
	attenuations_[2] = a3;
}

void W4dLightData::scope( const Scope& scope )
{
	scope_ = scope;
}

void W4dLightData::type( const Type& type )
{
	type_ = type;
}

void W4dLightData::direction( const MexVec3& direction )
{
	direction_ = direction;
}

void W4dLightData::position( const MexPoint3d& position )
{
	position_ = position;
}

void W4dLightData::maxRange( MATHEX_SCALAR maxRange  )
{
	maxRange_ = maxRange;
}

void W4dLightData::colour( const RenColour& colour )
{
	colour_ = colour;
}

const W4dLightData::Type& W4dLightData::type() const
{
	return type_;
}

void W4dLightData::times( const ctl_vector< MATHEX_SCALAR >& timeIntervals )
{
	times_ = timeIntervals;
}

void W4dLightData::intensities( const ctl_vector< MATHEX_SCALAR >& intensities )
{
	intensities_ = intensities;
}
/* End LIGHTDAT.CPP ***************************************************/
