/*
 * O P G O L A B . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

#include "machphys/station.hpp"
#include "machphys/stations.hpp"
#include "machphys/harddata.hpp"
//#include "machphys/softdata.hpp"

#include "machlog/opgolab.hpp"
#include "machlog/technici.hpp"
#include "machlog/inout.hpp"
#include "machlog/races.hpp"
#include "machlog/constron.hpp"
#include "machlog/hwlab.hpp"
//#include "machlog/swlab.hpp"

PER_DEFINE_PERSISTENT( MachLogGotoLabOperation );

MachLogGotoLabOperation::~MachLogGotoLabOperation()
{
}


/* //////////////////////////////////////////////////////////////// */

MachLogGotoLabOperation::MachLogGotoLabOperation( MachLogTechnician * pActor  )
:	MachLogOperation( "GOTO_LAB_OPERATION" , MachLogOperation::GOTO_LAB_OPERATION ),
 	pActor_( pActor ),
	finished_( false )
{
	ASSERT( pActor_->objectType() == MachLog::TECHNICIAN," Wrong object type in machLogGotoLabOperation\n" );
}


void MachLogGotoLabOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogGotoLabOperation " << std::endl;
}

bool MachLogGotoLabOperation::doStart()
{
	return not checkNeedAndDoLeaveOperation( pActor_ );
}

///////////////////////////////////

void MachLogGotoLabOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogGotoLabOperation::doIsFinished() const
{
	return finished_;
}

PhysRelativeTime
MachLogGotoLabOperation::doUpdate( )
{
	//have we got an active sub operation at the moment?
	//HAL_STREAM("(" << pActor_->id() << ") MLEnterAPCOp\n" );
	if( pSubOperation() )
		return 2.0;

	//find the nearest lab with free stations and active research and proceed there...
	MATHEX_SCALAR sqrRange = 1000000000;
	bool found = false;
	if( MachLogRaces::instance().hardwareLabs( pActor_->race() ).size() == 0 )//and
//		MachLogRaces::instance().softwareLabs( pActor_->race() ).size() == 0 )
	{
		finished_ = true;
		return 0.0;
	}
	//default iterator pointing to first hardwareLab
	MachLogConstruction* pCon = NULL;
	MachPhysStation* pActualStation = NULL;

	if( MachLogRaces::instance().hardwareLabs( pActor_->race() ).size() > 0 )
	{
		MachLogRaces::HardwareLabs::iterator j = MachLogRaces::instance().hardwareLabs( pActor_->race() ).begin();

		for( MachLogRaces::HardwareLabs::iterator i = MachLogRaces::instance().hardwareLabs( pActor_->race() ).begin();
			i != MachLogRaces::instance().hardwareLabs( pActor_->race() ).end(); ++i)
				if( pActor_->position().sqrEuclidianDistance( (*i)->position() ) < sqrRange and (*i)->currentResearchQueue().size() )
				{
					MachPhysStation* pStation;
					//MachPhysConstructionData& conData = _STATIC_CAST( MachPhysConstructionData&, (*i)->constructionData() );
					MachPhysConstructionData& conData = _STATIC_CAST( MachPhysConstructionData&, _CONST_CAST( MachPhysConstructionData&, (*i)->constructionData() ) );
					if( conData.stations().freeStation( MachPhysStation::RESEARCH_BAY, &pStation ) )
					{
						j = i;
						pActualStation = pStation;
						found = true;
					}
				}
		if( found )
			pCon = *j;
	}

//No software labs??	if( MachLogRaces::instance().softwareLabs( pActor_->race() ).size() > 0 )
//No software labs??	{
//No software labs??		MachLogRaces::SoftwareLabs::iterator j = MachLogRaces::instance().softwareLabs( pActor_->race() ).begin();
//No software labs??
//No software labs??		for( MachLogRaces::SoftwareLabs::iterator i = MachLogRaces::instance().softwareLabs( pActor_->race() ).begin();
//No software labs??			i != MachLogRaces::instance().softwareLabs( pActor_->race() ).end(); ++i)
//No software labs??				if( pActor_->position().sqrEuclidianDistance( (*i)->position() ) < sqrRange )
//No software labs??				{
//No software labs??					MachPhysStation* pStation;
//No software labs??					MachPhysConstructionData& conData = _STATIC_CAST( MachPhysConstructionData&, (*i)->constructionData() );
//No software labs??					if( conData.stations().freeStation( MachPhysStation::RESEARCH_BAY, &pStation ) )
//No software labs??					{
//No software labs??						j = i;
//No software labs??						pActualStation = pStation;
//No software labs??						found = true;
//No software labs??					}
//No software labs??				}
//No software labs??		if( found )
//No software labs??			pCon = *j;
//No software labs??	}

	subOperation( pActor_, _NEW( MachLogEnterBuildingOperation( pActor_, pCon, pActualStation ) ) );

	finished_ = true;

	return 2.0;
}

//virtual
bool MachLogGotoLabOperation::doBeInterrupted()
{
	return true;
}

/* //////////////////////////////////////////////////////////////// */

ostream& operator <<( ostream& o, const MachLogGotoLabOperation& t )
{

    o << "MachLogGotoLabOperation " << (void*)&t << " start" << std::endl;
    o << "MachLogGotoLabOperation " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogGotoLabOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.finished_;
}

void perRead( PerIstream& istr, MachLogGotoLabOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.finished_;
}

MachLogGotoLabOperation::MachLogGotoLabOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* End OPGOLAB.CPP **************************************************/
