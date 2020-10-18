#include "ctl/list.hpp"
#include "ctl/list.ctp"
 
class W4dDomain;

W4dDummyFunction12( void )
{
     static ctl_list<W4dDomain*> dummyDomains;
}
