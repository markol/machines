/*
 * T R E A C H R Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/phys.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/matseqpl.hpp"
#include "world4d/mvececha.hpp"
#include "world4d/mvecccha.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "machphys/machphys.hpp"
#include "machphys/machine.hpp"
#include "machphys/racechng.hpp"
#include "machphys/treachry.hpp"
#include "machphys/trechorb.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "render/colour.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysTreacheryWeapon );

MachPhysTreacheryWeapon::MachPhysTreacheryWeapon
(
    W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting
)
:   MachPhysLinearWeapon( exemplar(), mounting, pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysTreacheryWeapon::MachPhysTreacheryWeapon()
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( "models/weapons/treachry/treachry.cdf" ),
                          MachPhys::ORB_OF_TREACHERY, MachPhys::LEFT )
{
    TEST_INVARIANT;
}

MachPhysTreacheryWeapon::MachPhysTreacheryWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

//static
const MachPhysTreacheryWeapon& MachPhysTreacheryWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().treacheryWeaponExemplar();
}

MachPhysTreacheryWeapon::~MachPhysTreacheryWeapon()
{
    TEST_INVARIANT;

}

void MachPhysTreacheryWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTreacheryWeapon& t )
{

    o << "MachPhysTreacheryWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysTreacheryWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
PhysRelativeTime MachPhysTreacheryWeapon::traitorAnimation
(
    MachPhysMachine* pMachine, const PhysAbsoluteTime& startTime,
    MachPhys::Race oldRace, MachPhys::Race newRace
)
{
    //Get the old and new race composite vector override changers
    const MachPhysRaceChanger& theRaceChanger = MachPhysRaceChanger::instance();
    const W4dCompositeMaterialVecChanger& oldRaceChanger =
        theRaceChanger.changer( *pMachine, oldRace );
    const W4dCompositeMaterialVecChanger& newRaceChanger =
        theRaceChanger.changer( *pMachine, newRace );

    //Set up timing data etc
    const PhysRelativeTime flickerTime = 0.4;
    const uint nRepetitions = 5;

    //Set up a material sequence plan for the machine composite itself
    uint nCompositeLODs = pMachine->nLODs();
    if( nCompositeLODs != 0  and
        oldRaceChanger.hasCompositeChanger()  and  newRaceChanger.hasCompositeChanger() )
    {
        //Get the entity level changers
        const W4dEntityMaterialVecChanger& oldEntityChanger = oldRaceChanger.compositeChanger();
        const W4dEntityMaterialVecChanger& newEntityChanger = newRaceChanger.compositeChanger();

        //Set up a material sequence plan for all defined levels of detail using the material
        //vector overrides defined by the changers
        W4dMaterialSequencePlan* pPlan = NULL;

        for( uint lodId = 0;
             lodId < nCompositeLODs and
                oldEntityChanger.hasOverride( lodId ) and newEntityChanger.hasOverride( lodId );
             ++lodId )
        {


            W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
            materialVecPtrs.reserve( 2 );

            materialVecPtrs.push_back( oldEntityChanger.override( lodId ) );
            materialVecPtrs.push_back( newEntityChanger.override( lodId ) );

            if( lodId == 0 )
                pPlan = _NEW( W4dMaterialSequencePlan( materialVecPtrs, flickerTime,
                                                       nCompositeLODs - 1 ) );
            else
                pPlan->lodPlan( materialVecPtrs, lodId );
        }

        //If we had a plan, apply to the composite entity
        if( pPlan )
        {
            W4dMaterialPlanPtr planPtr( pPlan );
            pMachine->entityPlanForEdit().materialPlan( planPtr, startTime, nRepetitions );
        }
    }

    //Now set up material sequence plans for each link
    const W4dComposite::W4dLinks& links = pMachine->links();
    for( W4dComposite::W4dLinks::const_iterator it = links.begin(); it != links.end(); ++it )
    {
        //Get the link pointer and link id
        W4dLink* pLink = *it;
        W4dLinkId linkId = pLink->id();

        uint nLinkLODs = pLink->nLODs();
        if( nLinkLODs != 0  and
            oldRaceChanger.hasLinkChanger( linkId )  and
            newRaceChanger.hasLinkChanger( linkId ) )
        {
            //Get the entity level changers
            const W4dEntityMaterialVecChanger& oldEntityChanger =
                oldRaceChanger.linkIdChanger( linkId );
            const W4dEntityMaterialVecChanger& newEntityChanger =
                newRaceChanger.linkIdChanger( linkId );

            //Set up a material sequence plan for all defined levels of detail using the material
            //vector overrides defined by the changers
            W4dMaterialSequencePlan* pPlan = NULL;

            for( uint lodId = 0;
                 lodId < nLinkLODs and
                    oldEntityChanger.hasOverride( lodId ) and newEntityChanger.hasOverride( lodId );
                 ++lodId )
            {
                W4dMaterialSequencePlan::MaterialVecPtrs materialVecPtrs;
                materialVecPtrs.reserve( 2 );

                materialVecPtrs.push_back( oldEntityChanger.override( lodId ) );
                materialVecPtrs.push_back( newEntityChanger.override( lodId ) );

                if( lodId == 0 )
                    pPlan = _NEW( W4dMaterialSequencePlan( materialVecPtrs, flickerTime,
                                                             nLinkLODs - 1 ) );
                else
                    pPlan->lodPlan( materialVecPtrs, lodId );
            }

            //If we had a plan, apply to the composite entity
            if( pPlan )
            {
                W4dMaterialPlanPtr planPtr( pPlan );
                pLink->entityPlanForEdit().materialPlan( planPtr, startTime, nRepetitions );
            }
        }
    }

    return flickerTime * nRepetitions;
}

//virtual
MachPhysLinearProjectile* MachPhysTreacheryWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createTreacheryOrb( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysTreacheryOrb* MachPhysTreacheryWeapon::createTreacheryOrb
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    //Use utility function to get launch time for the projectile and its
    //start position relative to pParent.
    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
    PhysAbsoluteTime launchTime =
        launchData( burstStartTime, index, pParent, target, targetOffset,
                    &startTransform, &distance );

    //Create the orb
    MachPhysTreacheryOrb* pOrb =
        _NEW( MachPhysTreacheryOrb( pParent, startTransform ) );

    //get the race of the launcher
    MachPhys::Race race = ( hasMachine() ? machine().race() : MachPhys::RED );

    //Make it fly
    pOrb->beLaunched( launchTime, weaponData(), race );

    return pOrb;
}

//virtual
PhysRelativeTime MachPhysTreacheryWeapon::fire( const PhysAbsoluteTime& startTime, int )
{
	//lighting up the machine/construction
	lighting(RenColour(1.5, 1.5, 0.5), startTime, 8);

    //Play the sound
	W4dSoundManager::instance().play( this, SID_TREACHERY, startTime, 1 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/treach.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return 0.0;
}

void perWrite( PerOstream& ostr, const MachPhysTreacheryWeapon& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysTreacheryWeapon& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

/* End TREACHRY.CPP *************************************************/
