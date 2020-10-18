/*
 * A V A I L O D S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PedAvailableTileLods

    A brief description of the class should go in here
*/

#ifndef _PLANETED_AVAILODS_HPP
#define _PLANETED_AVAILODS_HPP

#include "base/base.hpp"
#include "system/fileenum.hpp"

class PedAvailableTileLods : public SysFileEnumerator
{
public:
    //  Singleton class
    static PedAvailableTileLods& instance( void );
    
	void initialise( const SysPathName& );
	// POST( not isEmpty() );
	
    void CLASS_INVARIANT;

	const SysPathName& next();
	const SysPathName& prev();
	const SysPathName& current();

private:
    // Operations deliberately revoked
    PedAvailableTileLods( const PedAvailableTileLods& );
    PedAvailableTileLods& operator =( const PedAvailableTileLods& );
    bool operator ==( const PedAvailableTileLods& );

    PedAvailableTileLods( void );
	// POST( lodIndex == 0 );

    ~PedAvailableTileLods( void );

    friend ostream& operator <<( ostream& o, const PedAvailableTileLods& t );

	//Data...
	size_t lodIndex_;
};


#endif

/* End AVAILODS.HPP *************************************************/
