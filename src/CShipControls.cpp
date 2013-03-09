// CShipControls.cpp
// by Alex D'Angelo
// Created on Aug 14, 2000
// Last modified on Aug 14, 2000
//
//---------------------------------------------------------------------------

#include "CShip.h"

//-----------------------------------------------------------------------------
// Func: void CShip::reset()
// Desc: Resets the current ship, ie shields, position, mathengine state, etc
//-----------------------------------------------------------------------------
void CShip::reset()
{
  if( trackPtr != NULL )
  {
    pos[0] = trackPtr->playerStartPos[0];
    pos[1] = trackPtr->playerStartPos[1];
    pos[2] = trackPtr->playerStartPos[2];
  }
  else
  {
    pos[0] = 0.0f;
    pos[1] = 10.0f;
    pos[2] = 0.0f; 
  }
  
  dt       = 0.0f;
  da       = 100.0f; // why some of these numbers work, I'm not sure, but
  acc      = 0.0f;   // they look good
  vel      = 0.0f;
  maxAcc   = 10.0f;
  maxVel   = 800.0f;
  friction = -1.0f;
  heading  = 0.0f;
  dDeg     = 90.0f;
  
  shieldStrength = 100; // percent

  roll = 0.0f;

  minAcc = -friction;
  minVel = -friction*4;
  minRoll = 1.0f; // 1 degree
}

//---------------------------------------------------------------------------
void CShip::accelerate()
{
  acc += da * 100.0f * dt;

  if( acc > maxAcc )
  {
    acc = maxAcc;
  }
}

//---------------------------------------------------------------------------
void CShip::decelerate()
{
  acc -= da * 100.0f * dt;

  if( acc < -maxAcc )
  {
    acc = -maxAcc;
  }
}

//---------------------------------------------------------------------------
void CShip::turnLeft()
{
  heading += dDeg * dt;

  if( heading < 0.0f   ) heading += 360.0f;
  if( heading > 360.0f ) heading -= 360.0f;

  roll -= dDeg * dt;

  if( roll < -30.0f ) roll = -30.0f; // limit roll to -30 degrees
}

//---------------------------------------------------------------------------
void CShip::turnRight()
{
  heading -= dDeg * dt;

  if( heading < 0.0f   ) heading += 360.0f;
  if( heading > 360.0f ) heading -= 360.0f;

  roll += dDeg * dt;

  if( roll > 30.0f ) roll = 30.0f; // limit roll to 30 degrees
}

//-- E O F ------------------------------------------------------------------