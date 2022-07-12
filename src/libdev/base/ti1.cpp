#define	_INSTANTIATE_TEMPLATE_CLASSES

#include "base/lessptr.hpp"

#include <string.hpp>
#include <fstream>

#include "ctl/map.hpp"
#include "ctl/set.hpp"
#include "ctl/vector.hpp"
#include "ctl/utility.hpp"

#include "base/persist.hpp"
#include "internal/perolog.hpp"

CTL_QUAD( Data, string, name, double, time, size_t, count, size_t, recursionCount );

void	PerDummyFunction1()
{
	static	ctl_map< size_t, size_t, less< size_t > >	        dummy1;

    static	pair< void**, size_t >                              dummy3;
    static	ctl_vector< pair< void**, size_t > >                dummy4;

	static	ctl_map< string, PerReadFnPtr, less< string > >	    dummy5;
	static	ctl_map< string, PerWriteFnPtr, less< string > >	dummy5a;
    static	ctl_map< string, Data, less< string > >             dummy5b;
    static	ctl_vector< Data >                                  dummy5c;

    static  ctl_set< void*, less_ptr< void > >                  dummy6;
    static  ctl_set< const void*, less_ptr< const void > >      dummy7;
    typedef ctl_set< PersistenceObjectLog, less< PersistenceObjectLog > >     dummy8;
    
    static  ctl_vector< Diag::PFn >                             dummy9;

    static  ctl_set< ofstream*, less_ptr< ofstream >  >         dummy10;
}