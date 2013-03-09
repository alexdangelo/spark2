// CSplineDisp.cpp (Spark2)
// by Alex D'Angelo
// Created on May 24, 2000
// Last modified on May 24, 2000
//
// Notes: This sets up a splines's data so it can be displayed using OpenGL
//       
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
// Initializes the variables.  
//
void CSpline::initOpenGLData(tSpline *currSpline)
{
  currSpline->vertList  = NULL; // vertex array list for drawing using OpenGL
  currSpline->vertIndex = NULL; 
}

//---------------------------------------------------------------------------
// Simply encapsulates the separate conversions that need to be done
//
void CSpline::genVertArray(tSpline *currSpline)
{
  // cout << "Generating spline vertex array data for OpenGL" << endl;
  
  genVerts    (currSpline);
  genVertIndex(currSpline);
}
//---------------------------------------------------------------------------
// Creates a contiguous list of verts for OpenGL
//
void CSpline::genVerts(tSpline *currSpline)
{
  currSpline->vertList = new GLfloat[ currSpline->numVerts * 3 ]; // each node = (x,y,z)
  memset(currSpline->vertList, 0, sizeof(GLfloat) * currSpline->numVerts * 3 );
  
  int i;
  
  for(i = 0; i < currSpline->numVerts; i++) // think 320*y + x
  {
    currSpline->vertList[ 3 * i + 0 ] = currSpline->vert[i][0];
    currSpline->vertList[ 3 * i + 1 ] = currSpline->vert[i][1];
    currSpline->vertList[ 3 * i + 2 ] = currSpline->vert[i][2];
  }
}

//---------------------------------------------------------------------------
// Creates an index table (1, 2, 3, 4, ...) for the spline's verts
// This is done to use glDrawElements(), which supposedly is accelerated or
// better implemented than glDrawArrays() because it's used in Quake II/III
//
void CSpline::genVertIndex(tSpline *currSpline)
{
  currSpline->vertIndex = new GLuint[ currSpline->numVerts];
  
  memset(currSpline->vertList, 0, 
         sizeof(GLuint) * currSpline->numVerts );
  
  GLuint i;
  for(i = 0; i < (GLuint)currSpline->numVerts; i++)
  {
    currSpline->vertIndex[ i ] = i;
  }
}

//---------------------------------------------------------------------------
// Generates vertex array info for all the splines in this class
//
void CSpline::genVertArrayAll()
{
  tSpline *currSpline = splineHead->next;
  
  while(currSpline != splineHead)
  {
    genVertArray(currSpline);
    currSpline = currSpline->next;
  }
}

//---------------------------------------------------------------------------
// Draws on the screen. OpenGL is used for drawing in the inner function
//
void CSpline::drawAll()
{
  tSpline *currSpline = splineHead->next;
  
  while(currSpline != splineHead)
  {
    draw(currSpline);
    currSpline = currSpline->next;
  }
}

//---------------------------------------------------------------------------
// Draws on screen using OpenGL
//
void CSpline::draw(tSpline *currSpline)
{
  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glDisable( GL_TEXTURE_2D );
    glColor3f( 1.0f, 1.0f, 1.0f );

    glEnableClientState(GL_VERTEX_ARRAY);

    //glVertexPointer(3, GL_FLOAT, 0, currSpline->vertList); // why doesn't this work???!!!
    glVertexPointer(3, GL_FLOAT, 0, (GLfloat *)currSpline->vert);
  
    if(currSpline->isClosed)
    {
      glDrawElements(GL_LINE_LOOP, 
                     currSpline->numVerts, 
                     GL_UNSIGNED_INT, 
                     currSpline->vertIndex);
    
      //glDrawArrays(GL_LINE_LOOP, 0, currSpline->numVerts);      
    }
    else
    {
      glDrawElements(GL_LINES, 
                     currSpline->numVerts, 
                     GL_UNSIGNED_INT, 
                     currSpline->vertIndex);
    
      //glDrawArrays(GL_LINES, 0, currSpline->numVerts);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    
  glPopAttrib();
}

//-- E O F ------------------------------------------------------------------
