#include <math.h>
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "render/internal/glmath.hpp"

/*
 * Normalises the vector v
 */
glm::vec3*
GLVecNormalise(glm::vec3* v)
{
    float vx, vy, vz, inv_mod;
    vx = v->x;
    vy = v->y;
    vz = v->z;
    if ((vx == 0) && (vy == 0) && (vz == 0))
	return v;
    inv_mod = (float)(1.0 / sqrt(vx * vx + vy * vy + vz * vz));
    v->x = vx * inv_mod;
    v->y = vy * inv_mod;
    v->z = vz * inv_mod;
    return v;
}


/*
 * Calculates cross product of a and b.
 */
glm::vec3*
GLVecCrossProduct(glm::vec3* lpd, glm::vec3* lpa, glm::vec3* lpb)
{

    lpd->x = lpa->y * lpb->z - lpa->z * lpb->y;
    lpd->y = lpa->z * lpb->x - lpa->x * lpb->z;
    lpd->z = lpa->x * lpb->y - lpa->y * lpb->x;
    return lpd;
}

// In the M$ version, this is incorrectly named "Invert".  With the data
// they use, an inversion's what they get, but it's not what it does.
glm::mat4*
GLMatTranspose(glm::mat4* d, glm::mat4* a)
{
    (*d)[0][0] = (*a)[0][0];
    (*d)[0][1] = (*a)[1][0];
    (*d)[0][2] = (*a)[2][0];
    (*d)[0][3] = (*a)[0][3];

    (*d)[1][0] = (*a)[0][1];
    (*d)[1][1] = (*a)[1][1];
    (*d)[1][2] = (*a)[2][1];
    (*d)[1][3] = (*a)[1][3];

    (*d)[2][0] = (*a)[0][2];
    (*d)[2][1] = (*a)[1][2];
    (*d)[2][2] = (*a)[2][2];
    (*d)[2][3] = (*a)[2][3];

    (*d)[3][0] = (*a)[0][3];
    (*d)[3][1] = (*a)[1][3];
    (*d)[3][2] = (*a)[2][3];
    (*d)[3][3] = (*a)[3][3];

    return d;
}

/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
glm::mat4*
GLMatSetRotation(glm::mat4* lpM, glm::vec3* lpD, glm::vec3* lpU)
{
    float t;
    glm::vec3 d, u, r;


    // Normalise the direction vector.

    d.x = lpD->x;
    d.y = lpD->y;
    d.z = lpD->z;
    GLVecNormalise(&d);

    u.x = lpU->x;
    u.y = lpU->y;
    u.z = lpU->z;
    // Project u into the plane defined by d and normalise.

    t = u.x * d.x + u.y * d.y + u.z * d.z;
    u.x -= d.x * t;
    u.y -= d.y * t;
    u.z -= d.z * t;
    GLVecNormalise(&u);


     // Calculate the vector pointing along the matrix x axis (in a right
    // handed coordinate system) using cross product.
    GLVecCrossProduct(&r, &u, &d);

    (*lpM)[0][0] = r.x;
    (*lpM)[0][1] = r.y, (*lpM)[0][2] = r.z;
    (*lpM)[1][0] = u.x;
    (*lpM)[1][1] = u.y, (*lpM)[1][2] = u.z;
    (*lpM)[2][0] = d.x;
    (*lpM)[2][1] = d.y;
    (*lpM)[2][2] = d.z;

    return lpM;
}

// If efficiency were critical, we could optimise this by assuming that
// the input value of w==1.  It typcially does for 3D graphics applications.
void transformPt(const glm::mat4& mat, MexPoint3d *p, MATHEX_SCALAR* w)
{
    ASSERT_INFO( mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] );
    ASSERT_INFO( mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] );
    ASSERT_INFO( mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] );
    ASSERT_INFO( mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] );
    ASSERT_INFO( *p );
    ASSERT_INFO( *w );

    MATHEX_SCALAR   newP[3];
	MATHEX_SCALAR   newW;

    newP[0] = p->x()*mat[0][0] + p->y()*mat[1][0] + p->z()*mat[2][0] + (*w) * mat[3][0];
    newP[1] = p->x()*mat[0][1] + p->y()*mat[1][1] + p->z()*mat[2][1] + (*w) * mat[3][1];
    newP[2] = p->x()*mat[0][2] + p->y()*mat[1][2] + p->z()*mat[2][2] + (*w) * mat[3][2];
    newW    = p->x()*mat[0][3] + p->y()*mat[1][3] + p->z()*mat[2][3] + (*w) * mat[3][3];

    p->setPoint( newP[0], newP[1], newP[2] );
	*w = newW;

    ASSERT_INFO( *p );
    ASSERT_INFO( *w );
}

void MexToGLMatrix(const MexTransform3d& mex, glm::mat4& mat)
{
	MexFloatMatrix3d m;
	mex.getTransform(m);
	mat[0][0] = (m[0][0]);
	mat[1][0] = (m[1][0]);
	mat[2][0] = (m[2][0]);
	mat[3][0] = (m[3][0]);

	mat[0][1] = (m[0][1]);
	mat[1][1] = (m[1][1]);
	mat[2][1] = (m[2][1]);
	mat[3][1] = (m[3][1]);

	mat[0][2] = (m[0][2]);
	mat[1][2] = (m[1][2]);
	mat[2][2] = (m[2][2]);
	mat[3][2] = (m[3][2]);

	mat[0][3] = (m[0][3]);
	mat[1][3] = (m[1][3]);
	mat[2][3] = (m[2][3]);
	mat[3][3] = (m[3][3]);
}

void GLMatrixToMex(const glm::mat4& mat, MexTransform3d* mex)
{
	PRE(mex);

	MexFloatMatrix3d m;
	m[0][0] = mat[0][0];
	m[1][0] = mat[1][0];
	m[2][0] = mat[2][0];
	m[3][0] = mat[3][0];

	m[0][1] = mat[0][1];
	m[1][1] = mat[1][1];
	m[2][1] = mat[2][1];
	m[3][1] = mat[3][1];

	m[0][2] = mat[0][2];
	m[1][2] = mat[1][2];
	m[2][2] = mat[2][2];
	m[3][2] = mat[3][2];

	m[0][3] = mat[0][3];
	m[1][3] = mat[1][3];
	m[2][3] = mat[2][3];
	m[3][3] = mat[3][3];

	mex->setTransform(m);
}
