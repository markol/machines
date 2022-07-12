/*
 * P N D P L A N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PLAN_TYPE

    A brief description of the class should go in here
*/

#ifndef _WORLD4D_PNDPLAN_HPP
#define _WORLD4D_PNDPLAN_HPP

#include "base/base.hpp"

template< class X >
class PLAN_TYPE
{
public:
    PLAN_TYPE();
    ~PLAN_TYPE();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PLAN_TYPE< X >& t );

private:
    // Operation deliberately revoked
    PLAN_TYPE( const PLAN_TYPE< X >& );

    // Operation deliberately revoked
    PLAN_TYPE< X >& operator =( const PLAN_TYPE< X >& );

    // Operation deliberately revoked
    bool operator ==( const PLAN_TYPE< X >& );

};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "world4d/pndplan.ctp"
//#endif


#endif

/* End PNDPLAN.HPP **************************************************/
