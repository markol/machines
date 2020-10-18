/*
 * M C C O N V O I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachineConvoyImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_MCCONVOI_HPP
#define _MACHLOG_MCCONVOI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/mcconvoy.hpp"

class MachLogMachineConvoyImpl
// Canonical form revoked
{
public:
    MachLogMachineConvoyImpl( void );
    ~MachLogMachineConvoyImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogMachineConvoyImpl& t );

    MachLogMachineConvoyImpl( const MachLogMachineConvoyImpl& );
    MachLogMachineConvoyImpl& operator =( const MachLogMachineConvoyImpl& );

	friend class MachLogMachineConvoy;
	 
	// Data members...
    MachLogMachineConvoy::Sequencers sequencers_; 	//The motion sequencers of the machines in the convoy
    MATHEX_SCALAR minTopSpeed_ ;// The minimum top speed of all the machines in the convoy
    bool minTopSpeedUpToDate_; 	// True if minTopSpeed_ is correct for the machines
};

#define CB_MachLogMachineConvoy_DEPIMPL() \
    CB_DEPIMPL( MachLogMachineConvoy::Sequencers, sequencers_ ); \
    CB_DEPIMPL( MATHEX_SCALAR, minTopSpeed_ ); \
    CB_DEPIMPL( bool, minTopSpeedUpToDate_ );


#endif

/* End MCCONVOI.HPP *************************************************/
