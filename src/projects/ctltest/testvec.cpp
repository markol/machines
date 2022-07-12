#include "base/base.hpp"

// #include "ctl/vector.hpp"
#include "ctl/fixedvec.hpp"
// #include "ctl/mmvector.hpp"

void	testVector();

int	main( int argc, char* argv[] )
{
    testVector();

	return 0;
}

void    testVector()
{
  
    ctl_fixed_vector< int >    v1( 10 );

    v1[ 0 ] = 10;
    v1[ 1 ] = 20;
}
