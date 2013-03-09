// CSpline.cpp (Spark2)
// by Alex D'Angelo
// Created on May 10, 2000
// Last modified on May 10, 2000
//
// Overview: reads in splines
//
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
CSpline::CSpline()
{
  initialized = 0;
  init();
}

//---------------------------------------------------------------------------
void CSpline::init()
{
  if(!initialized)
  {
    numSplines = 0;

    splineHead = NULL;
    splineHead = new tSpline;
    memset(splineHead, 0, sizeof(tSpline));
    splineHead->prev = splineHead;
    splineHead->next = splineHead;
    splineHead->vert = NULL;

    initialized = 1;
  }
}

//---------------------------------------------------------------------------
CSpline::~CSpline()
{
  unload();
}

//---------------------------------------------------------------------------
void CSpline::unload()
{
  releaseSplines();
  initialized = 0;
}

//---------------------------------------------------------------------------
void CSpline::add(char *splineName)
{
  tSpline *new_spline = new tSpline;
  initSpline (new_spline);
  getSpline  (new_spline);
  linkSpline (new_spline); // adds spline to linked-list of splines

  numSplines++;
}

//---------------------------------------------------------------------------
// Init current spline to 0 & NULL
//
void CSpline::initSpline(tSpline  *currSpline)
{
  memset(currSpline, 0, sizeof(tSpline));
  
  currSpline->prev = NULL;
  currSpline->next = NULL;
  currSpline->vert = NULL;
}

//---------------------------------------------------------------------------
// Get the spline
//
bool CSpline::getSpline(tSpline  *currSpline)
{
  bool goodToGo = 0;
  
  goodToGo = getSplineInfo(currSpline);
  
  if(goodToGo)
  {
    getSplineData(currSpline);
    return 1;
  }
  else
  {
    return 0;
  }
}

//---------------------------------------------------------------------------
// Get info, such as pos, row0-3, etc
//
bool CSpline::getSplineInfo(tSpline  *currSpline)
{
/*
    read this stuff:
    *NODE_NAME "splinename"
		*TM_ROW0 1.0000	0.0000	0.0000
		*TM_ROW1 0.0000	1.0000	0.0000
		*TM_ROW2 0.0000	0.0000	1.0000
		*TM_ROW3 -63.0571	-16.7710	0.0000
		*TM_POS -63.0571	-16.7710	0.0000
    ...
    *SHAPE_CLOSED // not always there
    *SHAPE_VERTEXCOUNT 84
*/
  memset(buffer, '\0', sizeof(char) * CONV_BUFFER_LEN);
  bool done = 0;

  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*NODE_NAME"))
    {
      // do nothing; we don't care what its name is
      // besides, that's already be processed before
      // this func has been called to determine if
      // its a track, map, or generic spline.
      
      ; // null statement;
    }

    else if(!strcmp(buffer, "*TM_ROW0"))
            *aseStream >> currSpline->row0[0] // Transformation / unrotation matrix for the verts
                      >> currSpline->row0[2] // remember, 3ds likes to switch y and z
                      >> currSpline->row0[1];

    else if(!strcmp(buffer, "*TM_ROW1"))
            *aseStream >> currSpline->row1[0] // remember, 3ds likes to switch y and z
                      >> currSpline->row1[2] 
                      >> currSpline->row1[1];

    else if(!strcmp(buffer, "*TM_ROW2"))
            *aseStream >> currSpline->row2[0] // remember, 3ds likes to switch y and z
                      >> currSpline->row2[2] 
                      >> currSpline->row2[1];

    else if(!strcmp(buffer, "*TM_ROW3")) 
            *aseStream >> currSpline->row3[0] // Probably the translation amount in the matrix
                      >> currSpline->row3[2] // remember, 3ds likes to switch y and z
                      >> currSpline->row3[1];

    else if(!strcmp(buffer, "*TM_POS"))
            *aseStream >> currSpline->worldpos[0] 
                      >> currSpline->worldpos[2] 
                      >> currSpline->worldpos[1];

    else if(!strcmp(buffer,"*SHAPE_CLOSED"))
            currSpline->isClosed = 1;

    else if(!strcmp(buffer,"*SHAPE_VERTEXCOUNT"))
    {
            *aseStream >> currSpline->numVerts;
            done = 1;
    }

    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }
  
  // 3ds' z is our negative z; hopefully this will work; I don't think this is the right way!
  currSpline->row0[2]     *= -1;
  currSpline->row1[2]     *= -1;
  currSpline->row2[2]     *= -1;
  currSpline->row3[2]     *= -1;
  currSpline->worldpos[2] *= -1;

  currSpline->wireframeColor[0] = 1.0f; // The spline is always white
  currSpline->wireframeColor[1] = 1.0f;
  currSpline->wireframeColor[2] = 1.0f;

  
  currSpline->vert = NULL;
  currSpline->vert = new vect3[ currSpline->numVerts * sizeof(tVert3) ];

  if(currSpline->vert == NULL)
  {
    error.msg( "Couldn't alloc enough mem for the spline" );
    // cout << "Couldn't alloc enough mem for the spline" << endl;
    return 0;
  }
  else
  {
    return 1;
  }
}

//---------------------------------------------------------------------------
// Get the verts
//
void CSpline::getSplineData(tSpline  *currSpline)
{
  int  index;
  int  vertCounter = 0;

  vertCounter = currSpline->numVerts;

  while(!aseStream->eof() && vertCounter)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer,"*SHAPE_VERTEX_KNOT")  ||
       !strcmp(buffer,"*SHAPE_VERTEX_INTERP") )
    {
      *aseStream >> index;
      
      *aseStream >> currSpline->vert[index][0]  //swap y and z for 3ds
                >> currSpline->vert[index][2]
                >> currSpline->vert[index][1];
 
      // in 3dsm negative z goes out of the screen, we want it to go in
      currSpline->vert[index][2] = -currSpline->vert[index][2];
      vertCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading the line and move on to the next
  }
}

//---------------------------------------------------------------------------
// center object at (0,0,0), otherwise instances won't work because
//  we translate WRT the center of the world.
//
void CSpline::translateSpline(tSpline  *currSpline)
{
  int i;

  for(i = 0; i < currSpline->numVerts; i++)
  {
    currSpline->vert[i][0] -= currSpline->worldpos[0];
    currSpline->vert[i][1] -= currSpline->worldpos[1];
    currSpline->vert[i][2] -= currSpline->worldpos[2];
  }
}

//---------------------------------------------------------------------------
// Releases all the splines
//
void CSpline::releaseSplines()
{
  if(splineHead != NULL) // if it hasn't been released yet
  {
    tSpline *currSpline = splineHead->next;
  
    // cout << "Releasing splines..." << endl;
    while(currSpline != splineHead)
    {
      unlinkSpline(&currSpline); // automatically does currSpline = currSpline->next
    }
  
    // cout << "Releasing spline head" << endl;
    SAFE_DELETE(splineHead);
  }
}

//---------------------------------------------------------------------------
// Links the current spline to the spline list
//
void CSpline::linkSpline(tSpline  *currSpline)
{
  currSpline->next        = splineHead->next;
  splineHead->next        = currSpline;
  currSpline->prev        = splineHead;
  currSpline->prev        = currSpline->next->prev;
  currSpline->next->prev  = currSpline;
}

//---------------------------------------------------------------------------
// Unlinks the current spline from the spline list
//
void CSpline::unlinkSpline(tSpline **currSpline)
{
  tSpline *temp = *currSpline;

  (*currSpline)->next->prev = (*currSpline)->prev;
  (*currSpline)->prev->next = (*currSpline)->next;

  if((*currSpline)->next == splineHead)
  {
    *currSpline = (*currSpline)->prev;
  }
  else
  {
    *currSpline = (*currSpline)->next;
  }

  SAFE_DELARY(temp->vert);
  SAFE_DELETE(temp);
}

//-- E O F ------------------------------------------------------------------