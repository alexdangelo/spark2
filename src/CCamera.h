// CCamera.h (spark2)
// by Alex D'Angelo
// Created on June 01, 2000
// Last modified on June 01, 2000
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

#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include "ConvCommon.h" // required because this uses CModel
#include "CShip.h"      // so we can attach the camera to a ship

//---------------------------------------------------------------------------
// Notes: this should be using insts instead of the model, since they
//        hold all the position info.
//
//---------------------------------------------------------------------------
class CCamera
{
public: // protected:
    vect3  eyepos;
    vect3  target;
    vect3  up;
  protected:
    CModel *targetModel;
    CShip  *targetShip;
    bool   isCameraOn;
public:
    bool   isFollowingModel;
    bool   isFollowingShip;
  
    bool   isLookingAtModel;  // Only applies if isFollowingModel/Ship == 1
    bool   isLookingAtShip;   //   0 == temporarily move independently of the model
                              //   1 == return to following the model
  public: // private:
    GLfloat upDownAngle; // GLfloat about x-axis; in yz plane
    GLfloat elevation;   // GLfloat same as rho (p)
    GLfloat heading;     // GLfloat about y-axis; in xz plane
    GLfloat distFromTarget;
    GLfloat hoverHeight; // how high up are we?

  public:
    CCamera();
    ~CCamera();

    bool isOn()    { return isCameraOn; };
    void turnOn()  { isCameraOn = 1;    };
    void turnOff() { isCameraOn = 0;    };

    void view();

    
    // Note: Only inits the camera; doesn't do lookAt()
    void init     (float eyex,    float eyey,    float eyez,
                   float targetx, float targety, float targetz,
                   float upx,     float upy,     float upz);

    void setEyepos(float eyex,    float eyey,    float eyez);
    void setUp    (float upx,     float upy,     float upz);
    void setTarget(float targetx, float targety, float targetz);

    void followModel(CModel *currModel);
    void stopFollowingModel();

    void followShip (CShip *currShip);
    void stopFollowingShip();

    void lookAtModel(CModel *currModel);

    void rotate(float deg); // camera position stays stationary and rotates the camera target
    
    void move(float step);
    
    void moveUp();
    void moveDown();
    void moveIn();
    void moveOut();

    // moves camera around a point (circle-strafe :P)
    void orbitUp   (float deg);
    void orbitDown (float deg);
    void orbitXZ(float deg);

    void cross (float dst[3], float srcA[3], float srcB[3]);
    void normalize(float src[3]);
    void scale      (float v[3], float s);
    void multLookAt (float eyex, float eyey, float eyez,
                     float atx, float aty, float atz,
                     float upx, float upy, float upz);

};


//---------------------------------------------------------------------------

#endif

//-- E O F ------------------------------------------------------------------