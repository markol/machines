/*
 * D A T A S T R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysDataStream

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_DATASTR_HPP
#define _MACHPHYS_DATASTR_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"

class SysPathName;
template <class T > class ctl_vector;

class MachPhysDataStream
// Canonical form revoked
{
public:
    ~MachPhysDataStream( void );

	static ctl_vector< MATHEX_SCALAR > data(const SysPathName& fileName);

    void CLASS_INVARIANT;

private:
    MachPhysDataStream( void );
    friend ostream& operator <<( ostream& o, const MachPhysDataStream& t );

    MachPhysDataStream( const MachPhysDataStream& );
    MachPhysDataStream& operator =( const MachPhysDataStream& );

};


#endif

/* End DATASTR.HPP **************************************************/
