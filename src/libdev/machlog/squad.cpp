/*
 * S Q U A D . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "machlog/squad.hpp"
#include "mathex/point2d.hpp"
#include "machlog/internal/squadi.hpp"
#include "ctl/algorith.hpp"
#include "world4d/generic.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/orehdata.hpp"
#include "machlog/machine.hpp"
#include "machlog/administ.hpp"
#include "machlog/races.hpp"
#include "machlog/factory.hpp"
#include "machlog/produnit.hpp"
#include "machlog/strategy.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/planet.hpp"

#include "profiler/stktrace.hpp"

PER_DEFINE_PERSISTENT( MachLogSquadron );

#define CB_MachLogSquadron_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachLogAdministrator*				,pCommander_) \
	CB_DEPIMPL( bool								,hasCommander_) \
	CB_DEPIMPL( MachLogSquadron::Machines			,machines_) \
	CB_DEPIMPL( int									,squadronId_) \
	CB_DEPIMPL( MachLogSquadron::DesiredMachineList	,desiredMachineList_) \
	CB_DEPIMPL( int									,totalDesiredMachines_) \
	CB_DEPIMPL( bool								,setDefCon_ ) \
    CB_DEPIMPL( MachLog::DefCon						,defCon_ ) \
    CB_DEPIMPL( MachLogMachine*                     ,pStrongestMachine_) \
    CB_DEPIMPL( bool                                ,squadronHasChanged_);


MachLogSquadron::MachLogSquadron( MachLogRace* pRace, W4dEntity* pPhysObject, int squadronId )
:   MachActor( pRace, pPhysObject, MachLog::SQUADRON, UtlId( 0 ) ),
	pImpl_( _NEW( MachLogSquadronImpl( squadronId ) ) )
{
	//this assertion is no longer valid for the AI races.
//	ASSERT( squadronId > 0 and squadronId < 11,"Bad ID passed into MLSquadron\n" );
    CB_DEPIMPL( MachLogSquadron::DesiredMachineList	,desiredMachineList_);
    desiredMachineList_.reserve( 10 );

    TEST_INVARIANT;
}

MachLogSquadron::MachLogSquadron( MachLogRace* pRace, W4dEntity* pPhysObject, int squadronId, UtlId withId )
:   MachActor( pRace, pPhysObject, MachLog::SQUADRON, withId ),
	pImpl_( _NEW( MachLogSquadronImpl( squadronId ) ) )
{
	//this assertion is no longer valid for the AI races.
//	ASSERT( squadronId > 0 and squadronId < 11,"Bad ID passed into MLSquadron\n" );
    CB_DEPIMPL( MachLogSquadron::DesiredMachineList	,desiredMachineList_);
    desiredMachineList_.reserve( 10 );

    TEST_INVARIANT;
}

MachLogSquadron::~MachLogSquadron()
{
	releaseAllMachines();
	removeAllDesiredMachines();

    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void MachLogSquadron::removeAllDesiredMachines()
{
	CB_MachLogSquadron_DEPIMPL();
	HAL_STREAM("(" << id() << ") MLSquadron::removeAllDesiredMachines\n" );
	while( desiredMachineList_.size() )
	{
		DesiredMachineData* dmd = desiredMachineList_.front();
		_DELETE( dmd->pProdUnit_ );
		_DELETE( dmd );
		desiredMachineList_.erase( desiredMachineList_.begin() );
	}
	totalDesiredMachines_ = 0;
	HAL_STREAM("(" << id() << ") MLSquadron::removeAllDesiredMachines done\n" );
}

void MachLogSquadron::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

bool MachLogSquadron::addToControl( MachLogMachine* p )
{
	//check size of list against brain level etc...
	//Needs doing.
//	HAL_STREAM(" Administrator (" << id() << ") is gaining machine " << p->id() << " " << p->objectType() << std::endl );
//	HAL_STREAM(" Administrator (" << id() << ") objects.size BEFORE " << objects().size() << std::endl );
	CB_MachLogSquadron_DEPIMPL();
	if( ctl_contains( &machines_, p ) )
	{
		return true;
	}

	if( setDefCon_ )
		p->defCon( defCon_ );

	// note that if this machine is deemed to be the new squadron commander, it'll go to the front of the queue.
	// note that this is a bit grubby as it simply boots the poor previosu leader right to the back. Ho ho ho.

    bool addToEnd = true;
	if( p->objectType() == MachLog::ADMINISTRATOR )
	{
		if( not hasCommander() or p->swLevel() > pCommander_->swLevel() )
		{
			pCommander_ = &p->asAdministrator();
			hasCommander_ = true;

            if( not machines_.empty() )
            {
    			machines_.push_back( machines_.front() );
    			machines_.front() = p;
                addToEnd = false;
            }
        }
	}

    if( addToEnd )
		machines_.push_back( p );

    // This is for get strongest machine
    squadronHasChanged_ = true;

	MachLogProductionUnit prod( p->objectType(), p->subType(), p->hwLevel(), p->swLevel(), MachLogProductionUnit::UNCHECKED_PRIORITY );
	MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;

	if( p->objectIsCanAttack() )
		wc = p->asCanAttack().weaponCombo();
	prod.weaponCombo( wc );
	for( DesiredMachineList::iterator i = desiredMachineList_.begin();
		i != desiredMachineList_.end(); ++i )
		if( *((*i)->pProdUnit_) == prod )
			(*i)->actualNumber_++;
	
//	HAL_STREAM(" Administrator (" << id() << ") objects.size AFTER " << objects().size() << std::endl );
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::SQUADRON_CHANGED );
	return true;

}

/* //////////////////////////////////////////////////////////////// */

template< class CONTAINER, class T >
void
squadron_ctl_erase( CONTAINER *cPtr, const T& t )
{
	cPtr->erase( remove( cPtr->begin(), cPtr->end(), t ), cPtr->end() );
}

void MachLogSquadron::removeFromControl( const MachLogMachine* p )
{
	CB_MachLogSquadron_DEPIMPL();

	if( setDefCon_ )
	{
		MachLogMachine* pMach = _CONST_CAST( MachLogMachine*, p );
		pMach->defCon( MachLogRaces::instance().defCon( p->race() ) );
	}

	HAL_STREAM("MLSquadron::removeFromControl (void*)" << (void*)p << " machine (" );
	HAL_STREAM( p->id() << ")" << std::endl );
	PRE( ctl_contains( &machines_, p ) )
	//Oops commander has just been removed from squadron
	bool checkForNewCommander = false;
	if( p->objectType() == MachLog::ADMINISTRATOR )
		if( hasCommander() and pCommander_->id() == p->id() )
		{
			hasCommander_ = false;
			pCommander_ = NULL;
			checkForNewCommander = true;
		}
	squadron_ctl_erase( &machines_,  p );
	if( checkForNewCommander and machines_.size() > 0 )
	{
		for( Machines::iterator i = machines_.begin(); i != machines_.end(); ++i )
			if( (*i)->objectType() == MachLog::ADMINISTRATOR )
			{
				if( not hasCommander() )
					pCommander_ = &(*i)->asAdministrator();

				else if( p->swLevel() > pCommander_->swLevel() )
					pCommander_ = &(*i)->asAdministrator();
				hasCommander_ = true;
			}
		// okay, if we have a new commander, we have to shove him to the front of the queue.
		if( hasCommander_ )
		{
			MachLogMachine* pCommanderAsMachine = &( pCommander_->asMachine() );
			Machines::iterator i = find( machines_.begin(), machines_.end(), pCommanderAsMachine );
			ASSERT( i != machines_.end(), "No commander found in collection!" );

			(*i) = machines_.front();
			machines_.front() = pCommanderAsMachine;
		}
	}
		
	
	static MachLogProductionUnit prod;
	prod.type( p->objectType() );
	prod.subType( p->subType() );
	prod.hwLevel( p->hwLevel() );
	prod.swLevel( p->swLevel() );
	prod.priority( MachLogProductionUnit::UNCHECKED_PRIORITY );
	MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
	if( p->objectIsCanAttack() )
		wc = p->asCanAttack().weaponCombo();
	prod.weaponCombo( wc );
	for( DesiredMachineList::iterator i = desiredMachineList_.begin();
		i != desiredMachineList_.end(); ++i )
		if( *((*i)->pProdUnit_) == prod )
			(*i)->actualNumber_--;

	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::SQUADRON_CHANGED );

    // This is for get strongest machine
    squadronHasChanged_ = true;
}

void MachLogSquadron::releaseAllMachines()
{
	CB_MachLogSquadron_DEPIMPL();
	HAL_STREAM("MLSquadron::releaseAllMachines\n" );
	while( machines_.size() > 0 )
	{
		(*machines_.begin())->squadron( NULL );
	}

    pStrongestMachine_ = nullptr;
}

int MachLogSquadron::squadronId() const
{
	CB_MachLogSquadron_DEPIMPL();
	return squadronId_;
}
	
bool MachLogSquadron::hasCommander() const
{
	CB_MachLogSquadron_DEPIMPL();
	return hasCommander_;
}

MachLogAdministrator& MachLogSquadron::commander()
{
	CB_MachLogSquadron_DEPIMPL();
	return *pCommander_;
}

const MachLogAdministrator& MachLogSquadron::commander() const
{
	CB_MachLogSquadron_DEPIMPL();
	return *pCommander_;
}

//virtual 
PhysRelativeTime MachLogSquadron::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR junk)
{
	CB_MachLogSquadron_DEPIMPL();
	MachLogProductionUnit	prod;
	if( checkIfMachineMissing( &prod ) )
	{
		if( not MachLogRaces::instance().addMatchingMachineToSquadron( race(), prod, this ) )
		{
			MachLogFactory* mlf;
			if( MachLogRaces::instance().getSuitableFactory( race(), prod, &mlf ) )
				mlf->buildMachineUniqueWithSquadIndex( prod.type(), prod.subType(), prod.hwLevel(), prod.swLevel(), prod.priority(), squadronId() - 1,prod.weaponCombo() );
		}
	}

	PhysRelativeTime interval = MachActor::update( maxCPUTime, junk );
	return interval;
}

void MachLogSquadron::addDesiredMachine( MachLogProductionUnit* pProd, int desiredNumber )
{
	CB_MachLogSquadron_DEPIMPL();
	DesiredMachineData*	dmd = _NEW( DesiredMachineData );
	dmd->pProdUnit_ = pProd;
	dmd->desiredNumber_ = desiredNumber;
	dmd->actualNumber_ = 0;
	desiredMachineList_.push_back( dmd );
	totalDesiredMachines_ += desiredNumber;

    // This is for get strongest machine
    squadronHasChanged_ = true;
}

//virtual
MachPhysObjectData& MachLogSquadron::objectData() const
{
	ASSERT(false,"Called objectData for a squadron object\n" );
	return (MachPhysObjectData&)MachPhysData::instance().oreHolographData();
}

//have to have these...implementation does nothing
//virtual 
void MachLogSquadron::doStartExplodingAnimation()
{
}
//virtual 
void MachLogSquadron::doEndExplodingAnimation()
{
}

//virtual 
void MachLogSquadron::doVisualiseSelectionState()
{
}

//virtual 
PhysRelativeTime MachLogSquadron::beDestroyed()
{
	return 0.0;
}

bool MachLogSquadron::checkIfMachineMissing( MachLogProductionUnit* pProd ) const
{
	CB_MachLogSquadron_DEPIMPL();
	if( totalDesiredMachines_ == 0 or totalDesiredMachines_ == machines_.size() )
		return false;
	bool found = false;
	for( DesiredMachineList::iterator i = desiredMachineList_.begin(); i != desiredMachineList_.end() and not found; ++i )
	{
		const DesiredMachineData& dmd = **i;
		if( dmd.desiredNumber_ > dmd.actualNumber_ )
		{
			pProd->type( dmd.pProdUnit_->type() );
			pProd->subType( dmd.pProdUnit_->subType() );
			pProd->hwLevel( dmd.pProdUnit_->hwLevel() );
			pProd->swLevel( dmd.pProdUnit_->swLevel() );
			pProd->priority( dmd.pProdUnit_->priority() );
			pProd->weaponCombo( dmd.pProdUnit_->weaponCombo() );
			pProd->constructionId( dmd.pProdUnit_->constructionId() );
			found = true;
		} 
	}
	return true;
}

//virtual 
void MachLogSquadron::doOutputOperator( ostream& o ) const
{
	CB_MachLogSquadron_DEPIMPL();
    o << "MachLogSquadron (" << id() << ") squadron id " << squadronId() << std::endl;
	o << " Number of machines controlled " << machines_.size() << " hasCommander " << hasCommander() << std::endl;
	if( machines_.size() > 0 )
	{
		int j = 0;
		for( MachLogSquadron::Machines::const_iterator i = machines().begin(); i != machines().end(); ++i )
		{
			o << " (" << (*i)->id() << ") " << (*i)->objectType();
			if( hasCommander() and commander().id() == (*i)->id() )
				o << " [Commander]\t";
			else
				o << "\t";
			if( ++j % 3 == 0 )
			{
				o << std::endl;
				j = 0;
			}
		}
	}
	o << "Strategy::" << strategy();
}

ostream& operator <<( ostream& o, const MachLogSquadron& t )
{
	t.doOutputOperator( o );
    return o;
}

void perWrite( PerOstream& ostr, const MachLogSquadron& actor )
{
	const MachActor& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;
	actor.persistenceWriteStrategy( ostr );

}

void perRead( PerIstream& istr, MachLogSquadron& actor )
{
	MachActor& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

    W4dGeneric* pPhysObject = _NEW( W4dGeneric( &MachLogPlanet::instance().hiddenRoot(), MexTransform3d() ) );
	actor.setObjectPtr( pPhysObject, MexTransform3d() );
	actor.id( 0 );
	actor.persistenceReadStrategy( istr );

}

MachLogSquadron::MachLogSquadron( PerConstructor con )
:	MachActor( con )
{
}

MachLogSquadron::Machines& MachLogSquadron::machines()
{
	CB_MachLogSquadron_DEPIMPL();
	return machines_;
}

const MachLogSquadron::Machines& MachLogSquadron::machines() const
{
	CB_MachLogSquadron_DEPIMPL();
	return machines_;
}

void MachLogSquadron::manualCommandIssuedToSquadron()
{
	for( MachLogSquadron::Machines::iterator i = machines().begin(); i != machines().end(); ++i )
	{
		(*i)->manualCommandIssued();
	}
}

void MachLogSquadron::autoSetDefCon( MachLog::DefCon defCon )
{
	CB_MachLogSquadron_DEPIMPL();
	setDefCon_ = true;
	defCon_ = defCon;
}

MachLogMachine* MachLogSquadron::getStrongestMachine()
{
    MachLogMachine* strongestMachine = nullptr;
    CB_MachLogSquadron_DEPIMPL();

    if (machines_.size() > 0)
    {
        if (not squadronHasChanged_ and pStrongestMachine_ != nullptr)
        {
            strongestMachine = pStrongestMachine_;
        }
        else if (pStrongestMachine_ == nullptr or squadronHasChanged_ == true)
        {
            int bestStrength = 0;
            int myStrength = 0;
            for (auto machine : machines())
            {
                if (not machine)
                {
                    continue;
                }
                // Initially assign the first machine, so that all-civvie squads don't cause null pointer woes.
                if (strongestMachine == nullptr)
                {
                    strongestMachine = machine;
                }

                myStrength = machine->militaryStrength();
                if (std::max(myStrength, bestStrength) > bestStrength)
                {
                    bestStrength = myStrength;
                    strongestMachine = machine;
                }
            }
        }
    }

    // This bool serves this function
    // ...so that we perform the above iteration sparingly
    squadronHasChanged_ = false;

    return strongestMachine;
}

/* End SQUAD.CPP ****************************************************/
