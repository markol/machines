/*
 * O P T S K L O C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"
#include "machphys/constron.hpp"
#include "machphys/consdata.hpp"
#include "machphys/slocdata.hpp"

#include "machlog/oplmine.hpp"
#include "machlog/oprefill.hpp"
#include "machlog/optskdrm.hpp"
#include "machlog/squad.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/administ.hpp"
#include "machlog/construc.hpp"
#include "machlog/strategy.hpp"
#include "machlog/produnit.hpp"
#include "machlog/constron.hpp"
#include "machlog/planet.hpp"
#include "machlog/spy.hpp"
#include "machlog/pod.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogTaskDropLandMineOperation );

MachLogTaskDropLandMineOperation::MachLogTaskDropLandMineOperation( MachLogSquadron * pActor, MATHEX_SCALAR minRange, MATHEX_SCALAR maxRange )
:	MachLogTaskOperation( "TASK_DROPLANDMINE_OPERATION" , MachLogOperation::TASK_DROPLANDMINE_OPERATION ),
	pActor_( pActor ),
	complete_( false ),
	minRange_( minRange ),
	maxRange_( maxRange ),
	startDefined_( false )
{
}

MachLogTaskDropLandMineOperation::MachLogTaskDropLandMineOperation( MachLogSquadron * pActor, MATHEX_SCALAR minRange, MATHEX_SCALAR maxRange, const MexPoint2d& start )
:	MachLogTaskOperation( "TASK_DROPLANDMINE_OPERATION" , MachLogOperation::TASK_DROPLANDMINE_OPERATION ),
	pActor_( pActor ),
	complete_( false ),
	minRange_( minRange ),
	maxRange_( maxRange ),
	start_( start ),
	startDefined_( true )
{
}

MachLogTaskDropLandMineOperation::~MachLogTaskDropLandMineOperation( )
{
}


void MachLogTaskDropLandMineOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTaskDropLandMineOperation" << std::endl; 
}

///////////////////////////////////

bool MachLogTaskDropLandMineOperation::doStart()
{
	return true;
}

PhysRelativeTime MachLogTaskDropLandMineOperation::doUpdate( )
{
	setCompleteState();
	if( complete_ )
		return 60;
    //Check we have a spy locator
	MachLogSpyLocator *pSpyLocator = NULL;
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i!=pActor_->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::SPY_LOCATOR )
				pSpyLocator = &(*i )->asSpyLocator();

    //If not, set longer call back as owing squad will be trying to create a spy.
    if( pSpyLocator == NULL )
    {
        return 20.0;
    }

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i!=pActor_->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::SPY_LOCATOR )
			{
				MachLogSpyLocator* pSpyLocator = &(*i )->asSpyLocator();
				if( pSpyLocator->isIdle() )
				{
					if( pSpyLocator->nMines() == 0 )
						pSpyLocator->newOperation( _NEW( MachLogRefillLandMinesOperation( pSpyLocator ) ) );
					else
					{
						MachLogDropLandMineOperation::Path path;
						MexPoint3d start;

						if( startDefined_ )
							start = start_;
						else
						{
							if( MachLogRaces::instance().pods( pActor_->race() ).size() > 0 )
								start = MachLogRaces::instance().pods( pActor_->race() ).front()->position();
							else
								start = (*i)->position();
						}

						PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
						for( int i = 0; i < pSpyLocator->nMines(); ++i )
						{
							//generate coordinates.
							MATHEX_SCALAR range = MachPhysRandom::randomDouble( minRange_, maxRange_ );
							MATHEX_SCALAR x = MachPhysRandom::randomDouble( -1, 1 );
							MATHEX_SCALAR y = sqrt( 1 - x * x );
							if( MachPhysRandom::randomInt( 0, 10 ) < 5 )
								y *= -1;
							x *= range;
							y *= range;
							MexPoint2d point( start.x() + x, start.y() + y );
							MexPoint2d newPoint;
							MATHEX_SCALAR radius = 5;
							while( not cs.findSpace( point, point, pSpyLocator->highClearence() * 1.5 , radius, pSpyLocator->obstacleFlags(), &newPoint ) )
								radius+=5;
							path.push_back( newPoint );
						}
						pSpyLocator->newOperation( _NEW( MachLogDropLandMineOperation( pSpyLocator, path ) ) );
					}
				}
			}

	return 60;
}

void MachLogTaskDropLandMineOperation::doFinish()
{
}

bool MachLogTaskDropLandMineOperation::doIsFinished() const
{
	return false;
}

bool MachLogTaskDropLandMineOperation::setCompleteState()
{
	complete_ = false;
	return complete_;
}

bool MachLogTaskDropLandMineOperation::doBeInterrupted()
{
	return true;
}

void perWrite( PerOstream& ostr, const MachLogTaskDropLandMineOperation& op )
{
	const MachLogTaskOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
    ostr << op.complete_;
	ostr << op.minRange_;
	ostr << op.maxRange_;
	ostr << op.start_;
	ostr << op.startDefined_;
}

void perRead( PerIstream& istr, MachLogTaskDropLandMineOperation& op )
{
	MachLogTaskOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
    istr >> op.complete_;
	istr >> op.minRange_;
	istr >> op.maxRange_;
	istr >> op.start_;
	istr >> op.startDefined_;
}

MachLogTaskDropLandMineOperation::MachLogTaskDropLandMineOperation( PerConstructor con )
:	MachLogTaskOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
