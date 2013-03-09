/////////////////////////////////////////////////////////////////////////////
// Vector.cpp
// Created: April 7, 2000 - 3:06pm - Alex D'Angelo
// Common vector and matrix resources
/////////////////////////////////////////////////////////////////////////////

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h"
#include "Vector.h"

// F U N C T I O N S ////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
float dot(vect3 a, vect3 b)
{
  return (a[0] * b[0] +
          a[1] * b[1] +
          a[2] * b[2]);
}

//---------------------------------------------------------------------------
// c = a X b
void cross(vect3 a, vect3 b, vect3 c)
{
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}

//---------------------------------------------------------------------------
float length(vect3 a)
{
  return (float)sqrt( a[0] * a[0] +
                      a[1] * a[1] +
                      a[2] * a[2]);
}

//---------------------------------------------------------------------------
bool normalize(vect3 a)
{
  float length;
  float invlength;
  const float tolerance = (float)1e-05;

  length = (float)sqrt( a[0] * a[0] +
                        a[1] * a[1] + 
                        a[2] * a[2]);

  if(length > tolerance)
  {
    invlength = 1.0f / length;
    
    a[0] *= invlength;
    a[1] *= invlength;
    a[2] *= invlength;
    
    return 1;
  }
  else
  {
    a[0] = 0.0f;
    a[1] = 0.0f;
    a[2] = 0.0f;

    return 0;
  }
}

//---------------------------------------------------------------------------
// projects vector u onto vector b
// projection = b * (u dot b)/(||b||^2)
//
// the projected vector is stored in b
bool project(vect3 u, vect3 b)
{
  float dottemp;
  float divisor;
  float multiplier;
  const double tolerance = 1e-05;
  
  dottemp = dot(u, b);
  divisor = dot(b, b); // same as square( sqrt( square(b) ) );
  if(divisor > (float)tolerance)
  multiplier = dottemp / divisor;
  
  b[0] *= multiplier;
  b[1] *= multiplier;
  b[2] *= multiplier;
  
  return 1;
}

//---------------------------------------------------------------------------
float getAngle(vect3 a, vect3 b)
{
  // get angle -- cos theta = a.b / ||a|| ||b||
  const float toler = 1e-05f;
  float theta;
  float angle;
  float dotprod;
  float len;

  len = length(a) * length(b);

  if(len > toler)
  {
    dotprod = dot(a,b) / len;
    theta = (float)acos((double)dotprod);
    angle = theta * ONE_EIGHTY_OVER_PI;
    return angle;
  }
  else
  {
    return 0.0f; // this means that a & b are the same vector ==> angle == 0
  }  
}

// E O F ////////////////////////////////////////////////////////////////////