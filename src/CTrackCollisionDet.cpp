// CTrackLoad.cpp
// by Alex D'Angelo
// Created on Feb 10, 2001
// Last modified on Feb 10, 2001
//
//---------------------------------------------------------------------------

#include "CTrack.h"
#include "ConvCommon.h"

//---------------------------------------------------------------------------
// figure out what triangle is next to the ship. it doesn't necessarily
// mean that we're colliding with it. CShip will determine that.
// Returns triangle id if we intersected with a triangle
//         -1 if we didn't hit a triangle
//---------------------------------------------------------------------------
int CTrack::getLineIntersectTriangle( Line3 &lineThroughShipAndSpline )
{
  // Line is L(t) = b+t*m for any real-valued t
  Line3 cdLine = lineThroughShipAndSpline;       // alias
  bool foundCollision = false;
  Triangle3 testTri;
  int currVertIdx0;
  int currVertIdx1;
  int currVertIdx2;
  int vertId;

  vect3 &worldpos = model.modelHead->instHead->next->worldpos;
  Point3 intersect;
  int intersectedTriId = -1; // the index of the triangle we found

  for( int i = 0; (i < model.modelHead->next->numFaces) && (!foundCollision); i++ ) // we're "guaranteed" that modelHead->next is the track model
  {
    // create the triangle and translate to world space
    currVertIdx0 = model.modelHead->next->face[i].vertIndex[ 0 ];
    currVertIdx1 = model.modelHead->next->face[i].vertIndex[ 1 ];
    currVertIdx2 = model.modelHead->next->face[i].vertIndex[ 2 ];

    // translate to world space
    vertId = model.modelHead->next->face[i].vertIndex[ 0 ]; //currVertIdx0 ];
    testTri.b.x = model.modelHead->next->vertex[vertId].x + worldpos[0];
    testTri.b.y = model.modelHead->next->vertex[vertId].y + worldpos[1];
    testTri.b.z = model.modelHead->next->vertex[vertId].z + worldpos[2];

    vertId = model.modelHead->next->face[i].vertIndex[ 1 ]; //currVertIdx1 ];
    testTri.e0.x = model.modelHead->next->vertex[vertId].x - testTri.b.x;
    testTri.e0.y = model.modelHead->next->vertex[vertId].y - testTri.b.y;
    testTri.e0.z = model.modelHead->next->vertex[vertId].z - testTri.b.z;

    vertId = model.modelHead->next->face[i].vertIndex[ 2 ]; //currVertIdx0 ]; //currVertIdx2 ];
    testTri.e1.x = model.modelHead->next->vertex[vertId].x - testTri.b.x;
    testTri.e1.y = model.modelHead->next->vertex[vertId].y - testTri.b.y;
    testTri.e1.z = model.modelHead->next->vertex[vertId].z - testTri.b.z;
    
    if( LinesegTriIntersect( cdLine, testTri, intersect) )
    {
      foundCollision = true;
      intersectedTriId = i;
    }
  }

  return intersectedTriId;
}

//---------------------------------------------------------------------------
// Returns a Triangle3 that corresponds to index triId
// Used with collision detection. First we find out if we hit a triangle,
// then we get that triangle's id, then use this function to return the
// triangle data.
//---------------------------------------------------------------------------
Triangle3 CTrack::getTriangle( int triId )
{
  if( triId >= model.modelHead->next->numFaces ) // we're "guaranteed" that modelHead->next is the track model
  {
    error.msg( "Error: getTriangle: invalid index" );
    triId = 0; // just set this so we can continue
               // Here's to hacked-in-the-last-minute coding! (sigh...)
  }

  //vect3 &worldpos = model.modelHead->instHead->next->worldpos;
  int vertId;
  Triangle3 retTri;

  // load up the triangle
  // b  = v0
  // e0 = v1-v0
  // e1 = v2-v0

  vertId = model.modelHead->next->face[triId].vertIndex[ 0 ];
  retTri.b.x = model.modelHead->next->vertex[vertId].x;
  retTri.b.y = model.modelHead->next->vertex[vertId].y;
  retTri.b.z = model.modelHead->next->vertex[vertId].z;

  vertId = model.modelHead->next->face[triId].vertIndex[ 1 ];
  retTri.e0.x = model.modelHead->next->vertex[vertId].x - retTri.b.x;
  retTri.e0.y = model.modelHead->next->vertex[vertId].y - retTri.b.y;
  retTri.e0.z = model.modelHead->next->vertex[vertId].z - retTri.b.z;

  vertId = model.modelHead->next->face[triId].vertIndex[ 2 ];
  retTri.e1.x = model.modelHead->next->vertex[vertId].x - retTri.b.x;
  retTri.e1.y = model.modelHead->next->vertex[vertId].y - retTri.b.y;
  retTri.e1.z = model.modelHead->next->vertex[vertId].z - retTri.b.z;

  return retTri;
}

//---------------------------------------------------------------------------
void CTrack::drawTriangle( Triangle3 currTri )
{
  vect3 vert0 = { currTri.b.x, currTri.b.y, currTri.b.z };
  vect3 vert1 = { vert0[0] + currTri.e0.x, 
                  vert0[1] + currTri.e0.y, 
                  vert0[2] + currTri.e0.z };

  vect3 vert2 = { vert0[0] + currTri.e1.x, 
                  vert0[1] + currTri.e1.y, 
                  vert0[2] + currTri.e1.z };

  // Draw the triangle
  glBegin( GL_TRIANGLES );
    glVertex3f( vert0[0], vert0[1], vert0[2] );
    glVertex3f( vert1[0], vert1[1], vert1[2] );
    glVertex3f( vert2[0], vert2[1], vert2[2] );
  glEnd();
}

//---------------------------------------------------------------------------
void CTrack::drawTriangleWire( Triangle3 currTri )
{
  glPushAttrib( GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT );
  glDisable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    drawTriangle( currTri );
  glPopAttrib();
}