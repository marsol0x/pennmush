/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include <math.h>
#include <float.h>

float VectorDotProduct(v3f A, v3f B)
{
    float Result = 0;
    Result += A.X * B.X;
    Result += A.Y * B.Y;
    Result += A.Z * B.Z;

    return Result;
}

float VectorLength(v3f A)
{
    float Result;
    Result = sqrtf(VectorDotProduct(A, A));

    return Result;
}

v3f VectorNormal(v3f A)
{
    v3f Result = {0, 0, 0};
    float Length = VectorLength(A);

    if (fpclassify(Length) == FP_NORMAL)
    {
        Result.X = A.X / Length;
        Result.Y = A.Y / Length;
        Result.Z = A.Z / Length;
    }

    return Result;
}

v3f VectorAdd(v3f A, v3f B)
{
    v3f Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;

    return Result;
}

v3f VectorSubtract(v3f A, v3f B)
{
    v3f Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;

    return Result;
}

v3f VectorMultiplyScalar(v3f A, int S)
{
    v3f Result;
    Result.X = A.X * S;
    Result.Y = A.Y * S;
    Result.Z = A.Z * S;

    return Result;
}

float VectorDistance(v3f A, v3f B)
{
    float Result;
    v3f SubVector = VectorSubtract(A, B);
    Result = VectorLength(SubVector);

    return Result;
}
