#ifndef __GLMATH_H__
#define __GLMATH_H__

#include <math.h>
#include "mathex/point3d.hpp"
#include <glm/gtc/matrix_transform.hpp>

class MexTransform3d;

// Normalises the vector v
//LPD3DVECTOR D3DVECTORNormalise(LPD3DVECTOR v);
glm::vec3*  GLVecNormalise(glm::vec3* v);

// Calculates cross product of a and b.
//LPD3DVECTOR D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb);
glm::vec3* GLVecCrossProduct(glm::vec3* lpd, glm::vec3* lpa, glm::vec3* lpb);

//LPD3DMATRIX D3DMATRIXTranspose(LPD3DMATRIX d, LPD3DMATRIX a);
glm::mat4* GLMatTranspose(glm::mat4* d, glm::mat4* a);

// Set the rotation part of a matrix such that the vector lpD is the new
// z-axis and lpU is the new y-axis.
//LPD3DMATRIX D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU);
glm::mat4* GLMatSetRotation(glm::mat4* lpM, glm::vec3* lpD, glm::vec3* lpU);

//void multiplyMatrices(const D3DMATRIX& a, const D3DMATRIX& b, D3DMATRIX* result);
//void transformPt(const D3DMATRIX&, MexPoint3d *p, MATHEX_SCALAR* w);
void transformPt(const glm::mat4& mat, MexPoint3d *p, MATHEX_SCALAR* w);
void MexToGLMatrix(const MexTransform3d& mex, glm::mat4& mat);
void GLMatrixToMex(const glm::mat4& mat, MexTransform3d* mex);

#endif // __GLMATH_H__

