#ifndef SPACE_VECTOR_H
/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include <math.h>

typedef struct
{
    float X, Y, Z;
} v3f;

typedef struct
{
    int X, Y, Z;
} v3i;

#define IsVectorZero(v) ((fpclassify((v)) == FP_ZERO) && (fpclassify((v)) == FP_ZERO) && (fpclassify((v)) == FP_ZERO))


float VectorDotProduct(v3f A, v3f B);
float VectorLength(v3f A);
float VectorDistance(v3f A, v3f B);
v3f VectorNormal(v3f A);
v3f VectorAdd(v3f A, v3f B);
v3f VectorSubtract(v3f A, v3f B);
v3f VectorMultiplyScalar(v3f A, int S);

#define SPACE_VECTOR_H
#endif
