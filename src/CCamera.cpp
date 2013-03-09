// CCamera.cpp (spark2)
// by Alex D'Angelo
// Created on June 01, 2000
// Last modified on June 06, 2000
//
// Desc: Framework for camera class
//
// Usage:
// class CShip
// {
//   ....
//   CCamera camera; // each ship has its own camera
// };
//
// class CClient
// {
//   CCamera worldcam;
//   CShip   ship;
//   ...
// };
//
//---------------------------------------------------------------------------

#include "CCamera.h"
#include "Vector.h"

//---------------------------------------------------------------------------
// Notes: this should be using insts instead of the model, since they
//        hold all the position info.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Func: CCamera::CCamera()
//
// Desc: Camera constructor
//---------------------------------------------------------------------------
CCamera::CCamera()
{
  eyepos[0] = 0;
  eyepos[1] = 0;
  eyepos[2] = 100;
  
  target[0] = 0;
  target[1] = 0;
  target[2] = 0;
  
  up    [0] = 0;
  up    [1] = 1;
  up    [2] = 0;
  
  isCameraOn        = 0;
  targetModel       = NULL;
  targetShip        = NULL;
  isFollowingModel  = 0;
  isFollowingShip   = 0;
  isLookingAtModel  = 0;
  isLookingAtShip   = 0;

  upDownAngle       = 0;
  elevation         = 45;
  heading           = 0;
  distFromTarget    = 300;
}

//---------------------------------------------------------------------------
// Func: CCamera::~CCamera()
//
// Desc: Camera destructor
//---------------------------------------------------------------------------
CCamera::~CCamera()
{
  // do nothing besides:
  targetModel = NULL;
  targetShip  = NULL;
}

//---------------------------------------------------------------------------
// Func: void CCamera::init(float eyex,    float eyey,    float eyez,
//                          float targetx, float targety, float targetz,
//                          float upx,     float upy,     float upz)
//
// Desc: Initializes the camera with the specified values.
//---------------------------------------------------------------------------
void CCamera::init(float eyex,    float eyey,    float eyez,
                   float targetx, float targety, float targetz,
                   float upx,     float upy,     float upz)
{
  eyepos[0] = eyex;
  eyepos[1] = eyey;
  eyepos[2] = eyez;

  target[0] = targetx;
  target[1] = targety;
  target[2] = targetz;

  up    [0] = upx;
  up    [1] = upy;
  up    [2] = upz;
}

//---------------------------------------------------------------------------
// Func: void CCamera::view()
//
// Desc: Look at the world through this camera
//---------------------------------------------------------------------------
void CCamera::view()
{
  if(isFollowingModel)
  {
/*
    gluLookAt(eyepos          [0], eyepos          [1], eyepos          [2],
              targetModel->pos[0], targetModel->pos[1], targetModel->pos[2],
              up              [0], up              [1], up              [2]);
*/

  }
  else if(isFollowingShip)
  {
    gluLookAt(eyepos         [0], eyepos         [1], eyepos         [2],
              targetShip->pos[0], targetShip->pos[1], targetShip->pos[2],
              up             [0], up             [1], up             [2]);
  }
  else
  {
    gluLookAt(eyepos[0], eyepos[1], eyepos[2],
              target[0], target[1], target[2],
              up    [0], up    [1], up    [2]);
  }
}

//---------------------------------------------------------------------------
// Func: void CCamera::lookAtModel(CModel *currModel)
//
// Desc: Looks at the specified model. Doesn't connect the camera to the 
//       model. That needs to be done with CCamera::setTarget(...)
//---------------------------------------------------------------------------
void CCamera::lookAtModel(CModel *currModel)
{
/*
  gluLookAt(eyepos          [0], eyepos          [1], eyepos          [2],
            targetModel->pos[0], targetModel->pos[1], targetModel->pos[2],
            up              [0], up              [1], up              [2]);
*/
}



//---------------------------------------------------------------------------
void CCamera::setEyepos(float eyex,  float eyey,  float eyez)
{
  eyepos[0] = eyex;
  eyepos[1] = eyey;
  eyepos[2] = eyez;
}

//---------------------------------------------------------------------------
void CCamera::setUp(float upx,     float upy,     float upz)
{
  up    [0] = upx;
  up    [1] = upy;
  up    [2] = upz;
}

//---------------------------------------------------------------------------
void CCamera::setTarget(float targetx, float targety, float targetz)
{
  target[0] = targetx;
  target[1] = targety;
  target[2] = targetz;
}

//---------------------------------------------------------------------------
// Func: void CCamera::followModel(CModel *currModel)
//
// Desc: Makes the camera follow the given model
//---------------------------------------------------------------------------
void CCamera::followModel(CModel *currModel)
{
  targetModel      = currModel;
  isFollowingShip  = 1;
  isLookingAtModel = 1;
}

//---------------------------------------------------------------------------
// Func: void CCamera::stopFollowingModel()
//
// Desc: Disconnects the camera from the model; it just won't follow it anymore
//---------------------------------------------------------------------------
void CCamera::stopFollowingModel()
{
  if(isFollowingModel)
  {
    if(isLookingAtModel && targetModel != NULL)
    {
/*
      target[0] = targetModel->pos[0]; // makes sure our Model actually exists so we can 
      target[1] = targetModel->pos[1]; //  copy the info on where we were last pointing
      target[2] = targetModel->pos[2];
*/      
      targetModel = NULL; // disconnects the model from the camera
    }
    else
    {
      // leave the target alone
    }
  }
  
  isFollowingModel = 0;
  isLookingAtModel = 0;
}

//---------------------------------------------------------------------------
// Func: void CCamera::followShip(CShip *currShip)
//
// Desc: Connects the camera to the ship; chasecam
//---------------------------------------------------------------------------
void CCamera::followShip(CShip *currShip)
{
  targetShip      = currShip;
  isFollowingShip = 1;
  isLookingAtShip = 1;


// from spark v1
  distFromTarget   = 100.0f;
  upDownAngle    = 30.0f;
  heading = 0.0f;
  
  GLfloat dist = distFromTarget;
  eyepos[0] = targetShip->pos[0]; //pShip->Camera.xpos = pShip->xpos;
  eyepos[1] = targetShip->pos[1] - dist *(GLfloat)sin(upDownAngle); // pShip->Camera.ypos = pShip->ypos - dist * (GLfloat)sin(pShip->Camera.fUpDownAngle);
  eyepos[2] = targetShip->pos[2] - dist *(GLfloat)cos(upDownAngle); // pShip->Camera.zpos = pShip->zpos - dist * (GLfloat)cos(pShip->Camera.fUpDownAngle);

  
  hoverHeight = targetShip->pos[1] + 30.0f;
// end from spark v1

}

//---------------------------------------------------------------------------
// Func: void CCamera::stopFollowingShip()
//
// Desc: Disconnects the camera from the ship; it just won't follow it anymore
//---------------------------------------------------------------------------
void CCamera::stopFollowingShip()
{
  if(isFollowingShip)
  {
    if(isLookingAtShip && targetShip != NULL)
    {
      target[0] = targetShip->pos[0]; // makes sure our ship actually exists so we can 
      target[1] = targetShip->pos[1]; //  copy the info on where we were last pointing
      target[2] = targetShip->pos[2];
      
      targetShip = NULL;
    }
    else
    {
      // leave the target alone
    }
  }
  
  isFollowingShip = 0;
  isLookingAtShip = 0;
}

//---------------------------------------------------------------------------
// Func: void CCamera::rotate(float deg)
//
// Desc: A positive degree will rotate the target point counter-clockwise
//       A negative degree will rotate the target point clockwise
//---------------------------------------------------------------------------
void CCamera::rotate(float deg)
{
  if( isFollowingShip || isFollowingModel )
  {
    heading += deg;
  
    if(heading <   0.0f) heading += 360.0f;
    if(heading > 360.0f) heading -= 360.0f;
/*because we're doing things spark v1 style
    float radius = sqrt((target[0] - eyepos[0]) * (target[0] - eyepos[0]) +
                        (target[2] - eyepos[2]) * (target[2] - eyepos[2]) );

    target[0] = eyepos[0] + radius * (float)cos(heading*PIOVER180);
    target[2] = eyepos[2] + radius * (float)sin(heading*PIOVER180);
*/
  }
}

//---------------------------------------------------------------------------
// Func: 
//---------------------------------------------------------------------------
void CCamera::moveUp()
{
  if( isFollowingShip || isFollowingModel )
  {
    eyepos[1] += 2.0f;
  }
  else
  {
    eyepos[1] += 2.0f;
    target[1] += 2.0f;
  }
}

//---------------------------------------------------------------------------
// Func: 
//---------------------------------------------------------------------------
void CCamera::moveDown()
{
  if(isFollowingShip || isFollowingModel)
  {
    eyepos[1] -= 2.0f;
  }
  else
  {
    eyepos[1] -= 2.0f;
    target[1] -= 2.0f;
  }
}

//---------------------------------------------------------------------------
// Func: 
//---------------------------------------------------------------------------
void CCamera::move(float step)
{
  float dx = step * cos(heading * PIOVER180);
  float dz = step * sin(heading * PIOVER180);

  eyepos[0] += dx;
  eyepos[0] += 0.0f;
  eyepos[2] += dz;

  if( !(isFollowingShip || isFollowingModel) )
  {
    target[0] += dx;
    target[1] += 0.0f;
    target[2] += dz;
  }
}

//---------------------------------------------------------------------------
// Func: 
//---------------------------------------------------------------------------
void CCamera::moveIn()
{
  distFromTarget -= 1.0f;

  if(distFromTarget < 0.0f)
  {
    distFromTarget = 1.0f;
  }
}

//---------------------------------------------------------------------------
// Func: 
//---------------------------------------------------------------------------
void CCamera::moveOut()
{
  distFromTarget += 1.0f;

/* enable this later when it matters Feb 01, 2000
  if(distFromTarget > MAX_CAMERA_DIST)
  {
    distFromTarget = MAX_CAMERA_DIST;
  }
*/
}


//---------------------------------------------------------------------------
// Orbits the camera up
//  with the top of the y-axis = 0, 
//       the xy plane          = 90,
//       the bottom of y-axis  = 180
//---------------------------------------------------------------------------
//  
// This isn't working!!! (June 06, 2000)
void CCamera::orbitUp(float deg)
{
  elevation -= deg;
  
  if(elevation <   0.1f) elevation = 0.1f;
  if(elevation > 180.0f) elevation = 180.0f;

  if(isFollowingShip && isLookingAtShip)
  {
    target[0] = targetShip->pos[0]; // makes sure our ship actually exists so we can 
    target[1] = targetShip->pos[1]; //  copy the info on where we were last pointing
    target[2] = targetShip->pos[2];
  }

  float radius = sqrt((eyepos[0] - target[0]) * (eyepos[0] - target[0]) +
                      (eyepos[1] - target[1]) * (eyepos[1] - target[1]) +
                      (eyepos[2] - target[2]) * (eyepos[2] - target[2]) );
  GLfloat dx, dy, dz;
  
  dx = radius * (GLfloat)cos(heading * PIOVER180) * sin(elevation * PIOVER180);
  dy = radius * (GLfloat)cos(heading * PIOVER180);
  dz = radius * (GLfloat)sin(heading * PIOVER180) * sin(elevation * PIOVER180);

  eyepos[0] = target[0] + dx;
  eyepos[1] = target[1] + dy;
  eyepos[2] = target[2] + dz;
}

//---------------------------------------------------------------------------
// Orbits the camera down
//  with the top of the y-axis = 0, 
//       the xy plane          = 90,
//       the bottom of y-axis  = 180
//---------------------------------------------------------------------------
//
// This isn't working!!! (June 06, 2000)
void CCamera::orbitDown(float deg)
{
  elevation += deg;
  
  if(elevation <   0.1f) elevation = 0.1f;
  if(elevation > 180.0f) elevation = 180.0f;

  if(isFollowingShip && isLookingAtShip)
  {
    target[0] = targetShip->pos[0]; // makes sure our ship actually exists so we can 
    target[1] = targetShip->pos[1]; //  copy the info on where we were last pointing
    target[2] = targetShip->pos[2];
  }

  float radius = sqrt((eyepos[0] - target[0]) * (eyepos[0] - target[0]) +
                      (eyepos[1] - target[1]) * (eyepos[1] - target[1]) +
                      (eyepos[2] - target[2]) * (eyepos[2] - target[2]) );
  GLfloat dx, dy, dz;
  
  dx = radius * (GLfloat)cos(heading * PIOVER180) * sin(elevation * PIOVER180);
  dy = radius * (GLfloat)cos(heading * PIOVER180);
  dz = radius * (GLfloat)sin(heading * PIOVER180) * sin(elevation * PIOVER180);

  eyepos[0] = target[0] + dx;
  eyepos[1] = target[1] + dy;
  eyepos[2] = target[2] + dz;
}

//---------------------------------------------------------------------------
// Orbits in a circle parallel to the plane around target
// A positive degree rotates counter-clockwise from curr point
// A negative degree rotates clockwise from curr point
//---------------------------------------------------------------------------
//
void CCamera::orbitXZ(float deg)
{
  heading -= deg;

  if(heading > 360.0f) heading -= 360.0f;
  if(heading <   0.0f) heading += 360.0f;

  if(isFollowingShip && isLookingAtShip)
  {
    target[0] = targetShip->pos[0]; // makes sure our ship actually exists so we can 
    target[1] = targetShip->pos[1]; //  copy the info on where we were last pointing
    target[2] = targetShip->pos[2];
  }

  float radius = sqrt((eyepos[0] - target[0]) * (eyepos[0] - target[0]) +
                      (eyepos[2] - target[2]) * (eyepos[2] - target[2]) );
  GLfloat dx, dz;
  
  dx = radius * (GLfloat)cos((heading+180.0f) * PIOVER180);
  dz = radius * (GLfloat)sin((heading+180.0f) * PIOVER180);

  eyepos[0] = target[0] + dx;
  eyepos[2] = target[2] + dz;
}

//---------------------------------------------------------------------------
// Scale the given vector
void CCamera::scale (float v[3], float s)
{
    v[0] *= s;
    v[1] *= s;
    v[2] *= s;
}

/* from lookat.cpp -- OpenGL FAQ and Troubleshooting Guide 
                      http://www.frii.com/~martz/oglfaq/lookat.cpp
    multLookAt -- Create a matrix to make an object, such as
        a camera, "look at" another object or location, from
        a specified position.

    Parameters:
        eye[x|y|z] Desired location of the camera object
        at[x|y|z]  Location for the camera to look at
        up[x|y|z]  Up vector of the camera

    Algorithm:
        The desired transformation is obtained with this 4x4 matrix:
            |  [xaxis] 0  |
            |    [up]  0  |
            |   [-at]  0  |
            |   [eye]  1  |
        Where 'xaxis', 'up' and 'at' are the new X, Y, and Z axes of
        the transforned object, respectively, and 'eye' is the input
        new location of the transformed object.

    Assumptions:
        The camera geometry is defined to be facing
        the negative Z axis.

    Usage:
        multLookAt creates a matrix and transforms it onto the
        current matrix stack. Typical usage would be as follows:

            glMatrixMode (GL_MODELVIEW);
            // Define the usual view transformation here using
            //   gluLookAt or whatever.
            glPushMatrix();
            multLookAt (orig[0], orig[1], orig[2],
                at[0], at[1], at[2],
                up[0], up[1], up[2]);
            // Define "camera" object geometry here
            glPopMatrix();

    Warning: Results become undefined as (at-eye) approaches
        coincidence with (up).
*/
void CCamera::multLookAt (float eyex, float eyey, float eyez,
                          float atx, float aty, float atz,
                          float upx, float upy, float upz)
{
    float m[16];
    float *xaxis = &m[0],
        *up = &m[4],
        *at = &m[8];

    // Compute our new look at vector, which will be
    //   the new negative Z axis of our transformed object.
    at[0] = atx-eyex; at[1] = aty-eyey; at[2] = atz-eyez;
    CCamera::normalize(at);

    // Make a useable copy of the current up vector.
    up[0] = upx; up[1] = upy; up[2] = upz;

    // Cross product of the new look at vector and the current
    //   up vector will produce a vector which is the new
    //   positive X axis of our transformed object.
    CCamera::cross(xaxis, at, up);
    CCamera::normalize(xaxis);

    // Calculate the new up vector, which will be the
    //   positive Y axis of our transformed object. Note
    //   that it will lie in the same plane as the new
    //   look at vector and the old up vector.
    CCamera::cross(up, xaxis, at);

    // Account for the fact that the geometry will be defined to
    //   point along the negative Z axis.
    CCamera::scale(at, -1.f);

    // Fill out the rest of the 4x4 matrix
    m[3] = 0.f;     // xaxis is m[0..2]
    m[7] = 0.f;     // up is m[4..6]
    m[11] = 0.f;    // -at is m[8..10]
    m[12] = eyex; m[13] = eyey; m[14] = eyez;
    m[15] = 1.f;

    // Multiply onto current matrix stack.
    glMultMatrixf(m);
}

void CCamera::cross (float dst[3], float srcA[3], float srcB[3])
{
    dst[0] = srcA[1]*srcB[2] - srcA[2]*srcB[1];
    dst[1] = srcA[2]*srcB[0] - srcA[0]*srcB[2];
    dst[2] = srcA[0]*srcB[1] - srcA[1]*srcB[0];
}

// Normalize the input vector
void CCamera::normalize (float vec[3])
{
    const float squaredLen = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
    const float invLen = 1.f / (float) sqrt (squaredLen);

    vec[0] *= invLen;
    vec[1] *= invLen;
    vec[2] *= invLen;
}
//-- E O F ------------------------------------------------------------------