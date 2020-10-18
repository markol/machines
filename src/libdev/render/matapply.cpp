/*
 * M A T A P P L Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/matapply.hpp"
#include "render/internal/lightbuf.hpp"
#include "render/internal/intmap.hpp"
#include "render/internal/colpack.hpp"
#include "render/internal/vtxdata.hpp"
#include "ctl/mmvector.hpp"

#ifndef _INLINE
    #include "render/internal/matapply.ipp"
#endif

void RenIUnlitMatApp::applyViaIndices
(
	const ctl_min_memory_vector<Ren::VertexIdx>& indices,
	RenILightingBuffer*	liteBuf
) const
{
	RenIVertex*  vertices   = liteBuf->vertices();
	bool*        pApplied   = liteBuf->matAppliedFlags();
	bool*		 pPerVertex = liteBuf->perVertexMatFlags();

	ctl_min_memory_vector<Ren::VertexIdx>::const_iterator it = indices.begin();
	while (it != indices.end())
	{
		if (!perVertexMats_ || !pPerVertex[*it])
		{
			bool& applied = pApplied[*it];
			if (!applied)
			{
				applied = true;
				RenIVertex& vtx = vertices[*it];
				applyToVertex(vtx);

				vtx.color    = packedAmbient_;
				vtx.specular = packedBlack_;
			}
		}
		++it;
	}
}

// Defines a function which iterates over a vector of vertices and applies the
// lighting calculation to each vertex.
void RenIUnlitMatApp::applyDirectToVertices
(
	RenILightingBuffer*		liteBuf,
	size_t					nVertices
) const
{
	RenIVertex*  vertices   = liteBuf->vertices();
	bool*        pApplied   = liteBuf->matAppliedFlags();
	bool*		 pPerVertex = liteBuf->perVertexMatFlags();

	for (size_t i=nVertices; i!=0; --i)
	{
		if (!perVertexMats_ || !(*pPerVertex))
		{
			bool& applied = *pApplied;
			if (!applied)
			{
				applied = true;
				applyToVertex(*vertices);
				vertices->color    = packedAmbient_;
				vertices->specular = packedBlack_;
			}
		}
		++vertices;
		++pPerVertex;
		++pApplied;
	}
}

// Defines a function which iterates over a vector of indices and applies the
// lighting calculation to every vertex referred to by the indices.
#define APPLICATOR_INDICES_FUNCTION(APP_TYPE)					\
void APP_TYPE::applyViaIndices									\
(																\
	const ctl_min_memory_vector<Ren::VertexIdx>& indices,	    \
	RenIFloatLightingBuffer*	liteBuf							\
) const															\
{															    \
	RenIVertex*  vertices   = liteBuf->vertices();				\
	const float* pR		    = liteBuf->rLamberts();				\
	const float* pG		    = liteBuf->gLamberts();				\
	const float* pB		    = liteBuf->bLamberts();				\
	bool*        pApplied   = liteBuf->matAppliedFlags();		\
	bool*        pUnderflow = liteBuf->underflowFlags();		\
	bool*		 pPerVertex = liteBuf->perVertexMatFlags();		\
																\
	ctl_min_memory_vector<Ren::VertexIdx>::const_iterator it = indices.begin();		\
	while (it != indices.end())														\
	{																				\
		if (!perVertexMats_ || !pPerVertex[*it])									\
		{																			\
			bool& applied = pApplied[*it];											\
			if (!applied)															\
			{																		\
				applied = true;														\
																					\
				if (pUnderflow[*it])												\
					vertices[*it].color = packedAmbient_;							\
				else																\
					applyToVertex(vertices[*it], pR[*it], pG[*it], pB[*it]);		\
			}																		\
		}																			\
		++it;																		\
	}																				\
}

// Defines a function which iterates over a vector of vertices and applies the
// lighting calculation to each vertex.
#define APPLICATOR_DIRECT_VERTEX_FUNCTION(APP_TYPE)					\
void APP_TYPE::applyDirectToVertices								\
(																	\
	RenIFloatLightingBuffer*	liteBuf,							\
	size_t					nVertices								\
) const																\
{															    	\
	RenIVertex*  vertices   = liteBuf->vertices();					\
	const float* pR		    = liteBuf->rLamberts();					\
	const float* pG		    = liteBuf->gLamberts();					\
	const float* pB		    = liteBuf->bLamberts();					\
	bool*        pApplied   = liteBuf->matAppliedFlags();			\
	bool*        pUnderflow = liteBuf->underflowFlags();			\
	bool*		 pPerVertex = liteBuf->perVertexMatFlags();			\
																	\
	for (size_t i=nVertices; i!=0; --i)								\
	{																\
		if (!perVertexMats_ || !(*pPerVertex))						\
		{															\
			bool& applied = *pApplied;								\
			if (!applied)											\
			{														\
				applied = true;										\
																	\
				if (*pUnderflow)									\
					vertices->color = packedAmbient_;				\
				else												\
					applyToVertex(*vertices, *pR, *pG, *pB);		\
			}														\
		}															\
		++vertices;													\
		++pR;														\
		++pG;														\
		++pB;														\
		++pPerVertex;												\
		++pApplied;													\
		++pUnderflow;												\
	}																\
}

#define APPLICATOR_FUNCTIONS(T)				\
	APPLICATOR_DIRECT_VERTEX_FUNCTION(T) 	\
	APPLICATOR_INDICES_FUNCTION(T)

APPLICATOR_FUNCTIONS(RenINoOverflowMatApp)
APPLICATOR_FUNCTIONS(RenICheckedMatApp)

/* End MATAPPLY.CPP *************************************************/
