/*
 * M L N O T I F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNotifiable

	Abstract Base Class to be inherited from by any class that wishes to be notified by
	a ConstructionTree object when it has changed.
*/

#ifndef _MACHLOG_MLNOTIF_HPP
#define _MACHLOG_MLNOTIF_HPP

#include "machphys/machphys.hpp"

class MachLogNotifiable
// Canonical form revoked
{
public:
   
   MachLogNotifiable( MachPhys::Race r );
   virtual ~MachLogNotifiable ( ); 
   
   virtual void notifiableBeNotified() = 0;		// pure virtual
   
   void CLASS_INVARIANT;

   MachPhys::Race race()	{ return race_; }

	void changeRace( MachPhys::Race r );
    
private:
    MachLogNotifiable( const MachLogNotifiable& );
    MachLogNotifiable& operator =( const MachLogNotifiable& );
    bool operator ==( const MachLogNotifiable& );
	
   	MachPhys::Race race_;
};


#endif

/* End MLNOTIF.HPP *************************************************/
