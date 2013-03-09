// CShipLogic.cpp
// by Alex D'Angelo
// Created on Aug 14, 2000
// Last modified on Jan 12, 2001
//---------------------------------------------------------------------------

#include "CShip.h"

//-----------------------------------------------------------------------------
// Func: void CShip::update( float dTime )
// Desc: Updates game logic and stuff for the ship
//-----------------------------------------------------------------------------
void CShip::update( float dTime )
{
  dt = dTime; // get dt for the entire class to use
              // on my (Alex's) PIII/500, this is around 0.005s, +/- 0.004s, for checkpoints3.ase

  updatePhysics();
  checkForCollision(); // is this the right spot for it?; finish this func!

  updateGateState();
  checkForPowerups();
  
  FSOUND_SetFrequency( channel, 11025.0f + 10000.0f * vel/maxVel );  

  if( vel < (0.40f * maxVel) )
  {
    FSOUND_SetVolume( channel, (255*vel)/(0.40f * maxVel) );
  }
  else
  {
    FSOUND_SetVolume( channel, 255 );
  }
/*  
  error.msg( "The volume is: %d", FSOUND_GetVolume(channel) );
  if( this->vel < 10 ) 
    FSOUND_SetVolume(  channel, 50 );
  else
    FSOUND_SetVolume( channel, 128 );
*/
}

//-----------------------------------------------------------------------------
// Func: void CShip::updatePhysics()
// Desc: Updates the position of the ship by applying the acceleration and
//       velocity equations. 
// Note: DOES NOT handle the collision detection. However, the old position of
//       the ship is saved in oldpos[]
//-----------------------------------------------------------------------------
void CShip::updatePhysics()
{

/* for debugging
  if( (int)(dt * 100) % 5 == 0 )
  {
    error.msg( "dt=%f acc=%f vel=%f", dt, acc, vel );
  }
*/

  //---------------------
  // Save our old pos
  //---------------------
  oldpos[0] = pos[0]; // yes, could be done with a memcpy
  oldpos[1] = pos[1];
  oldpos[2] = pos[2];
  
  //---------------------
  // Update Roll
  //---------------------
  if( (roll < minRoll) && (roll > -minRoll) ) // make the roll go towards 0 degrees
  {
    roll = 0.0f;
  }
  else // decay the roll
  {
    if( roll < -minRoll )
    {
      roll += 50.0f * dt; // these numbers are fairly arbitrary -- they just look good. 

    }
    else if( roll > minRoll )
    {
      roll -= 50.0f * dt;
    }
  }
  
  //---------------------
  // Update Acceleration
  //---------------------
  if( (acc < minAcc) && (acc > -minAcc) ) // if we're at a min accel, stop accel altogether
  {
    acc = 0.0f;    
  }
  else // slow down our acceleration over time
  {
    if( acc > minAcc )
    {
      acc -= 0.8f * 100.0f * dt; // 80% each sec
    }
    else
    {
      acc += 0.8f * 100.0f * dt; // 80% each sec
    }

    // make sure the velocity doesn't go out of bounds
    if( acc >  maxAcc )
    {
      acc =  maxAcc;
    }
    else if( acc < -maxAcc )
    {
      acc = -maxAcc;
    }
  }    

  //---------------------
  // Update Velocity
  //---------------------
  vel += acc; // acc is time-dependent, so we don't need to do (acc * dt)

  if( (vel < minVel) && (vel > -minVel) ) // are we at a min vel? if so, shut of the engine
  {
    vel = 0.0f;
  }
  else
  {
    // apply friction in the right direction
    if( vel > 0.0f )
    {
      vel += friction * 100.0f * dt;
    }
    else if( vel < 0.0f )
    {
      vel -= friction * 100.0f * dt;
    }

     // and else limit the velocity
    if( vel > maxVel )
    {
      vel = maxVel;
    }
    else if( vel < -maxVel )
    {
      vel = -maxVel;
    }
  }

  //---------------------
  // Calc the new pos
  //---------------------
  // Rule: we can only accelerate in the direction we are moving in (dir of vel)
  float dv    = vel * dt;
  pos[0] += dv * sin( heading * PIOVER180 );
  pos[2] += dv * cos( heading * PIOVER180 );
}

//-----------------------------------------------------------------------------
// Func: void CShip::updateGateState()
// Desc: Checks to see if the ship passed through any checkpoints or 
//       the start and finish line
//-----------------------------------------------------------------------------
void CShip::updateGateState()
{
/* the oldpos-newpos stuff isn't working right now, mainly because I need
   to make oldpos != newpos during the update
  Line3 path;
  path.b.x = oldpos[0];
  path.b.y = oldpos[1];
  path.b.z = oldpos[2];

  path.m.x = pos[0] - oldpos[0];
  path.m.y = pos[1] - oldpos[1];
  path.m.z = pos[2] - oldpos[2];
*/

  // this is the ship's "feeler" for detecting gates. it's actually r*2 (*2? 2000-12-14) units long and is centered at the back of the ship
  float r = feelerRadius; 
  float dx = r * (float)sin( heading*PIOVER180 ); // the feeler is always in the xz plane
  float dy = 0;
  float dz = r * (float)cos( heading*PIOVER180 );

  // we're moving the base point to the back of the ship and putting the end point at the front
  Line3 path;
  path.b.x = pos[0] - dx;
  path.b.y = pos[1] - dy;
  path.b.z = pos[2] - dz;
  path.m.x = 2 * dx;
  path.m.y = 2 * dy;
  path.m.z = 2 * dz;

  int id = trackPtr->getGateCollisionId( path );// maybe we can return negative numbers to indicate the number of collisions so we can
                                                // act on more than one if needed. OTOH, we should design so we can *only* cross one
                                                // gate at a time.
  
  if( id >= 0 )                                 // we visited a gate
  {
    if( id == nextGateToVisit )                 // is this the right one?
    {
      error.msg( "Visited gate %d", id );
      
      if( 0 == id )                             // we passed through the start line
      {
        nextGateToVisit++;
      }
      else if( finishlineId == id )             // we passed through the finish line
      {
        currLapNumber++;
        nextGateToVisit = 0; // Is this right? Or should it be 1?

        error.msg( "Laps completed = %d", currLapNumber );
      }
      else // we just went through a checkpoint
      {
        nextGateToVisit = ( nextGateToVisit + 1) % trackPtr->getNumGates(); // this should wrap around
      }
    }
    else
    {
      // this is where we'd do: error.msg( "Wrong checkpoint! (%d)", id );
      // probably with a delay counter so that message doesn't come up while we're still in
      // the checkpoint and comes up once when at other checkpoints
    }
  }
  else
  {
    ; // do nothing; we didn't go through a gate
  }
}

//-----------------------------------------------------------------------------
// Func: void CShip::checkForPowerups()
// Desc: Checks for collisions with powerups
//-----------------------------------------------------------------------------
// TODO: Make the powerups have an effect on the ship
void CShip::checkForPowerups()
{
  // int id = trackPtr->getGateCollisionId( path );

  int powerupTypeId = trackPtr->getPowerupCollisionId( this->pos, this->feelerRadius );
  if(powerupTypeId >= 0)
  {
    switch( powerupTypeId )
    {
      case -1: // temp warning for Alex
        error.msg("We need to apply the powerups to the ship!");
        break;

      case PU_UNKNOWN:
        error.msg( "Found an unidentified or invalid powerup" );
        break; 

      case PU_SPEED:
        error.msg( "Found a SPEED powerup!" );
        break;

      case PU_TIME:
        error.msg( "Found a TIME powerup!" );
        break;

      default: 
        error.msg( "ERROR:  Found a powerup, but it doesn't do anything (sorry). (id = %d)", powerupTypeId );
        break;
    }
  } // else do nothing
}

//-- E O F ------------------------------------------------------------------
