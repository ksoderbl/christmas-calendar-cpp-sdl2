#ifndef VECTORMATH_H
#define VECTORMATH_H

extern double distance(double v1[3], double v2[3]);
extern double dotprod(double a[3], double b[3]);
extern   void normcrossprod3(double n[3], double v1[3], double v2[3]);
extern   void normcrossprod4(double n[3], double v0[3], double v1[3],
			     double v2[3]);
extern void   normcrossprod3f(GLfloat n[3], GLfloat v1[3], GLfloat v2[3]);
extern void   normcrossprod4f(GLfloat n[3], GLfloat v0[3],
			      GLfloat v1[3], GLfloat v2[3]);
/* vector addition and subtraction */
#define V_ADD(v1, v2, v3) \
(v1)[0] = (v2)[0] + (v3)[0]; \
(v1)[1] = (v2)[1] + (v3)[1]; \
(v1)[2] = (v2)[2] + (v3)[2]

#define V_ADD3(v1, v2, v3, v4) \
(v1)[0] = (v2)[0] + (v3)[0] + (v4)[0]; \
(v1)[1] = (v2)[1] + (v3)[1] + (v4)[1]; \
(v1)[2] = (v2)[2] + (v3)[2] + (v4)[2]

#define V_SUB(v1, v2, v3) \
(v1)[0] = (v2)[0] - (v3)[0]; \
(v1)[1] = (v2)[1] - (v3)[1]; \
(v1)[2] = (v2)[2] - (v3)[2]

#define V_ASSIGN(v1, v2) \
(v1)[0] = (v2)[0]; \
(v1)[1] = (v2)[1]; \
(v1)[2] = (v2)[2]

#define V_NEGATE(v1) \
(v1)[0] = -(v1)[0]; \
(v1)[1] = -(v1)[1]; \
(v1)[2] = -(v1)[2]

#define V_ASSIGN_NEGATION(v1, v2) \
(v1)[0] = -(v2)[0]; \
(v1)[1] = -(v2)[1]; \
(v1)[2] = -(v2)[2]

#define V_CONSTMUL(v1,c) \
(v1)[0] *= (c); \
(v1)[1] *= (c); \
(v1)[2] *= (c)

#define V_CONSTMUL2(v1,v2,c) \
(v1)[0] = (v2)[0] * (c); \
(v1)[1] = (v2)[1] * (c); \
(v1)[2] = (v2)[2] * (c)

#endif

