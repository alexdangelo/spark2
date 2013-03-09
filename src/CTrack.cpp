// CTrack.cpp
// by Alex D'Angelo
// Created on Aug 08, 2000
// Last modified on Jun 13, 2001
//
//---------------------------------------------------------------------------

#include "CTrack.h"

#define TRACKWIDTH 376.0f // I really wish I had written down the width from 3ds

//---------------------------------------------------------------------------
CTrack::CTrack()
{
  init();  
}

//---------------------------------------------------------------------------
void CTrack::init()
{ 
  numGates          = 0;
  numStartline      = 0;
  numFinishline     = 0;
  numCheckpoints    = 0;
  gate              = NULL;

  numStartPos       = 0;
  shipStartPos      = NULL;

  numPowerups       = 0;
  powerup           = NULL;
  currPowerupId     = 0;
  powerupDetectionRad = 40.0f;

  numLaps           = 0;
  numPlayersMax     = 1;
  
  playerStartPos[0] = 0.0f;
  playerStartPos[1] = 0.0f;
  playerStartPos[2] = 0.0f;

  model.init();
  spline.init();

//  cdModel = newCollisionModel3D();
  trackWidth = TRACKWIDTH; // ACK! THIS IS HARDCODED!

  memset( buffer, NULL, CONV_BUFFER_LEN * sizeof(char) );  

  initialized = 1;
}

//---------------------------------------------------------------------------
CTrack::~CTrack()
{
  if( initialized )
  {
    unload();
  }
}

//---------------------------------------------------------------------------
void CTrack::unload()
{ 
  SAFE_DELARY(shipStartPos); 
  SAFE_DELARY(powerup);
  SAFE_DELARY(gate);   

  model.unload(); 
  spline.unload(); 
  
  // we should figure out how to free the cdModel object

  initialized = 0;
}

//---------------------------------------------------------------------------
// Func: int CTrack::getClosestNode(vect3 pos)
// Rtns: The closest node number
// Note: This assumes that a valid trackspline exists
//        we're looking for a mindist^2 to avoid a sqrt
//---------------------------------------------------------------------------
int CTrack::getClosestNode(vect3 pos)
{
  int i;
  float minDist = 1000000000.0f;
  float testDist;
  float closestNode = 0;

  for( i = 0; i < spline.splineHead->next->numVerts; i++)
  {
                // (x-x0)^2 + (y-y0)^2 + (z-z0)^2
    testDist =  (spline.splineHead->next->vert[i][0] - pos[0]) * 
                (spline.splineHead->next->vert[i][0] - pos[0]) +

                (spline.splineHead->next->vert[i][1] - pos[1]) *
                (spline.splineHead->next->vert[i][1] - pos[1]) +

                (spline.splineHead->next->vert[i][2] - pos[2]) *
                (spline.splineHead->next->vert[i][2] - pos[2]);

    if( testDist < minDist)
    {
      minDist = testDist;
      closestNode = i;
    }
  }

  return closestNode;
}

//---------------------------------------------------------------------------
// Procedure: GetNearestPoint
// Purpose: Find the nearest point on a line segment
// Arguments: Two endpoints to a line segment a and b,
// and a test point c
// Returns: Sets the nearest point on the segment in nearest
//
// Originally by Jeff Lander, renamed by Alex D'Angelo
// http://www.gamasutra.com/features/20000210/lander_02.htm
//---------------------------------------------------------------------------
void CTrack::getPointOnSegment( tVert3 *a, tVert3 *b, tVert3 *c, tVert3 *nearest )
{
  long dot_ta;
  long dot_tb;

  // SEE IF a IS THE NEAREST POINT - ANGLE IS OBTUSE
  dot_ta = (c->x - a->x) * (b->x - a->x) +
           (c->z - a->z) * (b->z - a->z);

  if( dot_ta <= 0 ) // IT IS OFF THE A-VERTEX
  {
    nearest->x = a->x;
    nearest->z = a->z;
    return;
  }

  dot_tb = (c->x - b->x) * (a->x - b->x) +
           (c->z - b->z) * (a->z - b->z);

  // SEE IF b IS THE NEAREST POINT - ANGLE IS OBTUSE
  if( dot_tb <= 0 )
  {
    nearest->x = b->x;
    nearest->z = b->z;
    return;
  }

  // FIND THE REAL NEAREST POINT ON THE LINE SEGMENT -- BASED ON RATIO
  nearest->x = a->x + ((b->x - a->x) * dot_ta)/(dot_ta + dot_tb);
  nearest->z = a->z + ((b->z - a->z) * dot_ta)/(dot_ta + dot_tb);
}

//---------------------------------------------------------------------------
// Func: int CTrack::getNextClosestNode( vect3 pos, int closestNodeIndex )
// Desc: Gets the next closest node to the position pos on the trackspline
// Rtns: The index of the next closest node on the trackspline
// Note: This is fairly accurate, so it should work fine.
//---------------------------------------------------------------------------
int CTrack::getNextClosestNode( vect3 pos, int closestNodeIndex )
{
  int numSplineVerts = spline.splineHead->next->numVerts;
  int nodeNext = (closestNodeIndex + 1) % numSplineVerts;
  int nodePrev = (numSplineVerts + closestNodeIndex - 1) % numSplineVerts;
  
  // now figure out the next closest node, which will be 
  // on either side of the closest one (closestNode +/- 1) -- so test both
  float *splineVertNext = spline.splineHead->next->vert[nodeNext];
  float dist1 = ( (pos[0] - splineVertNext[0]) * (pos[0] - splineVertNext[0]) +
//                  (pos[1] - splineVertNext[1]) * (pos[1] - splineVertNext[1]) + // we don't care about the y-height yet
                  (pos[2] - splineVertNext[2]) * (pos[2] - splineVertNext[2]) );


  float *splineVertPrev = spline.splineHead->next->vert[nodePrev];
  float dist2 = ( (pos[0] - splineVertPrev[0]) * (pos[0] - splineVertPrev[0]) +
//                  (pos[1] - splineVertPrev[1]) * (pos[1] - splineVertPrev[1]) + // we don't care about the y-height yet
                  (pos[2] - splineVertPrev[2]) * (pos[2] - splineVertPrev[2]) );
  

  int nextClosestNode = 0;
  if( dist1 < dist2 )
  {
    nextClosestNode = nodeNext;  
  }
  else
  {
    nextClosestNode = nodePrev;
  }

  return nextClosestNode;
}

//---------------------------------------------------------------------------
// Func: int CTrack::getGateCollisionId(Line3 &shipPathSegment)
// Rtns: id => 0 if a collision, 
//       id < 0 if no collision
// Note: Relies on modded Dave Eberly's line-tri intersect code -- lin3tri3.cpp/h
//---------------------------------------------------------------------------
int CTrack::getGateCollisionId(Line3 &shipPathSegment)
{
  int visitedGateId = -1;

  //int numGates = trackPtr->getNumGates();

  for( int i = 0; (i < this->numGates) && (visitedGateId == -1); i++ ) // for each gate
  {
    if( intersect( shipPathSegment, &gate[i] ) )  // see if the ship went through it
    {
      visitedGateId = i;                          // if there was, get the gate number
    }                                             // else move on to the next gate
  }

  return visitedGateId;
}

//---------------------------------------------------------------------------
// Func: bool CTrack::intersect(Line3 &line, tGate *gate)
// Desc: Essentially a wrapper for the intersection, this 
//       just sets up all the variables to be passed to lineTriIntersect()
// Rtns: 1 on intersection
//       0 on no intersection
//---------------------------------------------------------------------------
bool CTrack::intersect(Line3 &shipPathLine, tGate *gate)
{
  Point3 dummy; // we don't care about exact intersection point

  // Test triangle 0
  bool collision = LinesegTriIntersect(shipPathLine, gate->tri[0], dummy);
  
  if( collision )
  {
    return 1;
  }
  else
  {
    // Test triangle 1
    collision = LinesegTriIntersect(shipPathLine, gate->tri[1], dummy);
    if( collision )
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

//---------------------------------------------------------------------------
// Func: int CTrack::getPowerupCollisionId( vect3 pos )
// Rtns: id >= 0 if we detected a powerup
//       id <  0 if we didn't find a powerup
// Note: Simple sphere-sphere collision detection
//---------------------------------------------------------------------------
int CTrack::getPowerupCollisionId( vect3 pos, float shipRadius)
{
  float distSq, dx, dy, dz;                         // distance squared + temp vars
  float minSepDistSq;                               // minimum separating distance^2 before a collision
  int powerupTypeId = -1;

  minSepDistSq = (powerupDetectionRad + shipRadius) * 
                 (powerupDetectionRad + shipRadius);

  for( int i = 0; i < this->numPowerups; i++ )      // for each powerup; we should also do: i < num && powerupId == -1 
  {                                                 // so we don't have to check them all
    dx = pos[0] - powerup[i].pos[0];
    dy = pos[1] - powerup[i].pos[1];
    dz = pos[2] - powerup[i].pos[2];

    distSq = dx*dx + dy*dy + dz*dz;

    if( distSq < minSepDistSq )                     // see if the ship went through it
    {
      if( powerup[i].active )                       // if the powerup is still active
      {
        powerupTypeId = powerup[i].typeId;          // get the type of powerup
        powerup[i].active = 0;                      // de-activate the powerup so we can start the spawn countdown
      }
    }
    // else move on to the next powerup
  }

  return powerupTypeId;
}
//---------------------------------------------------------------------------
// Func: void CTrack::update()
// Desc: Updates things on the track, such as 
//       the countdown to respawn powerups
// Note: should be called in CScene::tick(), but now it's just in client.cpp
//---------------------------------------------------------------------------
void CTrack::update()
{
  // update the powerups' state
  for( int i = 0; i < this->numPowerups; i++ )
  {
    if( 0 == powerup[i].active )
    {
      if( powerup[i].spawnDelay > 0 )          // we're inactive so countdown until it becomes active again
      {
        powerup[i].spawnDelay--;
      }
      else
      {
        powerup[i].spawnDelay = powerup[i].spawnReset; // we need to reset the delay
        powerup[i].active = 1;                         // and the powerup becomes active again
        error.msg("Activating powerup type %d", powerup[i].typeId);
      }
    }
  }
}

//---------------------------------------------------------------------------
// Func: void CTrack::drawGates()
// Desc: Draws the detection zones for checkpoints, start, and finish line
//---------------------------------------------------------------------------
void CTrack::drawGates()
{
  int i,j;
  int faceId;
  int id;

  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_CULL_FACE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  

  glBegin( GL_TRIANGLES );
    for( j = 0; j < numGates; j++ )
    {
      for( faceId = 0; faceId < 2; faceId++ )
      {
        for( i = 0; i < 3; i++ )
        {
          id = gate[j].face[faceId].vertIndex[i];
        
          glVertex3f( gate[j].vert[id].x,
                      gate[j].vert[id].y,
                      gate[j].vert[id].z );
        }
      }
    }
  glEnd();

  glPopAttrib();
}

//---------------------------------------------------------------------------
// Func: void CTrack::drawStartpos()
// Desc: Places a cube at each start position
//---------------------------------------------------------------------------
void CTrack::drawStartPos()
{
  int i;

  for( i = 0; i < numStartPos; i++ )
  {
    glPushMatrix();

      glTranslatef( shipStartPos[i][0],
                    shipStartPos[i][1],
                    shipStartPos[i][2] );

      drawCube(10.0f);

    glPopMatrix();
  }
}

//---------------------------------------------------------------------------
// Func: void CTrack::drawPowerups()
// Desc: Places a cube at each powerup position
//---------------------------------------------------------------------------
void CTrack::drawPowerups()
{
  int i;

  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT );
  glDisable(GL_TEXTURE_2D);
  
  for( i = 0; i < numPowerups; i++ )
  {

    if(powerup[i].active)
    {
      glColor3f( 0.0f, 1.0f, 0.0f );
    }
    else
    {
      glColor3f( 1.0f, 0.0f, 0.0f );
    }

    glPushMatrix();

      glTranslatef( powerup[i].pos[0],
                    powerup[i].pos[1],
                    powerup[i].pos[2] );

      glutWireSphere( powerupDetectionRad, 12.0f, 12.0f);
    glPopMatrix();
  }

  glPopAttrib();
}

//---------------------------------------------------------------------------
// Hey Ryan -- your cube drawing code will always persist!
//---------------------------------------------------------------------------
void CTrack::drawCube(float size)
{
  glBegin( GL_LINES );
  {
    glVertex3f( -size,  size,  size ); glVertex3f(  size,  size,  size );
    glVertex3f(  size,  size,  size ); glVertex3f(  size,  size, -size );
    glVertex3f(  size,  size, -size ); glVertex3f( -size,  size, -size );
    glVertex3f( -size,  size, -size ); glVertex3f( -size,  size,  size );

    glVertex3f( -size, -size,  size ); glVertex3f( -size,  size,  size );
    glVertex3f(  size, -size,  size ); glVertex3f(  size,  size,  size );
    glVertex3f(  size, -size, -size ); glVertex3f(  size,  size, -size );
    glVertex3f( -size, -size, -size ); glVertex3f( -size,  size, -size );

    glVertex3f( -size, -size,  size ); glVertex3f(  size, -size,  size );
    glVertex3f(  size, -size,  size ); glVertex3f(  size, -size, -size );
    glVertex3f(  size, -size, -size ); glVertex3f( -size, -size, -size );
    glVertex3f( -size, -size, -size ); glVertex3f( -size, -size,  size );
  }
  glEnd();
}

//-- E O F ------------------------------------------------------------------