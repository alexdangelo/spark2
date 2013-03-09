// MAGIC Software
// http://www.magic-software.com
//
// This free software is supplied under the following terms:
// 1. You may distribute the original source code to others at no charge.
// 2. You may modify the original source code and distribute it to others at
//    no charge.  The modified code must be documented to indicate that it is
//    not part of the original package.
// 3. You may use this code for non-commercial purposes.  You may also
//    incorporate this code into commercial packages.  However, you may not
//    sell any of your source code which contains my original and/or modified
//    source code (see items 1 and 2).  In such a case, you would need to
//    factor out my code and freely distribute it.
// 4. The original code comes with absolutely no warranty and no guarantee is
//    made that the code is bug-free.

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <math.h>

//---------------------------------------------------------------------------
// Wrapped math functions
//---------------------------------------------------------------------------
extern const float INFINITY;
#ifndef PI  // added by AD Feb 01, 2000
extern const float PI;
#endif
//---------------------------------------------------------------------------
inline float Abs (float x)
{
    return float(fabs(x));
}
//---------------------------------------------------------------------------
inline float ACos (float x)
{
    if ( -1.0f < x )
    {
        if ( x < 1.0f )
            return float(acos(x));
        else
            return 0.0f;
    }
    else
    {
        return PI;
    }
}
//---------------------------------------------------------------------------
inline float ATan2 (float y, float x)
{
    return float(atan2(y,x));
}
//---------------------------------------------------------------------------
inline float Cos (float x)
{
    return float(cos(x));
}
//---------------------------------------------------------------------------
inline float Sign (float x)
{
    return ( x > 0.0f ? +1.0f : ( x < 0.0f ? -1.0f : 0.0f ) );
}
//---------------------------------------------------------------------------
inline float Sin (float x)
{
    return float(sin(x));
}
//---------------------------------------------------------------------------
inline float Sqrt (float x)
{
    return float(sqrt(x));
}
//---------------------------------------------------------------------------
inline float UnitRandom ()
{
    return float(rand())/float(RAND_MAX);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Points in 2D
//---------------------------------------------------------------------------
typedef struct
{
    float x, y;
}
Point2;
//---------------------------------------------------------------------------
inline Point2 operator+ (const Point2& p, const Point2& q)
{
    Point2 add;
    add.x = p.x + q.x;
    add.y = p.y + q.y;
    return add;
}
//---------------------------------------------------------------------------
inline Point2 operator- (const Point2& p, const Point2& q)
{
    Point2 sub;
    sub.x = p.x - q.x;
    sub.y = p.y - q.y;
    return sub;
}
//---------------------------------------------------------------------------
inline Point2 operator* (float t, const Point2& p)
{
    Point2 prod;
    prod.x = t*p.x;
    prod.y = t*p.y;
    return prod;
}
//---------------------------------------------------------------------------
inline Point2 operator* (const Point2& p, float t)
{
    Point2 prod;
    prod.x = t*p.x;
    prod.y = t*p.y;
    return prod;
}
//---------------------------------------------------------------------------
inline Point2 operator- (const Point2& p)
{
    Point2 neg;
    neg.x = -p.x;
    neg.y = -p.y;
    return neg;
}
//---------------------------------------------------------------------------
inline float Dot (const Point2& p, const Point2& q)
{
    return p.x*q.x + p.y*q.y;
}
//---------------------------------------------------------------------------
inline Point2 Perp (const Point2& p)
{
    Point2 perp;
    perp.x = -p.y;
    perp.y = +p.x;
    return perp;
}
//---------------------------------------------------------------------------
inline float Length (const Point2& p)
{
    return float(sqrt(p.x*p.x+p.y*p.y));
}
//---------------------------------------------------------------------------
inline bool Unitize (Point2& p, float tolerance = 1e-05)
{
    float length = float(sqrt(p.x*p.x+p.y*p.y));
    if ( length > tolerance )
    {
        float invLength = 1.0f/length;
        p.x *= invLength;
        p.y *= invLength;
        return true;
    }
    else
    {
        p.x = 0.0f;
        p.y = 0.0f;
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Points in 3D
//---------------------------------------------------------------------------
typedef struct
{
    float x, y, z;
}
Point3;
//---------------------------------------------------------------------------
extern const Point3 PT_INFINITY;
//---------------------------------------------------------------------------
inline Point3 operator+ (const Point3& p, const Point3& q)
{
    Point3 add;
    add.x = p.x + q.x;
    add.y = p.y + q.y;
    add.z = p.z + q.z;
    return add;
}
//---------------------------------------------------------------------------
inline Point3 operator- (const Point3& p, const Point3& q)
{
    Point3 sub;
    sub.x = p.x - q.x;
    sub.y = p.y - q.y;
    sub.z = p.z - q.z;
    return sub;
}
//---------------------------------------------------------------------------
inline Point3 operator* (float t, const Point3& p)
{
    Point3 prod;
    prod.x = t*p.x;
    prod.y = t*p.y;
    prod.z = t*p.z;
    return prod;
}
//---------------------------------------------------------------------------
inline Point3 operator* (const Point3& p, float t)
{
    Point3 prod;
    prod.x = t*p.x;
    prod.y = t*p.y;
    prod.z = t*p.z;
    return prod;
}
//---------------------------------------------------------------------------
inline Point3 operator- (const Point3& p)
{
    Point3 neg;
    neg.x = -p.x;
    neg.y = -p.y;
    neg.z = -p.z;
    return neg;
}
//---------------------------------------------------------------------------
inline float Dot (const Point3& p, const Point3& q)
{
    return p.x*q.x + p.y*q.y + p.z*q.z;
}
//---------------------------------------------------------------------------
inline Point3 Cross (const Point3& p, const Point3& q)
{
    Point3 cross;
    cross.x = p.y*q.z - p.z*q.y;
    cross.y = p.z*q.x - p.x*q.z;
    cross.z = p.x*q.y - p.y*q.x;
    return cross;
}
//---------------------------------------------------------------------------
inline float Length (const Point3& p)
{
    return float(sqrt(p.x*p.x+p.y*p.y+p.z*p.z));
}
//---------------------------------------------------------------------------
inline bool Unitize (Point3& p, float tolerance = 1e-05)
{
    float length = float(sqrt(p.x*p.x+p.y*p.y+p.z*p.z));
    if ( length > tolerance )
    {
        float invLength = 1.0f/length;
        p.x *= invLength;
        p.y *= invLength;
        p.z *= invLength;
        return true;
    }
    else
    {
        p.x = 0.0f;
        p.y = 0.0f;
        p.z = 0.0f;
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Lines in 3D
//---------------------------------------------------------------------------
typedef struct
{
    // Line is L(t) = b+t*m for any real-valued t
    // Ray has constraint t >= 0, b is the origin of the ray
    // Line segment has constraint 0 <= t <= 1, b and b+m are end points

    Point3 b, m;
}
Line3;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Circles in 3D
//---------------------------------------------------------------------------
typedef struct
{
    // Plane containing circle is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Circle within
    // the plane is parameterized by X = C + R*(cos(A)*U + sin(A)*V) where
    // A is an angle in [0,2*pi).

    Point3 U, V, N;  // coordinate system of plane containing circle
    Point3 C;  // center
    float R;  // radius > 0
}
Circle3;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Triangles in 3D
//---------------------------------------------------------------------------
typedef struct
{
    // Triangle points are tri(s,t) = b+s*e0+t*e1 where 0 <= s <= 1,
    // 0 <= t <= 1, and 0 <= s+t <= 1.

    Point3 b, e0, e1;
}
Triangle3;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Parallelograms in 3D
//---------------------------------------------------------------------------
typedef struct
{
    // Rectoid points are rect(s,t) = b+s*e0+t*e1 where 0 <= s <= 1
    // and 0 <= t <= 1.  Could have called this a parallelogram, but
    // I do not like typing long words...

    Point3 b, e0, e1;
}
Rectoid3;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Planes in 3D
//---------------------------------------------------------------------------
typedef struct
{
    // plane is Dot(N,X) = d
    Point3 N;
    float d;
}
Plane3;
//---------------------------------------------------------------------------

#endif
