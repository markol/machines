/*
 * E X P A N D B I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogExpandingBlastImpl

    Implementation class for the explanding blast ABC.
*/

#ifndef _MACHLOG_EXPANDBI_HPP
#define _MACHLOG_EXPANDBI_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "phys/phys.hpp"
//#include "world4d/world4d.hpp"
#include "ctl/pvector.hpp"

#include "machlog/expandbl.hpp"

class MachActor;
class MachLogRace;


class MachLogExpandingBlastImpl
// Canonical form revoked
{
public:
    MachLogExpandingBlastImpl( 	MachActor* pOwner, 
								MachLogRace* pLogRace, 
								const MexPoint3d& startPosition, 
								MATHEX_SCALAR blastRadius_,
								MachLogExpandingBlast::CanHitAirUnits canHitAirUnits );
								
    virtual ~MachLogExpandingBlastImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogExpandingBlastImpl& t );

    MachLogExpandingBlastImpl( const MachLogExpandingBlastImpl& );
    MachLogExpandingBlastImpl& operator =( const MachLogExpandingBlastImpl& );

	friend class MachLogExpandingBlast; 

	int											nVictims_;	
	MachActor* 									pOwner_;
    MachLogRace* 								pRace_;
    MexPoint3d 									startPosition_;
	bool 										finished_;
	PhysAbsoluteTime							creationTime_;
	MachLogExpandingBlast::CanHitAirUnits 		canHitAirUnits_;
	
	bool										startedFirstWave_;
	bool										startedSecondWave_;
	bool										startedThirdWave_;
	
	MATHEX_SCALAR 								blastRadius_;
	
	MachLogConstruction*						pConstruction_;
	
	MachLogExpandingBlast::Victims				victims_;
};

#define CB_MachLogExpandingBlast_DEPIMPL() \
	CB_DEPIMPL( int											,nVictims_ ); \
	CB_DEPIMPL( MachActor* 									,pOwner_ ); \
    CB_DEPIMPL( MachLogRace* 								,pRace_ ); \
    CB_DEPIMPL( MexPoint3d 									,startPosition_ ); \
	CB_DEPIMPL( bool		 								,finished_ ); \	
	CB_DEPIMPL( bool		 								,startedFirstWave_ ); \	
	CB_DEPIMPL( bool		 								,startedSecondWave_ ); \	
	CB_DEPIMPL( bool		 								,startedThirdWave_ ); \	
	CB_DEPIMPL( MATHEX_SCALAR								,blastRadius_ ); \
	CB_DEPIMPL( PhysAbsoluteTime							,creationTime_ ); \
	CB_DEPIMPL( MachLogExpandingBlast::CanHitAirUnits, 		canHitAirUnits_ ); \
	CB_DEPIMPL( MachLogConstruction*, 						pConstruction_ ); \
	CB_DEPIMPL( MachLogExpandingBlast::Victims				,victims_ );


#endif

/* End VORTBOMI.HPP *************************************************/
