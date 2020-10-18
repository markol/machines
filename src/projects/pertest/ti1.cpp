#define _INSTANTIATE_TEMPLATE_CLASSES

#include "base/base.hpp"

#include "pertest/master.hpp"

#include "pertest/tclass1.hpp"

// #include "ctl/countptr.hpp"
// #include "ctl/countptr.ctp"
// #include "ctl/vector.hpp"
// #include "ctl/vector.ctp"
// #include "ctl/mmvector.hpp"
// #include "ctl/mmvector.ctp"
// #include "ctl/fixedvec.hpp"
// #include "ctl/fixedvec.ctp"
// #include "ctl/list.hpp"
// #include "ctl/list.ctp"
// #include "pertest/class8.hpp"

void dummyFunction()
{
// 	static	CtlCountedPtr< int >	        dummy1;
// 	static	CtlConstCountedPtr< int >	    dummy1a;
// 	static	CtlCountedPtr< Class8 >	        dummy2;
// 	static	CtlConstCountedPtr< Class8 >	dummy3;
//     static  ctl_vector< Class8 >            dummy4;
//     static  ctl_vector< int >               dummy5;
//     static  ctl_fixed_vector< int >         dummy6;
//     static  ctl_min_memory_vector< int >    dummy7;
//     static  ctl_list< int >                 dummy8;

    static  Master::TC1a          dummy1;
}


PER_DEFINE_PERSISTENT_T2( TClass1, int, double );
