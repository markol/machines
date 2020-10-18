/*
 * M A C H B U R N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"

#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "machphys/machburn.hpp"
#include "machphys/machine.hpp"
#include "machphys/machdata.hpp"
#include "machphys/flames.hpp"
#include "machphys/snddata.hpp"

MachPhysMachineBurning::MachPhysMachineBurning( MachPhysMachine* pMachine )
:   pMachine_( pMachine ),
    endBurningTime_( 0.0 )
{
    //Ensure the flame vector has correct size
    uint nMaxFlames = 5;
	MATHEX_SCALAR flamePosition = 0.75;

	bool isBallista4 = pMachine->machineData().machineType() == MachPhys::AGGRESSOR and
					   pMachine->machineData().subType() == MachPhys::BALLISTA and
					   pMachine->machineData().hwLevel() == 4;

	bool isBrainBox = pMachine->machineData().machineType() == MachPhys::TECHNICIAN and
					   pMachine->machineData().subType() == MachPhys::BRAIN_BOX;

	if( isBallista4 or isBrainBox )
	{
		nMaxFlames = 1;
		flamePosition = 1.1;
	}

	bool isCommander5 = pMachine->machineData().machineType() == MachPhys::AGGRESSOR and
					   pMachine->machineData().subType() == MachPhys::COMMANDER and
					   pMachine->machineData().hwLevel() == 5;

	bool isGeoLocator = pMachine->machineData().machineType() == MachPhys::GEO_LOCATOR;

	if( isCommander5 or isGeoLocator )
		flamePosition = 0.5;

    flames_.reserve( nMaxFlames );

    //Get the largest nMaxFlames links
    Links links = burnLinks( *pMachine, nMaxFlames );

    //Construct a flame attached to each link
    MexTransform3d linkTransform;
    PhysRelativeTime ages = 20000.0;
    PhysAbsoluteTime now = SimManager::instance().currentTime();

    //the links are in decreasing order of size, so we want to work through them
    //in reverse order, to get or flame vector in ascending size order
    while( links.size() != 0 )
    {
        //get the link and its transform
        W4dLink* pLink = links.back();
        links.pop_back();
        pMachine->linkTransform( *pLink, &linkTransform );

        //Get the link's bounding volume. Hence compute centroid and smallest x/y dimension.
        const MexAlignedBox3d& boundary = pLink->boundingVolume();
        const MexPoint3d& p = boundary.minCorner();
        const MexPoint3d& q = boundary.maxCorner();

        MATHEX_SCALAR xDim = q.x() - p.x();
        MATHEX_SCALAR yDim = q.y() - p.y();
        MATHEX_SCALAR size = std::min( xDim, yDim );
        MATHEX_SCALAR depthOffset = 0.0;
 /*
        MexPoint3d centroid( (p.x() + q.x()) * 0.5, (p.y() + q.y()) * 0.5, q.z() + 0.4 * size );

        //Get the centroid position relative to the machine. Use this to construct
        //a flame transform.
        linkTransform.transform( &centroid );
        MexTransform3d flameTransform( centroid );
*/

        MexTransform3d flameTransform( MexPoint3d(0, 0, flamePosition*boundary.zLength() ) );

        //Attach a flame which will burn for several hours
//        MachPhysFlame* pFlame = _NEW( MachPhysFlame( pLink, flameTransform, FLAME_1,
//                                                     size, size*2.0, depthOffset, ages ) );

        MachPhysFlame* pFlame = _NEW( MachPhysFlame( pMachine, flameTransform, FLAME_1,
                                                     size, depthOffset, ages ) );
		pMachine->hold( pFlame, pLink, flameTransform );

        //Start the material plan, but make it invisible until required
        pFlame->startBurning( now );
        pFlame->visible( false );

        //Store the flame pointer
        flames_.push_back( pFlame );
    }

    TEST_INVARIANT;
}

MachPhysMachineBurning::~MachPhysMachineBurning()
{
    TEST_INVARIANT;

    //Delete all the flames
    for( Flames::iterator it = flames_.begin(); it != flames_.end(); ++it )
    {
        _DELETE( *it );
    }
}

void MachPhysMachineBurning::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachineBurning& t )
{

    o << "MachPhysMachineBurning " << (void*)&t << " start" << std::endl;
    o << "MachPhysMachineBurning " << (void*)&t << " end" << std::endl;

    return o;
}

//static
MachPhysMachineBurning::Links MachPhysMachineBurning::burnLinks
(
    const MachPhysMachine& machine, uint nWanted
)
{
    //Create vectors of the right size for the link pointers and the volumes
    Links links;
    links.reserve( nWanted );

    ctl_vector< MATHEX_SCALAR > volumes;
    volumes.reserve( nWanted );

    uint nAdded = 0;

    //Traverse the links of the machine
    const W4dComposite::W4dLinks& mLinks = machine.links();
    for( W4dComposite::W4dLinks::const_iterator it = mLinks.begin() ; it != mLinks.end(); ++it )
    {
        //get the volume of this link
        W4dLink* pLink = *it;
        const MexAlignedBox3d& linkBoundary = pLink->boundingVolume();
        const MexPoint3d& p = linkBoundary.minCorner();
        const MexPoint3d& q = linkBoundary.maxCorner();
        MATHEX_SCALAR volume = (q.x() - p.x()) * (q.y() - p.y()) * (q.z() - p.z());

        //Choose to use this link, it isn't empty, and we don't already have enough
        //that are larger.
        bool useLink = volume != 0.0 and (nAdded < nWanted  or  volume > volumes.back());

        //If using it, insert at appropriate point in the vectors
        if( useLink )
        {
            //Add this link to our vectors unless already at capacity
            if( nAdded != nWanted )
            {
                volumes.push_back( volume );
                links.push_back( pLink );
                ++nAdded;
            }

            //Now do a little bubble sort to get in the right position
            if( nAdded != 1 )
            {
                uint index = nAdded - 1;
                while( index != 0 and volumes[index-1] < volume )
                {
                    volumes[index] = volumes[index-1];
                    links[index] = links[index-1];
                    --index;
                }

                volumes[index] = volume;
                links[index] = pLink;
            }
        }
    }

    return links;
}

void MachPhysMachineBurning::burn
(
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,
    MATHEX_SCALAR percentage
)
{
    PRE( percentage > 0.0 );
    PRE( percentage <= 100.0 );

    //Decide how many links to set fire to.
    uint nFlames = flames_.size();
    uint nToBurn = uint( (percentage / 100.0) * (nFlames + 0.499) );
    if( nToBurn == 0 )
        nToBurn = 1;
    else if( nToBurn > nFlames )
        nToBurn = nFlames;

    //Add visibility plans for these links
    W4dVisibilityPlan* pPlan = _NEW( W4dVisibilityPlan( true ) );
    pPlan->add( false, duration );
    W4dVisibilityPlanPtr planPtr( pPlan );

    if( startTime < endBurningTime_ )
    {
	    for( uint i = 0; i != nToBurn; ++i )
	        flames_[i]->entityPlanForEdit().clearVisibilityPlans( 10 );

		W4dSoundManager::instance().stop( pMachine_);
	}

    for( uint i = 0; i != nToBurn; ++i )
        flames_[i]->entityPlanForEdit().visibilityPlan( planPtr, startTime );

    //Play burning sound
	W4dSoundManager::instance().playForDuration( pMachine_, SID_BURNING,
                                  startTime, duration );

    //Store the time at which burning ceases
    endBurningTime_ = std::max( endBurningTime_, startTime + duration );

}

bool MachPhysMachineBurning::isBurningFinished() const
{
    return SimManager::instance().currentTime() >= endBurningTime_;
}

PER_DEFINE_PERSISTENT( MachPhysMachineBurning );
MachPhysMachineBurning::MachPhysMachineBurning( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysMachineBurning& t )
{
	ostr << t.pMachine_;
	ostr << t.endBurningTime_;
	ostr << t.flames_;
}

void perRead( PerIstream& istr, MachPhysMachineBurning& t )
{
	istr >> t.pMachine_;
	istr >> t.endBurningTime_;
	istr >> t.flames_;
}

/* End MACHBURN.CPP *************************************************/
