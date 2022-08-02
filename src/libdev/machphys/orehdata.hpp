/*
 * C O N S D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysOreHolographData
	Special value class for Ore holograph object.

*/

#ifndef _OREHDATA_HPP
#define _OREHDATA_HPP

#include "machphys/objdata.hpp"

class MachPhysOreHolographData : public MachPhysObjectData
{
public:
    //ctor. Defaults to no entrances, pickup points etc.
    //cost is the quantity of building material needed to fully build the construction
    MachPhysOreHolographData();
    ~MachPhysOreHolographData();


    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysOreHolographData& t );

private:
	friend class MachPhysData;
    // Operations deliberately revoked
    MachPhysOreHolographData( const MachPhysOreHolographData& );
    MachPhysOreHolographData& operator =( const MachPhysOreHolographData& );
    bool operator ==( const MachPhysOreHolographData& );


};

#endif

/* End CONSDATA.HPP *************************************************/
