// CModelInst.cpp (Spark2)
// by Alex D'Angelo
// Created on May 05, 2000
// Last modified on May 22, 2000
//
// Overview: contains all the model inst (instance) functions
//
//---------------------------------------------------------------------------
#include "ConvCommon.h"
#include <stdlib.h> // for rand()

//---------------------------------------------------------------------------
void CModel::addInst(tModelData *currModel, char *modelInstName)
{
  tModelInst *newInst = new tModelInst;

  initInst(newInst, modelInstName); 

  getInstData(newInst);         // reads inst data from the ASE file

  linkInst(newInst, currModel); // attach this new inst to the curr model's
                                // linked list

  if(newInst->isTextured)       // If one inst of the model is textured,
  {                             // the model should account for tex coords,
    currModel->isTextured = 1;  // whether they're used or not for all insts.
    // load the texture here?
  }

  numInsts++;                   // add one more inst to the total
}

//---------------------------------------------------------------------------
// initializes the tModelInst struct to zero and NULL and copies the 
// modelInstName into the struct
// 
void CModel::initInst(tModelInst *currInst, char *modelInstName)
{
  memset(currInst, 0, sizeof(tModelInst));

  currInst->prev = NULL;
  currInst->next = NULL;
 
  // assign the model inst name to the instance
  currInst->modelInstName = new char [ strlen(modelInstName) + 1 ];
  strcpy(currInst->modelInstName, modelInstName);
}

//---------------------------------------------------------------------------
// Reads inst data from the ASE file (pos, row0-3, etc)
// 
void CModel::getInstData(tModelInst *currInst)
{
/*
  read this stuff:

    *NODE_NAME "Box01"
  ...
    *TM_ROW0 1.0000	0.0000	0.0000
    *TM_ROW1 0.0000	1.0000	0.0000
    *TM_ROW2 0.0000	0.0000	1.0000
    *TM_ROW3 -37.2983	-14.3233	0.0000
    *TM_POS -37.2983	-14.3233	0.0000
*/
  bool meshNotRead = 1;
  
  while(!aseStream->eof() && meshNotRead) // and we don't reach "*MESH"
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*TM_ROW0"))
            *aseStream >> currInst->row0[0] // Transformation / unrotation matrix for the verts
                      >> currInst->row0[2] // remember, 3ds likes to switch y and z
                      >> currInst->row0[1];

    else if(!strcmp(buffer, "*TM_ROW1"))
            *aseStream >> currInst->row1[0] // remember, 3ds likes to switch y and z
                      >> currInst->row1[2] 
                      >> currInst->row1[1];

    else if(!strcmp(buffer, "*TM_ROW2"))
            *aseStream >> currInst->row2[0] // remember, 3ds likes to switch y and z
                      >> currInst->row2[2] 
                      >> currInst->row2[1];

    else if(!strcmp(buffer, "*TM_ROW3")) 
            *aseStream >> currInst->row3[0] // Probably the translation amount in the matrix
                      >> currInst->row3[2] // remember, 3ds likes to switch y and z
                      >> currInst->row3[1];

    else if(!strcmp(buffer, "*TM_POS"))
            *aseStream >> currInst->worldpos[0] 
                      >> currInst->worldpos[2] 
                      >> currInst->worldpos[1];

    else if(!strcmp(buffer, "*MESH"))
            meshNotRead = 0;

    else aseStream->getline(buffer, sizeof(buffer)); // finish reading the line and move on to the next
  }

//## NOTE ##
//-- 3ds' z is our -z ---------------------------
  currInst->worldpos[2] *= -1.0f; 

  currInst->row0[2] *= -1.0f;
  currInst->row1[2] *= -1.0f;
  currInst->row2[2] *= -1.0f;
  currInst->row3[2] *= -1.0f;

//-- Get Texture Id and Wireframe Color ---------
  bool foundWireframe = 0;
  bool done = 0;
  
  streampos here = aseStream->tellg(); // save where we are in the file
  
  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;

    if (!strcmp(buffer, "*MESH_FACE"))
    {
        int x = 0;
        x++;
    }
    
    if(!strcmp(buffer, "*MATERIAL_REF"))  // Read the texture id
    {
      *aseStream >> currInst->texId;
      currInst->isTextured = 1;
    }
    
    else if(!strcmp(buffer, "*WIREFRAME_COLOR"))  //*WIREFRAME_COLOR 0.3373	0.3373	0.3373
    {
      *aseStream >> currInst->wireframecolor[0]
                >> currInst->wireframecolor[1]
                >> currInst->wireframecolor[2];

      foundWireframe = 1;
    }
  
    else if(!strcmp(buffer, "*GEOMOBJECT")) //we've potentially read the texid and/or the wire color
      done = 1;

    else aseStream->getline(buffer, sizeof(buffer)); // finish reading the line and move on to the next 
  }
  
  aseStream->clear();      // we might have generated an eof
  aseStream->seekg(here);  //move back to saved position

  if(!foundWireframe) // generate a color (to 4 places), eg 0.4439
  {
    currInst->wireframecolor[0] = rand()%10000 * .0001f;
    currInst->wireframecolor[1] = rand()%10000 * .0001f;
    currInst->wireframecolor[2] = rand()%10000 * .0001f;
  }
}

//-- E O F ------------------------------------------------------------------