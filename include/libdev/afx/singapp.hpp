//
// S I N G A P P . H P P 
// (c) Charybdis Limited, 1996. All Rights Reserved
//

#ifndef _FRAMEWORK_SINGAPP_INCLUDED
#define _FRAMEWORK_SINGAPP_INCLUDED

template < class DERIVED_APP >
class AfxSingletonApp
{
public:
        static DERIVED_APP& instance()
        {
                static DERIVED_APP instance_;
                return instance_;
        }
};

#endif /*_FRAMEWORK_SINGAPP_INCLUDED*/
