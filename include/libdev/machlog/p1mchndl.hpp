/*
 * P 1 M C H N D L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLog1stPersonMachineHandler

    Handler for a machine in 1st person
*/

#ifndef _MACHLOG_P1MCHNDL_HPP
#define _MACHLOG_P1MCHNDL_HPP

#include "base/base.hpp"
#include "machlog/p1handlr.hpp"

class MexCircle2d;
class MachPhys1stPersonDriver;
class MachPhys1stPersonMachineDriver;
class MachLogMachine;
class MachLog1stPersonMachineHandlerData;

class MachLog1stPersonMachineHandler : public MachLog1stPersonHandler
// Canonical form revoked
{
public:
    //Controls pMachine. Remote/local status indicated by networkType.
    MachLog1stPersonMachineHandler( MachLogMachine* pMachine, MachLog1stPersonHandler::NetworkType networkType );

    virtual ~MachLog1stPersonMachineHandler();

    void CLASS_INVARIANT;

    ///////////////////////////////////////////////////
    // Inherited from MachLog1stPersonHandler

    //Returns the entity to become the camera parent, and the local transform to give it
    //in pOffsetTransform
    virtual W4dEntity& cameraAttachment( MexTransform3d* pOffsetTransform ) const;
	
	// current scalar speed of physical machine controlled
	MATHEX_SCALAR currentSpeed() const;
    
    ///////////////////////////////////////////////////

private:
    friend ostream& operator <<( ostream& o, const MachLog1stPersonMachineHandler& t );

    ///////////////////////////////////////////////////
    // Inherited from MachLog1stPersonHandler

    //Implements the update of motion etc.
    //Setups are cleared after this callback.
    virtual void doUpdate();

    //Callback to take action on maximum weapon raneg change.
    //this occurs as different weapons are en/disabled
    virtual void doUpdateMaxWeaponRange( MATHEX_SCALAR range );

    ///////////////////////////////////////////////////

    //Construct the physical driver (and store pointer as side effect).
    //Remote/local status indicated by networkType.
    MachPhys1stPersonDriver* pNewPhysDriver( MachLogMachine* pMachine, MachLog1stPersonHandler::NetworkType networkType );

    //Stop the machine from doing its current operation workload
    void stopStrategy();

    //Used to test for validity of position and slide along obstacles
    bool nextPositionOk( MexCircle2d* pCircle );
    
    //Called on first entry to doUpdate()
    void firstUpdateProcessing();

    //revoked
    MachLog1stPersonMachineHandler( const MachLog1stPersonMachineHandler& );
    MachLog1stPersonMachineHandler& operator =( const MachLog1stPersonMachineHandler& );

    //data members
    MachLog1stPersonMachineHandlerData* pData_; //Data implementation object
    MachPhys1stPersonMachineDriver* pDriver_; //Physical driver
};


#endif

/* End P1MCHNDL.HPP *************************************************/
