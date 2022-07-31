/*
 * D I S P N O T I . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachLogDispositionChangeNotifiable

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_DISPNOTI_HPP
#define _MACHLOG_DISPNOTI_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

class MachLogDispositionChangeNotifiable
// Canonical form revoked
{
public:
    MachLogDispositionChangeNotifiable();
    virtual ~MachLogDispositionChangeNotifiable();

	//One of these is generated for all disposition changes.
	virtual void notifyGeneralDispositionChange() = 0;
	//one of these is generated for the actual specifics of a race
	virtual void notifySpecificDispositionChange( MachPhys::Race, MachPhys::Race ) = 0;
	//This is called when a class changes disposition to move to enemy or neutral, if they started as ally
	virtual void notifyDispositionChangeToNoneAlly( MachPhys::Race, MachPhys::Race ) = 0;
	//This is called when a class changes disposition to move to ally, if they started as non-ally
	virtual void notifyDispositionChangeToAlly( MachPhys::Race, MachPhys::Race ) = 0;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogDispositionChangeNotifiable& t );

    MachLogDispositionChangeNotifiable( const MachLogDispositionChangeNotifiable& );
    MachLogDispositionChangeNotifiable& operator =( const MachLogDispositionChangeNotifiable& );

};


#endif

/* End DISPNOTI.HPP *************************************************/
