//
// I N V K A R G S . H P P 
// (c) Charybdis Limited, 1996. All Rights Reserved

#ifndef _AFX_INVKARGS_INCLUDED
#define _AFX_INVKARGS_INCLUDED

//#include <string>
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
// #include "ctl/algorith.hpp"

class AfxInvokeArgs
: public ctl_vector< string >
{
public:
        virtual ~AfxInvokeArgs();
        int argc() const;
        
        int containsFlag(char flag) const;
        double asDouble(size_type index) const;
        double asInt   (size_type index) const;
};

//////////////////////////////////////////////////////////////////////

#endif /*_AFX_INVKARGS_INCLUDED*/
