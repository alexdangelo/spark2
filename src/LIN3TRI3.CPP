// MAGIC Software
// http://www.cs.unc.edu/~eberly
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

#include "lin3tri3.h"

//---------------------------------------------------------------------------
// Modified by Alex D on Aug 20, 2000
// The filename changed from LineTriangleIntersection to linesegTriIntersect
// Using the defn of a line segment in lincomm.h (was common.h):
//  ( Line is L(t) = b+t*m for any real-valued t, 
//    Line segment has constraint 0 <= t <= 1, b and b+m are end points )
// I've modified this func to add a check for if t is in [0,1]. 
// If there is an intersection and t is between [0,1], then true is returned
// Otherwise false is returned.
//---------------------------------------------------------------------------
bool LinesegTriIntersect (const Line3& line, const Triangle3& tri,
                               Point3& intersect)
{
  const float fTolerance = 1e-06f;

  // Compute plane of triangle, Dot(normal,X-tri.b) = 0 where 'normal' is
  // the plane normal.  If the angle between the line direction and normal
  // is small, then the line is effectively parallel to the triangle.
  Point3 normal = Cross(tri.e0,tri.e1);
  float fDenominator = Dot(normal,line.m);
  float fLLenSqr = Dot(line.m,line.m);
  float fNLenSqr = Dot(normal,normal);
  if ( fDenominator*fDenominator <= fTolerance*fLLenSqr*fNLenSqr )
  {
    // Line and triangle are parallel.  I assume that only transverse
    // intersections are sought.  If the line and triangle are coplanar
    // and they intersect, then you will need to put code here to
    // compute the intersection set.
    return false;
  }

  // The line is X(t) = line.b + t*line.m.  Compute line parameter t for
  // intersection of line and plane of triangle.  Substitute in the plane
  // equation to get Dot(normal,line.b-tri.b) + t*Dot(normal,line.m)
  Point3 kDiff0 = line.b - tri.b;
  float fTime = -Dot(normal,kDiff0)/fDenominator;

  if( fTime < 1.0f && fTime > 0.0f ) // AD Aug 20, 2000
  {                                  // for a line segment, 0 <= t <= 1

    // Find difference of intersection point of line with plane and vertex
    // of triangle.
    Point3 kDiff1 = kDiff0 + fTime*line.m;

    // Compute if intersection point is inside triangle.  Write
    // kDiff1 = s0*E0 + s1*E1 and solve for s0 and s1.
    float fE00 = Dot(tri.e0,tri.e0);
    float fE01 = Dot(tri.e0,tri.e1);
    float fE11 = Dot(tri.e1,tri.e1);
    float fDet = Abs(fE00*fE11-fE01*fE01);  // = |normal|^2 > 0
    float fR0 = Dot(tri.e0,kDiff1);
    float fR1 = Dot(tri.e1,kDiff1);

    float fS0 = fE11*fR0 - fE01*fR1;
    float fS1 = fE00*fR1 - fE01*fR0;

    if ( fS0 >= 0.0 && fS1 >= 0.0 && fS0 + fS1 <= fDet )
    {
      // intersection is inside triangle
      intersect = kDiff1 + tri.b;
      return true;
    }
    else
    {
      // intersection is outside triangle
      return false;
    }
  }
  else // block by AD Aug 20, 2000
  {
    return false; // t was not between [0,1]
  }
}
//---------------------------------------------------------------------------

#if 0
void main ()
{
    // line is (1,1,1) + t*(-1,-1,-1)
    Line3 line;
    line.b.x = 1.0f;
    line.b.y = 1.0f;
    line.b.z = 1.0f;
    line.m.x = -1.0f;
    line.m.y = -1.0f;
    line.m.z = -1.0f;

    // Triangle has vertices (1,0,0), (0,1,0), and (0,0,1), so triangle is
    // (1,0,0) + u*(-1,1,0) + v*(-1,0,1) for 0 <= u, 0 <= v, and u+v <= 1.
    Triangle3 tri;
    tri.b.x = 1.0f;
    tri.b.y = 0.0f;
    tri.b.z = 0.0f;
    tri.e0.x = -1.0f;
    tri.e0.y = 1.0f;
    tri.e0.z = 0.0f;
    tri.e1.x = -1.0f;
    tri.e1.y = 0.0f;
    tri.e1.z = 1.0f;

    Point3 intersect;

    bool bResult = LineTriangleIntersection(line,tri,intersect);
}
#endif
