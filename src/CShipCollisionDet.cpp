// CShipCollisionDet.cpp
// Created on Dec 14, 2000
// by Alex D'Angelo
//---------------------------------------------------------------------------

#include "CShip.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CShip::checkForCollision()
{
  // June 13, 2001
  // let's clamp the ship's maximum position to
  // the distance between the center of the track and the edge of the track
  // This means that 
  // 1) tracks must be a fixed width
  // 2) there can only be one solid track; the track can't split and rejoin
  //    later
  // 
  // Although this isn't the best solution, it's the quickest, easiest, and
  // best looking for now. I just want to finish the game.

  // void clampPositionToTrack()
  // first we'll need to get the dist from the ship to the track spline
  // which is in getClosestNodes...()


/*
  bool isCol = trackPtr->cdModel->sphereCollision(this->pos, this->feelerRadius);
  if(isCol)
  {
    // error.msg( "We detected the track" );
  }
  else
  {
    // error.msg( "We could not find the track" );
  }
*/
}
//---------------------------------------------------------------------------
// June 13, 2001
// let's clamp the ship's maximum position to
// the distance between the center of the track and the edge of the track
// This means that 
// 1) tracks must be a fixed width
// 2) there can only be one solid track; the track can't split and rejoin
//    later
// 
// Although this isn't the best solution, it's the quickest, easiest, and
// best looking for now. I just want to finish the game.
//---------------------------------------------------------------------------
void CShip::clampPosToTrack()
{
  // get the closest node to the ship
  int closestNode     = trackPtr->getClosestNode( pos );
  int nextClosestNode = trackPtr->getNextClosestNode( pos, closestNode );

  float *nodeA = trackPtr->spline.splineHead->next->vert[closestNode    ];
  float *nodeB = trackPtr->spline.splineHead->next->vert[nextClosestNode];
  tVert3 a       = { nodeA[0], nodeA[1], nodeA[2] };
  tVert3 b       = { nodeB[0], nodeB[1], nodeB[2] };
  tVert3 shipPos = { pos  [0], pos  [1], pos  [2] };
  tVert3 nearest;

  trackPtr->getPointOnSegment( &a, &b, &shipPos, &nearest );
  
  // June 13, 2001
  // Now we have the closest point on the spline to the ship
  // so we can clamp the absolute distance by moving the ship
  // along the line perpendicular to the spline and ship

  float shipdist2 = (shipPos.x - nearest.x)*(shipPos.x - nearest.x) +
  //                  (shipPos.y - nearest.y)*(shipPos.y - nearest.y) +
                    (shipPos.z - nearest.z)*(shipPos.z - nearest.z);
  float shipdist  = feelerRadius + sqrt( shipdist2 ); // ship_radius + dist to make sure ship isn't in the walls or outside them
  float halfTrackWidth = 0.5f * trackPtr->getTrackWidth();
  
  if( shipdist > halfTrackWidth )
  {
    // uh-oh! the ship is on or outside of the track bounds
    // so move the ship along the line perpendicular to 
    // the spline and ship until the dist+ship.radius is in the track
    // we can do this by treating the line as a vector (with tail
    // on the spline) and scaling down the head
    float scale = (halfTrackWidth / shipdist);
    pos[0] = nearest.x + scale * (shipPos.x - nearest.x);
    //pos[1] = nearest.y + scale * (shipPos.y - nearest.y);
    pos[2] = nearest.z + scale * (shipPos.z - nearest.z);
    //error.msg( "Clamping distance, with scale=%f", scale );
  }
  else
  {
    // the ship is within the walls of the track
    // so don't do anything
  }
}

//---------------------------------------------------------------------------
// Must send down an array with room for two integers. They will be the
// indexes of the two closest nodes to the ship
//---------------------------------------------------------------------------
//void CShip::getClosestNodes(int nodeIdxs[2])
void CShip::getClosestNodesAndDrawThem()
{
  // get the closest node to the ship
  int closestNode     = trackPtr->getClosestNode( pos );
  int nextClosestNode = trackPtr->getNextClosestNode( pos, closestNode );

  float *nodeA = trackPtr->spline.splineHead->next->vert[closestNode    ];
  float *nodeB = trackPtr->spline.splineHead->next->vert[nextClosestNode];
  tVert3 a       = { nodeA[0], nodeA[1], nodeA[2] };
  tVert3 b       = { nodeB[0], nodeB[1], nodeB[2] };
  tVert3 shipPos = { pos  [0], pos  [1], pos  [2] };
  tVert3 nearest;

  trackPtr->getPointOnSegment( &a, &b, &shipPos, &nearest );
  //---------------------------------------------------------------
  //
  // We would do some collision detection and response here
  //
  // Line is L(t) = b+t*m for any real-valued t
  //---------------------------------------------------------------
  // let's just ident the triangle we're colliding with here by
  // getting the triangle's verts and then drawing a white wireframe
  // outline on top of the textured track
  //---------------------------------------------------------------
  Line3 cdLine;

  // we use these deltas to calc the slope of the cd line
  float dx = pos[0] - nearest.x;
  float dy = 0; // not used right now, else it would be: pos[1] - nearest.y;
  float dz = pos[2] - nearest.z;
  float invLen = 1.0f / sqrt( dx*dx + dy*dy + dz*dz );
  float scale = 500.0f;

  // move line into worldspace
  cdLine.b.x = nearest.x;
  cdLine.b.y = 60; // not used right now, else it would be: nearest.y;
  cdLine.b.z = nearest.z;
  // scale rest of line up
  cdLine.m.x = scale * invLen * dx;
  cdLine.m.y = scale * invLen * dy;
  cdLine.m.z = scale * invLen * dz;

  // draw the cdLine
  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
  glDisable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_LINES );
    glVertex3f( cdLine.b.x, cdLine.b.y, cdLine.b.z );
    glVertex3f( cdLine.b.x + cdLine.m.x, 
                cdLine.b.y + cdLine.m.y, 
                cdLine.b.z + cdLine.m.z );
  glEnd();
  glPopAttrib();

/*   
  //---------------------------------------------------------------
  // for "visual debugging" -- draws lines to the two closest nodes
  float *splineVert1 = trackPtr->spline.splineHead->next->vert[closestNode];
  float *splineVert2 = trackPtr->spline.splineHead->next->vert[nextClosestNode];
  glPushMatrix();
    glBegin( GL_LINES );
      glVertex3f( pos[0], 1, pos[2] ); // ship
      glVertex3f( splineVert1[0], splineVert1[1], splineVert1[2] );

      glVertex3f( pos[0], 1, pos[2] ); // ship
      glVertex3f( splineVert2[0], splineVert2[1], splineVert2[2] );

      glVertex3f( pos[0], 1, pos[2]); // ship
      nearest.y = 0; // not used right now
      glVertex3f( nearest.x, nearest.y, nearest.z );
    glEnd();
  glPopMatrix();
*/

}
/*
//---------------------------------------------------------------------------
// Procedure: GetNearestPoint
// Purpose: Find the nearest point on a line segment
// Arguments: Two endpoints to a line segment a and b,
// and a test point c
// Returns: Sets the nearest point on the segment in nearest
//
// By Jeff Lander? from some article at gamasutra on collision detection
//---------------------------------------------------------------------------
void CShip::getPointOnSegment( tVert3 *a, tVert3 *b, tVert3 *c, tVert3 *nearest )
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
*/
int lastFoundTri = 0;
//---------------------------------------------------------------------------
// Jun 13, 2001
// We're not going to use this, even though the function name amuses me
//
// Mar 17, 2001
// So far this seems more promising. The problem right now is that although
// we're detecting triangles, they're either being detected when they
// shouldn't be (like the ship never even touched them) or not being detected
// when they should. I'm drawing the line between pos n and n-1, but it's
// basically non-existant. Maybe that's why it's not working right, or the
// cdLine thinks its somewhere else; that we're not drawing the cdLine in the
// correct spot. Anyway, very frustrating for now.
// Right now we're hi-lighting the last triangle detected. Race around the 
// track a couple of times and you'll see what i mean about things not 
// working.
void CShip::ChkVectorCurrPosMinusLastPosIntersectedATri()
{
  // check to see if vector ship (pos N) - (pos N-1) intersects a triangle
  // we have pos[0-2] and oldpos[0-2] to work with
  float dx = pos[0] - oldpos[0];
  float dy = pos[1] - oldpos[1];
  float dz = pos[2] - oldpos[2];

  Line3 cdLine;
  cdLine.b.x = oldpos[0];
  cdLine.b.y = oldpos[1];
  cdLine.b.z = oldpos[2]; 
  cdLine.m.x = dx; // are these right?
  cdLine.m.y = dy;
  cdLine.m.z = dz;
  
  int triangleId = trackPtr->getLineIntersectTriangle( cdLine );
  if( triangleId == -1 )
  {
    // do nothing because we didn't find a triangle
    // error.msg( "Debugging: No triagle found" );
  }
  else
  {
    error.msg( "Debugging: Found a triangle! (%d)", triangleId );
    lastFoundTri = triangleId; // debugging: keep hilighting the last detected tri
    //MessageBox( NULL, "found a triangle", "tri found", MB_OK );
    // get the triangle data
    Triangle3 foundTri = trackPtr->getTriangle( triangleId );

    // hi-lite the triangle here
    glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
    glColor3f( 1.0f, 1.0f, 0.0f );
    glPushMatrix();
      vect3 &worldPos = trackPtr->model.modelHead->next->instHead->next->worldpos;
      glTranslatef( worldPos[0], worldPos[1], worldPos[2] );
      trackPtr->drawTriangleWire( foundTri );
    glPopMatrix();
    glPopAttrib();
    // handle collision here
  }

  // debugging: keep hilighting the last detected tri
  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
  glColor3f( 1.0f, 1.0f, 0.0f );
  glPushMatrix();
    vect3 &worldPos = trackPtr->model.modelHead->next->instHead->next->worldpos;
    glTranslatef( worldPos[0], worldPos[1], worldPos[2] );
    trackPtr->drawTriangleWire( trackPtr->getTriangle(lastFoundTri) );
  glPopMatrix();
  glPopAttrib();

  // draw the cdLine
  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
  glDisable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor3f( 0.0f, 0.0f, 1.0f );
  glBegin( GL_LINES );
    glVertex3f( cdLine.b.x, cdLine.b.y, cdLine.b.z );
    glVertex3f( cdLine.b.x + cdLine.m.x, 
                cdLine.b.y + cdLine.m.y, 
                cdLine.b.z + cdLine.m.z );
  glEnd();
  glPopAttrib();
}

//---------------------------------------------------------------------------
void CShip::JustDrawAllTheTrackTriangles()
{
  Triangle3 currTri;
  
  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
  glDisable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor3d(0,1,0);

  glPushMatrix();
    vect3 &worldPos = trackPtr->model.modelHead->next->instHead->next->worldpos;
    glTranslatef( worldPos[0], worldPos[1], worldPos[2] );
    for( int i = 0; i < trackPtr->model.modelHead->next->numFaces; i++ )
    {  
      currTri = trackPtr->getTriangle( i );
      trackPtr->drawTriangle( currTri );
    }
  glPopMatrix();
  glPopAttrib();
}

//-- E O F ------------------------------------------------------------------