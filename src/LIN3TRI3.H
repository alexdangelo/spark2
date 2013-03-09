#ifndef LIN3TRI3_H
#define LIN3TRI3_H

//#include "common.h"
#include "lincomm.h" // renamed by AD (2000-08-08)

// Compute intersection of line with triangle in 3D.  Return value is
// false if there is no intersection.  Return value is true if there
// is an intersection.  In this case, 'intersect' contains the point
// of intersection.

bool LinesegTriIntersect (const Line3& line, const Triangle3& tri,
                          Point3& intersect);

#endif
