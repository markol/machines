/*
 * A T T A F I L T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/attafilt.hpp"
#include "machlog/races.hpp"
#include "machlog/actor.hpp"

MachLogAttackFilter::MachLogAttackFilter( MachPhys::Race attackerRace, MachLogCanAttack::SoftCoverAccuracy precision )
:   attackerRace_( attackerRace ),
    precision_( precision )
{

    TEST_INVARIANT;
}

MachLogAttackFilter::~MachLogAttackFilter()
{
    TEST_INVARIANT;

}

void MachLogAttackFilter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAttackFilter& t )
{

    o << "MachLogAttackFilter " << (void*)&t << " start" << std::endl;
    o << "MachLogAttackFilter " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachLogAttackFilter::check( const W4dEntity& entity, TreeOption* pOption )
{
    TreeOption option = PROCESS_SUBTREE;

    //Ignore non-solid objects
    bool processTheEntity = entity.solid() == W4dEntity::SOLID;

    if( processTheEntity )
    {
        //Check for an entity with an associated actor id
        UtlId actorId = entity.id();
        if( actorId != 0 )
        {
            MachLogRaces& races = MachLogRaces::instance();
            if( races.actorExists( actorId ) )
            {
                MachActor& actor = races.actor( actorId );

                if( actor.objectIsMachine() )                
                {
					//There's no point in checking the children of machines
					option = SKIP_SUBTREE;

					if( precision_ == MachLogCanAttack::SAME_RACE_OK )
					{
		                //If the actor is an enemy or neutral machine, we can fire anyway, so there's no need to
		                //check it for being in the way.

						MachLogRaces::DispositionToRace disposition = races.dispositionToRace( attackerRace_, actor.race() );
						if( disposition == MachLogRaces::ENEMY or disposition == MachLogRaces::NEUTRAL )
						{
							// ignore this entity - as it's not of our allegiance, we don't give a monkey's if projectiles
							// pass through it en route to the intended target.
							processTheEntity = false;
						}		
					}
				}
				else
				{
					// not a machine. But there are salient properties of certain other entity types.
					
					if( actor.objectIsConstruction() )
					{
						//There's no point in checking the children of constructions
						option = SKIP_SUBTREE;
					}
					else if( actor.objectIsDebris() )
					{
						// Of no consequence to LOS
						processTheEntity = false;	
					}
					else if( actor.objectType() == MachLog::ORE_HOLOGRAPH )
					{
						// Of no consequence to LOS
						processTheEntity = false;	
					}
				}
            }
        }
    }

    *pOption = option;
    return processTheEntity;
}

// Forced recompile 16/2/99 CPS
/* End ATTAFILT.CPP *************************************************/
