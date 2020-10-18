/*
 * A R T E F A C T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
//#include "stdlib/memory.hpp"
#include <memory>
#include "ctl/list.hpp"
#include "mathex/point2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/cvexpgon.hpp"
#include "phys/cspace2.hpp"
#include "world4d/domain.hpp"
#include "world4d/manager.hpp"
#include "sim/manager.hpp"
#include "machphys/machphys.hpp"
#include "machphys/artefact.hpp"
#include "machphys/artfdata.hpp"
#include "machphys/weputil.hpp"

#include "machlog/artefact.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"
#include "machlog/planet.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/strategy.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

#ifndef _INLINE
    #include "machlog/artefact.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogArtefact );
PER_DEFINE_PERSISTENT( MachLogArtefactLinkData );

MachLogArtefact::MachLogArtefact( int subType, const MexPoint3d& location, const MexRadians& angle )
:   MachActor(  &MachLogRaces::instance().race( MachPhys::NORACE ), newPhysArtefact( subType, location, angle ),
                MachLog::ARTEFACT ),
    pLinkDatas_( NULL ),
    obstaclePolygonId_( 0 )
{
    //NB pPhysArtefact_ initialised as side effect of newPhysArtefact()

    initialise();
        MachLogRaces::instance().artefactsCollection().push_back( this );

    TEST_INVARIANT;
}

MachLogArtefact::MachLogArtefact( int subType, const MexPoint3d& location, const MexRadians& angle, UtlId withId )
:   MachActor(  &MachLogRaces::instance().race( MachPhys::NORACE ), newPhysArtefact( subType, location, angle ),
                MachLog::ARTEFACT, withId ),
    pLinkDatas_( NULL ),
    obstaclePolygonId_( 0 )
{
    //NB pPhysArtefact_ initialised as side effect of newPhysArtefact()

    initialise();
        MachLogRaces::instance().artefactsCollection().push_back( this );

    TEST_INVARIANT;
}

MachLogArtefact::~MachLogArtefact()
{
    TEST_INVARIANT;

    //Detach from all observees
    if( pLinkDatas_ )
    {
        for( LinkDatas::iterator it = pLinkDatas_->begin(); it != pLinkDatas_->end(); ++it )
            (*it).pLinkedArtefact_->detach( this );

        _DELETE( pLinkDatas_ );
    }

    //Remove obstacle polygon
    PhysConfigSpace2d& configSpace = MachLogPlanet::instance().configSpace();
    configSpace.remove( PhysConfigSpace2d::PolygonId( obstaclePolygonId_ ) );
}

void MachLogArtefact::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogArtefact& t )
{

    o << "MachLogArtefact " << (void*)&t << " start" << std::endl;
    o << "MachLogArtefact " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachLogArtefact::update( const PhysRelativeTime&, MATHEX_SCALAR )
{
    //Update the damage indication
    pPhysArtefact_->updateDamageLevel();

    //Use long update time
    return 60.0;
}

//virtual
PhysRelativeTime MachLogArtefact::beDestroyed()
{
    //Initiate the destruction and get the animation time
    PhysAbsoluteTime nowTime = SimManager::instance().currentTime();
    PhysRelativeTime result = pPhysArtefact_->destroy( nowTime );

    //Set up a dying entity event to destruct the physical model after the animation
    preservePhysicalModel( result );

    //Create some debris with BMU count of the original cost
    dropDebris( nowTime );

    return result;
}

//virtual
void MachLogArtefact::beHit
(
    const int& damage, MachPhys::WeaponType byType,
        MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo
)
{
        HAL_STREAM("(" << id() << ") MachLogArtefact::beHit for damage " << damage << std::endl );
    //Use base class implementation.
    //TBD victim animations for artefacts.
        HAL_STREAM(" callin MA::beHit\n" );
        MachActor::beHit( damage, byType, pByActor, pByDirection, echo );
        if( isDead() )
        {
            if( pLinkDatas_ != NULL )
            {
                for( LinkDatas::iterator it = pLinkDatas_->begin(); it != pLinkDatas_->end(); ++it )
                    damageLinkedArtefact( (*it ) );
            }
        }
        else
    //Do more stuff if survived...
    {
        //Apply an victim animation if appropriate
        if( byType != MachPhys::N_WEAPON_TYPES and pByDirection != NULL )
                MachPhysWeaponUtility::victimAnimation( byType, SimManager::instance().currentTime(), *pByDirection,
                                                        pPhysArtefact_ );

        //Set the damage level
        double hpMax = maximumhp();
        double hpNow = hp();
        if( hpNow < hpMax )
        {
            double damagePercent = ((hpMax - hpNow) / hpMax) * 100.0;
            pPhysArtefact_->damageLevel( damagePercent );
        }
    }
        HAL_STREAM("(" << id() << ") MachLogArtefact::beHit DONE " << std::endl );
}

// virtual
void MachLogArtefact::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
    if( not isDead() )
    {
        MachPhys::ArmourUnits armourLeft = armour();
        MachPhys::ArmourUnits absorb = 0;
        MachPhys::HitPointUnits hpLeft = hp();
        if( damage > armourLeft )
                absorb = armourLeft;
        else
                absorb = damage - 1;
        armour( armourLeft - absorb );
        hp( hpLeft - damage + absorb );

        //Set the damage level
        double hpMax = maximumhp();
        double hpNow = hp();
        if( hpNow < hpMax )
        {
            double damagePercent = ((hpMax - hpNow) / hpMax) * 100.0;
            pPhysArtefact_->damageLevel( damagePercent );
        }

        if( armour() < 0 )
                armour( 0 );
        MachLogMessageBroker::ActorNowDead actorNowDead = MachLogMessageBroker::ACTOR_NOT_DEAD;
        if( hp() <= 0 )
                actorNowDead = MachLogMessageBroker::ACTOR_DEAD;
        if( echo == ECHO and MachLogNetwork::instance().isNetworkGame() )
                MachLogNetwork::instance().messageBroker().sendBeHitMessage( id(), damage, MachPhys::N_WEAPON_TYPES, pByActor, NULL, physicalTimeDelay, MachLogMessageBroker::NO_ANIMATION_HIT, actorNowDead );
        //Oops I'm dead!
        if( hp() <= 0 )
        {
            MATHEX_SCALAR junk = 0.0;
            PhysAbsoluteTime nowTime = SimManager::instance().currentTime();
            if( physicalTimeDelay > 0 )
            {
                MachLogArtefact& mla = asArtefact();
                mla.preservePhysicalModel( physicalTimeDelay );
                mla.dropDebris( nowTime + physicalTimeDelay );
			}

			//remove any outstanding operations.
			strategy().beInterrupted();
			isDead( true );
            //Ensure marker gets removed
            selectionState( MachLog::NOT_SELECTED );
            if( pLinkDatas_ != NULL )
            {
                for( LinkDatas::iterator it = pLinkDatas_->begin(); it != pLinkDatas_->end(); ++it )
                    damageLinkedArtefact( (*it ) );
            }
        }
        if( not isDead() )
		{
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
			doVisualiseSelectionState();
		}
    }
    lastBeHitFrame( W4dManager::instance().frameNumber() );
}


//virtual
const MachPhysObjectData& MachLogArtefact::objectData() const
{
    return pPhysArtefact_->artefactData();
}

//virtual
void MachLogArtefact::doStartExplodingAnimation()
{
        HAL_STREAM("MachLogArtefact::doStartExplodingAnimation\n" );
}

//virtual
void MachLogArtefact::doEndExplodingAnimation()
{
        HAL_STREAM("MachLogArtefact::doEndExplodingAnimation\n" );
}

//virtual
void MachLogArtefact::doVisualiseSelectionState()
{
        //Can't be selected
}

W4dEntity* MachLogArtefact::newPhysArtefact( int subType, const MexPoint3d& location, const MexRadians& zAngle )
{
    //Construct a new artefact entity model
    const MachLogArtefacts& artefacts = MachLogRaces::instance().artefacts();
    W4dEntity* pModel = artefacts.newPhysArtefact( subType, location, zAngle );

    //Construct a new artefact object as side effect, and store the pointer
    pPhysArtefact_ = _NEW( MachPhysArtefact( pModel, artefacts.artefactData( subType ) ) );

    //Return the physical model
    return pModel;
}

//virtual
bool MachLogArtefact::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int )
{
        HAL_STREAM("(" << id() << ") MachLogArtefact::beNotified\n" );
    bool stayObserving = true;

    //Iterate through the collection of damage link datas, looking for a link to artefact pSubject,
    //and erasing those entries
    if( event == W4dSubject::DELETED  and  pLinkDatas_ != NULL )
    {
        LinkDatas::iterator it, nextIt;
        for( nextIt = it = pLinkDatas_->begin(); it != pLinkDatas_->end(); it = nextIt )
        {
            ++nextIt;
            MachLogArtefactLinkData& linkData = *it;
            if( linkData.pLinkedArtefact_ == pSubject )
                pLinkDatas_->erase( it );
        }

        stayObserving = false;
    }
        HAL_STREAM("(" << id() << ") MachLogArtefact::beNotified DONE\n" );

    return stayObserving;
}

//virtual
void MachLogArtefact::domainDeleted( W4dDomain* )
{
}

void MachLogArtefact::damageOnDestroy
(
    MachLogArtefact* pLinkedArtefact, MachLogArtefactLinkData::DamageType hpDamageType, int hpDamage,
    MachLogArtefactLinkData::DamageType armourDamageType, int armourDamage
)
{
    //Check the collection exists
    if( pLinkDatas_ == NULL )
        pLinkDatas_ = _NEW( LinkDatas );

    //Add the new entry to the collection
    pLinkDatas_->push_back( MachLogArtefactLinkData( pLinkedArtefact, (MachLogArtefactLinkData::DamageType)hpDamageType, hpDamage,
                                      (MachLogArtefactLinkData::DamageType)armourDamageType, armourDamage) );

    //Become an observer of the linked artefact in case it gets destroyed first
    pLinkedArtefact->attach( this );
}

void MachLogArtefact::damageLinkedArtefact( const MachLogArtefactLinkData& linkData )
{
    //Get current hp and armour values
    MachLogArtefact* pDamageArtefact = linkData.pLinkedArtefact_;
    MachPhys::HitPointUnits hpOld = pDamageArtefact->hp();
    MachPhys::ArmourUnits armourOld = pDamageArtefact->armour();

    //Compute the new values
    MachPhys::HitPointUnits hpNew;
    MachPhys::ArmourUnits armourNew;

    hpNew = ( linkData.hpDamageType_ == MachLogArtefactLinkData::DAMAGE_ABSOLUTE ? linkData.hpDamage_
                                                        : hpOld - linkData.hpDamage_ );
    if( hpNew < 0 )
        hpNew = 0;
    else if( hpNew > hpOld )
        hpNew = hpOld;

    armourNew = ( linkData.armourDamageType_ == MachLogArtefactLinkData::DAMAGE_ABSOLUTE ? linkData.armourDamage_
                                                                : armourOld - linkData.armourDamage_ );
    if( armourNew < 0 )
        armourNew = 0;
    else if( armourNew > armourOld )
        armourNew = armourOld;

    //Modify the artefact
    pDamageArtefact->setHPAndArmour( hpNew, armourNew );

    //If the artefact has zero hp, damage it again to invoke destruction
        //Do not do this if the thing is already dead - there simply is no point and it also leads to
        //recursive call sequence - if artefact A kills artefact B which kills artefact A etc
    if( hpNew <= 0 and not pDamageArtefact->isDead() )
        pDamageArtefact->beHit( armourNew + 10, MachPhys::N_WEAPON_TYPES, NULL, NULL, ECHO );
}

void MachLogArtefact::addObstaclePolygon()
{
    //Get the physical rep to construct a global polygon boundary
    MexConvexPolygon2d* pPolygon = pPhysArtefact_->newGlobalBoundary();
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

    MATHEX_SCALAR height = pPhysArtefact_->artefactData().obstacleHeight();

    //Add this to the config space
    PhysConfigSpace2d& configSpace = MachLogPlanet::instance().configSpace();
    obstaclePolygonId_ = configSpace.add( polygonAPtr, height, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT ).asScalar();
}

FtlSerialId MachLogArtefact::obstaclePolygonId() const
{
    return PhysConfigSpace2d::PolygonId( obstaclePolygonId_ );
}

int MachLogArtefact::subType() const
{
    return pPhysArtefact_->artefactData().subType();
}

void MachLogArtefact::initialise()
{
    //Initialise damage values
    const MachPhysArtefactData& data = pPhysArtefact_->artefactData();
    hp( data.hitPoints() );
    armour( data.armour() );

    //Add the obstacle to the config space
    addObstaclePolygon();
}

void MachLogArtefact::preservePhysicalModel( const PhysRelativeTime& forTime )
{
        SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, forTime,
                                                                   MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
}

void MachLogArtefact::dropDebris( const PhysAbsoluteTime& )
{
    //get the bounding obstacle polygon, and hence an aligned boundary
    const MexPolygon2d& polygon = MachLogPlanet::instance().configSpace().polygon( obstaclePolygonId() );
    MexAlignedBox2d tempBoundary;
    polygon.boundary( &tempBoundary );

        if( not MachLogNetwork::instance().isNetworkGame() or
            ( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().isNodeLogicalHost() ) )
    {
                int quantity = pPhysArtefact_->artefactData().cost();
				if( quantity > 0 )
					MachLogActorMaker::newLogDebris( MachPhys::NORACE, quantity, position(), tempBoundary );
	}
}


void perWrite( PerOstream& ostr, const MachLogArtefact& actor )
{
        const MachActor& base1 = actor;
        const PhysConfigSpace2d& cspace = MachLogPlanet::instance().configSpace();
        ostr << base1;
    ostr << actor.pLinkDatas_;

        int subType = actor.subType();
        PER_WRITE_RAW_OBJECT( ostr, subType );

        const MexTransform3d& trans = actor.globalTransform();
        PER_WRITE_RAW_OBJECT( ostr, trans );
        const W4dId id = actor.id();
        PER_WRITE_RAW_OBJECT( ostr, id );

        actor.persistenceWriteStrategy( ostr );

}

void perRead( PerIstream& istr, MachLogArtefact& actor )
{
        MachActor& base1 = actor;

        istr >> base1;
        istr >> actor.pLinkDatas_;
        if( actor.pLinkDatas_ )
                for( MachLogArtefact::LinkDatas::iterator i = actor.pLinkDatas_->begin(); i != actor.pLinkDatas_->end(); ++i )
                        (*i).pLinkedArtefact_->attach( &actor );

        int subType;
        PER_READ_RAW_OBJECT( istr, subType );

        MexTransform3d trans;
        W4dId id;
        PER_READ_RAW_OBJECT( istr, trans );
        PER_READ_RAW_OBJECT( istr, id );

        W4dEntity* pEntity = actor.newPhysArtefact( subType, trans.position(), 0 );
        actor.setObjectPtr( pEntity, trans );
        actor.id( id );
        actor.addObstaclePolygon();
        actor.actorCreated();

        actor.persistenceReadStrategy( istr );
}

MachLogArtefact::MachLogArtefact( PerConstructor con )
:       MachActor( con )
{
}

void perWrite( PerOstream& ostr, const MachLogArtefactLinkData& ld )
{
    ostr << ld.pLinkedArtefact_;
    ostr << ld.hpDamageType_;
    ostr << ld.hpDamage_;
    ostr << ld.armourDamageType_;
    ostr << ld.armourDamage_;
}

void perRead( PerIstream& istr, MachLogArtefactLinkData& ld )
{
    istr >> ld.pLinkedArtefact_;
    istr >> ld.hpDamageType_;
    istr >> ld.hpDamage_;
    istr >> ld.armourDamageType_;
    istr >> ld.armourDamage_;
}


// forced to recompile Yueai 12/11/98
/* End ARTEFACT.CPP *************************************************/
