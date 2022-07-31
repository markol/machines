/*
 * C T R E E N O T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogConstructionTreeNotifiable

	Abstract Base Class to be inherited from by any class that wishes to be notified by
	a ConstructionTree object when it has changed.
*/

#ifndef _MACHLOG_CTREENOT_HPP
#define _MACHLOG_CTREENOT_HPP

class MachLogConstructionTree;

#include "machphys/machphys.hpp"

class MachLogConstructionTreeNotifiable
// Canonical form revoked
{
public:
   
   MachLogConstructionTreeNotifiable( MachPhys::Race r );
   virtual ~MachLogConstructionTreeNotifiable ( ); 
   
   virtual void constructionBeNotified() = 0;		// pure virtual
   
   void CLASS_INVARIANT;

   MachPhys::Race race()	{ return race_; }
    
private:
    MachLogConstructionTreeNotifiable( const MachLogConstructionTreeNotifiable& );
    MachLogConstructionTreeNotifiable& operator =( const MachLogConstructionTreeNotifiable& );
    bool operator ==( const MachLogConstructionTreeNotifiable& );
	
	MachLogConstructionTree& consTree_;
	
	MachPhys::Race race_;
};


#endif

/* End CTREENOT.HPP *************************************************/
