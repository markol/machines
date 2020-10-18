/*
 * S T A T I S T I C S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiStatistics

    ABC for statistics display objects intended for use soley on the statistics screen
*/

#ifndef _MACHGUI_STATISTICS_HPP
#define _MACHGUI_STATISTICS_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"

class MachGuiStatistics 
// Canonical form revoked
{
public:
    MachGuiStatistics( void );
    virtual ~MachGuiStatistics( void );

    void CLASS_INVARIANT;

	virtual	void update() = 0;
	virtual void setStatistics( int, int, int, int ) = 0;

private:
    friend ostream& operator <<( ostream& o, const MachGuiStatistics& t );

    MachGuiStatistics( const MachGuiStatistics& );
    MachGuiStatistics& operator =( const MachGuiStatistics& );
};


#endif

/* End STATISTICS.HPP ***********************************************/
