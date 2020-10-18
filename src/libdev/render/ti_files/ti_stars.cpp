// All the RenStars and related classes have their instantiations here.
#define _INSTANTIATE_TEMPLATE_CLASSES

#include "internal/d3d.hpp"

#include "ctl/vector.hpp"

void dummyFunctionStarsInstantiations( void )
{
	static ctl_vector< ctl_vector<D3DLVERTEX> > d3dLVerticesVec;
	static ctl_vector< D3DLVERTEX* > pVertices;
	static ctl_vector<ctl_vector<D3DLVERTEX>*> pSectors;
}