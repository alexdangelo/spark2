/////////////////////////////////////////////////////////////////////////////
// Vector.h
// Created: April 7, 2000 - 2:57pm - Alex D'Angelo
// Common vector and matrix resources
/////////////////////////////////////////////////////////////////////////////

#ifndef _VECTOR_H
#define _VECTOR_H

// I N C L U D E ////////////////////////////////////////////////////////////
#include <math.h>

// D E F I N E / M A C R O S ////////////////////////////////////////////////

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////
typedef float tVect;
typedef tVect vect2[2];
typedef tVect vect3[3];
typedef tVect vect4[4];
 
/////////////////////////////////////////////////////////////////////////////
float dot      (vect3 a, vect3 b);
void  cross    (vect3 a, vect3 b, vect3 c);
float length   (vect3 a);
bool  normalize(vect3 a);
bool  project  (vect3 u, vect3 b);
float getAngle (vect3 a, vect3 b);

/////////////////////////////////////////////////////////////////////////////

#endif // _VECTOR_H

// E O F ////////////////////////////////////////////////////////////////////