/*
 * M C C O N V O Y . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogMachineConvoy

    Represents a group of machines moving in cooperation with each other.
    This is an implementation class of MachLogMotionSequencer.
    HAS-A list of these machines
*/

#ifndef _MACHLOG_MCCONVOY_HPP
#define _MACHLOG_MCCONVOY_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

//Forward declarations
class MachLogMachineMotionSequencer;
class MachLogMachineConvoyImpl;
template < class T > class ctl_pvector;

//orthodox canonical (revoked)
class MachLogMachineConvoy
{
public:
    //Useful typedefs
    typedef ctl_pvector< MachLogMachineMotionSequencer > Sequencers;

    //dtor.    
    ~MachLogMachineConvoy();

    //Add pMachine to the convoy
    void add( MachLogMachineMotionSequencer* pMachine );

    //Remove pMachine from the convoy
    void remove( MachLogMachineMotionSequencer* pMachine );
    //PRE( &pMachine->convoy() == this )

    //The minimu top speed of all the machines in the convoy
    MATHEX_SCALAR minTopSpeed() const;

    //The collection of machines in the convoy
    const Sequencers& sequencers() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogMachineConvoy& t );

private:
    // Operation deliberately revoked
    MachLogMachineConvoy( const MachLogMachineConvoy& );
    MachLogMachineConvoy& operator =( const MachLogMachineConvoy& );
    bool operator ==( const MachLogMachineConvoy& );

    //This is an implementation class
    friend class MachLogMachineMotionSequencer;

    //Convoy initially consist of single machine, pMachine
    MachLogMachineConvoy( MachLogMachineMotionSequencer* pMachine );
    //PRE( pMachine != NULL )

    //Data members
	MachLogMachineConvoyImpl* pImpl_;
};

#ifdef _INLINE
    #include "machlog/mcconvoy.ipp"
#endif


#endif

/* End MCCONVOY.HPP *************************************************/
