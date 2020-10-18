/*
 * O P L O C A T E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/pvector.hpp"
#include "profiler/profiler.hpp"

#include "mathex/line2d.hpp"
#include "mathex/transf3d.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/glocdata.hpp"
#include "machlog/oplocate.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/planet.hpp"
#include "machlog/minesite.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/spacial.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"

PER_DEFINE_PERSISTENT( MachLogLocateOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogLocateOperation::MachLogLocateOperation( MachLogGeoLocator * pActor, const MexPoint3d& dest )
:	MachLogOperation( "LOCATE_OPERATION", MachLogOperation::LOCATE_OPERATION ),
	pActor_( pActor ),
	dest_( dest ),
	hasExternalPath_( false ),
	currentElement_( 0 )
{
HAL_STREAM("(" << pActor->id() << ") MachLogLocateOperation::CTOR\n" );
}

MachLogLocateOperation::MachLogLocateOperation( MachLogGeoLocator * pActor,  const Path& externalPath )
:	MachLogOperation( "LOCATE_OPERATION", MachLogOperation::LOCATE_OPERATION ),
	pActor_( pActor ),
	externalPath_( externalPath ),
	hasExternalPath_( true ),
	currentElement_( 0 )
{
HAL_STREAM("(" << pActor->id() << ") MachLogLocateOperation::CTOR\n" );
	if( externalPath_.size() == 1 )
	{
		dest_ = externalPath_.front();
		hasExternalPath_ = false;
		while( externalPath_.size() > 0 )
			externalPath_.erase( externalPath_.begin() );
	}
}

MachLogLocateOperation::~MachLogLocateOperation( )
{
HAL_STREAM("(" << pActor_->id() << ") MachLogLocateOperation::DTOR\n" );
	pActor_->isLocating( false );

	while( path_.size() > 0 )
		path_.erase( path_.begin() );
}

void MachLogLocateOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogLocateOperation Dest " << dest_ << " position " << pActor_->position() << std::endl;
	o << " isLocating " << pActor_->isLocating() << std::endl;
	o << " external path " << hasExternalPath_ << std::endl;
	o << " total path size " << path_.size() << std::endl;
	if( path_.size() )
	{
		for( Path::const_iterator i = path_.begin(); i != path_.end() ; ++i )
			o << (*i) << " ";
		o << std::endl;
	}
}

///////////////////////////////////

bool MachLogLocateOperation::doStart()
{
	if( not checkNeedAndDoLeaveOperation( pActor_ ) )
	{
HAL_STREAM("(" << pActor_->id() << ") MLLocateOp::doStart...computing points\n" );
		pActor_->isLocating( true );

        //Check the actual path to tarverse hasn't already been built. If reloaded by persistence
        //it will be.
        if( path_.empty() )
        {
    		if( hasExternalPath_ )
    			buildPathFromExternalPath();
    		else
    		{
    HAL_STREAM(" single point entry...computing anyintersections\n" );
    			path_.push_back( MexPoint2d( pActor_->position() ) );
    			MexPoint2d actorPos = pActor_->position();
    			MexPoint2d destPos = dest_;

                MATHEX_SCALAR range = pActor_->data().scannerRange();
                MATHEX_SCALAR sqrRange = range * range;

                MATHEX_SCALAR actorDestDistance = actorPos.euclidianDistance( destPos );
                bool actorAtDestination = actorDestDistance < 0.01;

    			for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin(); i!=MachLogPlanet::instance().sites().end(); ++i)
    			{
                    if( not (*i)->hasBeenDiscovered() )
                    {
        				MexPoint2d sitePos = (*i)->position();

                        bool addSite = false;

    	    			if( actorAtDestination )
    		    			addSite = actorPos.sqrEuclidianDistance( sitePos ) < sqrRange;
    				    else
    				        addSite = MexLine2d::sqrEuclidianDistance( actorPos, destPos, actorDestDistance, sitePos ) < sqrRange;

    					//verify that location is a valid move point.
    					if( addSite  and  pActor_->motionSeq().targetPositionContainedInSpace( sitePos ) )
    						path_.push_back( sitePos );
                    }
    			}

    			path_.push_back( destPos );
    		}
        }
		return true;
	}

	return false;
}

void MachLogLocateOperation::buildPathFromExternalPath()
{
HAL_STREAM("MLLocateOperation::buildPathFromExternalPath\n" );
HAL_STREAM(" externalPath.size " << externalPath_.size() << std::endl );
	path_.push_back( MexPoint2d( pActor_->position() ) );

	Path::const_iterator k = externalPath_.end();
	--k;
	Path::const_iterator l = externalPath_.begin();
	++l;

	for( Path::const_iterator j = externalPath_.begin(); j != k; ++j, ++l )
	{
		for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin(); i!=MachLogPlanet::instance().sites().end(); ++i)
		{
			MexPoint2d sitePos = (*i)->position();
			if( MexLine2d::sqrEuclidianDistance( (*j), *(l), (*j).euclidianDistance( *(l) ), sitePos )
			< pActor_->data().scannerRange() * pActor_->data().scannerRange() and
			not (*i)->hasBeenDiscovered() )
			{
				bool alreadyIn = false;
				for( Path::iterator n = path_.begin(); n != path_.end() and not alreadyIn; ++n )
					if( *n == sitePos )
						alreadyIn = true;
				if( not alreadyIn )
				{
					//verify that location is a valid move point.
					if( pActor_->motionSeq().targetPositionContainedInSpace( (*i)->position() ) )
						path_.push_back( sitePos );
HAL_STREAM("Going to locate one!!!! " << (*i)->position() << "\n" );
				}
				else
				{
HAL_STREAM(" already there " << sitePos << std::endl );
				}
			}
		}
HAL_STREAM(" adding way point " << (*j) << std::endl );
		path_.push_back( (*j) );
	}
HAL_STREAM(" adding externalPath_.back\n" );
	path_.push_back( externalPath_.back() );
	while( externalPath_.size() > 0 )
		externalPath_.erase( externalPath_.begin() );
}

void MachLogLocateOperation::doFinish()
{
	pActor_->isLocating( false );
}

bool MachLogLocateOperation::doIsFinished() const
{
	bool finished = false;

	if( currentElement_ >= path_.size() )
	{
	        finished = true;
	        // issue voicemail
	        MachLogVoiceMailManager::instance().postNewMail( VID_GEO_SEARCH_COMPLETE, pActor_->id(), pActor_->race() );
	}

	return finished;
}

//virtual
bool MachLogLocateOperation::doBeInterrupted()
{
	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

//virtual
PhysRelativeTime MachLogLocateOperation::doUpdate( )
{
	if( pSubOperation() )
		return 2.0;
	if( MexPoint2d( pActor_->position() ).sqrEuclidianDistance( path_.front() ) < 1 )
	{
		bool found  = false;
		MachPhys::Race race = pActor_->race();
		// has the Locator discovered a site?
		MachLogPlanet::Sites::const_iterator i =  MachLogPlanet::instance().sites().begin();
		for( ; i != MachLogPlanet::instance().sites().end() and not found; ++i )
		{
			MachLogMineralSite& site = (**i);
			found = ( MexPoint2d( pActor_->position() ).sqrEuclidianDistance( MexPoint2d( site.position() ) ) < 1 and not site.hasBeenDiscovered( ) );
			if( found )
			{
				site.beDiscoveredBy( race );
			}
		}
		++currentElement_;
		if( currentElement_ != path_.size() )
		{
			--currentElement_;
			path_.erase( path_.begin() );
			if( not pSubOperation() and not pActor_->motionSeq().hasDestination() )
			{
				while( not pActor_->motionSeq().targetPositionContainedInSpace( path_.front() ) )
				{
					if( path_.size() == 1 )
					{
						MexTransform3d trans( path_.front() );
						MATHEX_SCALAR radius = 0;
						MexPoint2d dest;
						while( not MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, radius, pActor_->highClearence(), &dest ) )
							radius += 20;
						path_.erase( path_.begin() );
						path_.push_back( dest );
					}
					else
					{
						path_.erase( path_.begin() );
					}
				}
				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, MexPoint3d( path_.front() ) ) ) );
				return 4.0;
			}
		}
	}
	return 1.0;
}

void perWrite( PerOstream& ostr, const MachLogLocateOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.dest_;
	ostr << op.path_;
	ostr << op.externalPath_;
	ostr << op.hasExternalPath_;
	ostr << op.currentElement_;
}

void perRead( PerIstream& istr, MachLogLocateOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.dest_;
	istr >> op.path_;
	istr >> op.externalPath_;
	istr >> op.hasExternalPath_;
	istr >> op.currentElement_;
}

MachLogLocateOperation::MachLogLocateOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
