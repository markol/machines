/*
 * D O U B L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include "mathex/double.hpp"

PER_DEFINE_PERSISTENT( MexDouble );

void perWrite( PerOstream& ostr, const MexDouble& d )
{
    PER_WRITE_RAW_DATA( ostr, &d.rep_, sizeof( MexDouble::IDouble ) );
}

void perRead( PerIstream& istr, MexDouble& d )
{
    PER_READ_RAW_DATA( istr, &d.rep_, sizeof( MexDouble::IDouble ) );
    //PER_READ_RAW_DATA( istr, &d.rep_, 8 );
}
