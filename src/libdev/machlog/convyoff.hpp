/*
 * C O N V Y O F F . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogConvoyOffsets

    Single point of access for getting convoy formation coordinates.
*/

#ifndef _MACHLOG_CONVYOFF_HPP
#define _MACHLOG_CONVYOFF_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"

class MachLogConvoyOffsets
{
public:

	enum ConvoyType
	{
		PATROL_CONVOY,
		KILLER_CONVOY,
		LOCATOR_CONVOY
	};

    ~MachLogConvoyOffsets();
	
	static const MexPoint2d convoyOffset( const ConvoyType&, const size_t& index, const MATHEX_SCALAR& clearence );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogConvoyOffsets& t );

private:
    // Operation deliberately revoked
    MachLogConvoyOffsets( const MachLogConvoyOffsets& );
    MachLogConvoyOffsets& operator =( const MachLogConvoyOffsets& );
    bool operator ==( const MachLogConvoyOffsets& );

    MachLogConvoyOffsets();
};


#endif

/* End CONVYOFF.HPP *************************************************/
