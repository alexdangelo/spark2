// CModelDisp.cpp (Spark2)
// by Alex D'Angelo
// Created on May 20, 2000
// Last modified on May 24, 2000
//
// Note: This sets up a model's data so it can be displayed using OpenGL
//       
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
// Initializes the variables.  
//
void CModel::initOpenGLData(tModelData *currModel)
{
  currModel->vertList      = NULL;  // vertex array list for drawing
  currModel->texCoordList  = NULL;  //  using OpenGL
  currModel->normalList    = NULL;
  currModel->faceIndex     = NULL;
}

//---------------------------------------------------------------------------
// Simply encapsulates the separate conversions that need to be done
//
void CModel::genVertArray(tModelData *currModel)
{
  // cout << "Generating vertex array data for OpenGL" << endl;
  genVerts(currModel);

  if(currModel->isTextured)
  {                         // this will have to search through the insts    
    genTexCoords(currModel);//  until it finds a textured inst and then uses
  }                         //  that data                                    
                             
  if(currModel->hasNormals)
  {
    genNormals(currModel);
  }

  genFaces(currModel);

  // used for debugging to make sure the model's data is correct:
  // writeVertArrayToFile("vertarray.txt", currModel);
}

//---------------------------------------------------------------------------
// Creates a contiguous list of verts for OpenGL
//
void CModel::genVerts(tModelData *currModel)
{
  // 1 face == 3 sets of 3 verts (x,y,z)
  currModel->vertList = new GLfloat[ currModel->numFaces * 3 * 3]; 
  memset(currModel->vertList, 0, sizeof(GLfloat) * currModel->numFaces*3*3);
  
  currModel->numVertListVerts = currModel->numFaces * 3; 

  int i,j;
  int currVertexNum = 0;
  
  for(j = 0; j < currModel->numFaces; j++) // think 320*y + x
  {
    for(i = 0; i < 3; i++) // 3 sets of (x,y,z) verts per face
    {
      // 3*3*j selects the current face "block" of the array
      //       ( 9 * j == "3 sets of 3 coords"-sized blocks )
      // 3*i selects which coordinate we're writing to
      // 0,1,2 are the offsets for (x,y,z)
      
      currVertexNum = currModel->face[j].vertIndex[i];

      currModel->vertList[ 3*3*j + 3*i + 0 ] = currModel->vertex[currVertexNum].x;
      currModel->vertList[ 3*3*j + 3*i + 1 ] = currModel->vertex[currVertexNum].y;
      currModel->vertList[ 3*3*j + 3*i + 2 ] = currModel->vertex[currVertexNum].z;
    }
  }
}

//---------------------------------------------------------------------------
// Creates a contiguous list of texture coords for OpenGL (if the model has
//  texture coord data).
//
void CModel::genTexCoords(tModelData *currModel)
{
  currModel->texCoordList = new float[ currModel->numFaces * 2 * 3 * 2]; // 1 face == 3 sets of 2 coords -- (s0,t0),(s1,t1),(s2,t2)
  memset(currModel->texCoordList, 0, sizeof(float) * currModel->numFaces * 2 * 3 * 2);
  
  int i,j;
 
  for(j = 0; j < currModel->numFaces; j++) // think 320*y + x
  {
    for(i = 0; i < 3; i++) // 3 sets of (s,t) texcoords per face
    {
      // 3*2*j selects the current face "block" of the array ( 6 * j == "3 sets of 2 texcoords"-sized blocks )
      // 2*i   selects which coordinate we're writing to
      // 0,1   are the offsets for (s,t)

      currModel->texCoordList[ 3*2*j + 2*i + 0 ] = currModel->texCoord[currModel->face[j].tcoordIndex[i]].x + currModel->tex->uOffset;
      currModel->texCoordList[ 3*2*j + 2*i + 1 ] = currModel->texCoord[currModel->face[j].tcoordIndex[i]].y + currModel->tex->vOffset;
    }
  }
}

//---------------------------------------------------------------------------
// Creates a contiguous list of normals for OpenGL (if the model has normal
//  data).
//
void CModel::genNormals(tModelData *currModel)
{
  // Note: 1 face == 3 sets of 3 verts (x,y,z)
  currModel->normalList = new GLfloat[ currModel->numFaces * 3 * 3 ]; 
  memset(currModel->normalList, 0, sizeof(GLfloat)*currModel->numFaces*3*3);

  // OpenGL needs a normal for each vertex to do lighting and other things.
  // For each face there are 3 coords and they each have the same
  // normal, so take the face normal and write it out three times.

  int i,j;

  for(j = 0; j < currModel->numFaces; j++)
  {
    for(i = 0; i < 3; i++)
    {
      // 3*3*j selects the current face "block" of the array 
      // 3*i   selects which coordinate we're writing to
      // 0,1,2 are the offsets for (x,y,z)

      currModel->normalList[ 3*3*j + 3*i + 0 ] = currModel->face[j].norm[0];
      currModel->normalList[ 3*3*j + 3*i + 1 ] = currModel->face[j].norm[1];
      currModel->normalList[ 3*3*j + 3*i + 2 ] = currModel->face[j].norm[2];
    }
  }

}

//---------------------------------------------------------------------------
// Essentially creates an index table (1, 2, 3, ...) for the model's verts
// This is done to use glDrawElements(), which supposedly is accelerated or
// better implemented than glDrawArrays() because it's used in Quake II/III
//
void CModel::genFaces(tModelData *currModel)
{
  currModel->faceIndex = new GLuint[ currModel->numFaces * 3 + 1 ];
  memset(currModel->faceIndex, 0, 
         sizeof(GLuint) * (currModel->numFaces * 3 + 1) );

  for(GLuint i = 0; i < (GLuint)currModel->numFaces * 3; i++)
  {
    currModel->faceIndex[i] = i;
  }

}

//-----------------------------------------------------------------------------
// Generates vertex array info for all the models in this class
//
void CModel::genVertArrayAll()
{
  tModelData *currModel = modelHead->next;
  
  while(currModel != modelHead)
  {
    genVertArray(currModel);
    currModel = currModel->next;
  }
}

//-----------------------------------------------------------------------------
// Draws on the screen. OpenGL is used for drawing in the inner function
//
void CModel::drawAll()
{
  tModelData *currModel = modelHead->next;
  tModelInst *currInst = NULL;

  glPushAttrib(GL_CURRENT_BIT); // save the current drawing color

  while(currModel != modelHead)
  {
    currInst = currModel->instHead->next;
    
    while(currInst != currModel->instHead)
    {
      glPushMatrix();

        glTranslatef(currInst->worldpos[0], 
                     currInst->worldpos[1], 
                     currInst->worldpos[2]);

        glBindTexture(GL_TEXTURE_2D, currInst->glTexId);
        glColor3fv((GLfloat *)currInst->wireframecolor);
    
        draw(currModel, currInst);
        currInst = currInst->next;

      glPopMatrix();
    }
    
    currModel = currModel->next;
  }
  
  glPopAttrib();
}
//---------------------------------------------------------------------------
//
void CModel::draw(tModelData *currModel, tModelInst *currInst)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, currModel->vertList);  

  if(currModel->isTextured)
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, currModel->texCoordList);
  }

  if(currModel->hasNormals)
  {
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, currModel->normalList);
  }

    glDrawElements(GL_TRIANGLES, 
                   currModel->numVertListVerts, 
                   GL_UNSIGNED_INT, 
                   currModel->faceIndex);
  //glDrawArrays(GL_TRIANGLES, 0, currModel->numVertListVerts);

  glDisableClientState(GL_VERTEX_ARRAY);

  if(currModel->isTextured)
  {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  if(currModel->hasNormals)
  {
    glDisableClientState(GL_NORMAL_ARRAY);
  }
}

//-----------------------------------------------------------------------------
void CModel::writeVertArrayToFile(char *outFile, tModelData *currModel)
{
  // --------------------
  // Open file for output
  // --------------------

  fstream outStream;
  outStream.open(outFile, ios::out | ios::app);

  if(!outStream.is_open()) 
  {
    error.msg( "ERROR:  Cannot open %s for conversion (writeVertArrayToFile)", outFile);
    
    outStream.close();
  }
  else
  {
    // cout << "Opened " << outFile << " successfully." << endl;
  }
  
  outStream << "--------------------------------------------" << endl;  
  outStream << "Start of " << currModel->modelName << " data" << endl;
  outStream << "--------------------------------------------" << endl;

  // ----------------
  // Write out info
  // ----------------
  outStream << endl;
  outStream << "Num verts  = " << currModel->numVerts  << endl;
  outStream << "Num tverts = " << currModel->numTverts << endl;
  outStream << "Num faces  = " << currModel->numFaces  << endl;
  outStream << "Num tfaces = " << currModel->numTfaces << endl;
  outStream << endl;

  int i;  
  // ----------------
  // Write out verts
  // ----------------
  outStream << "Verts:" << endl;
  outStream << "-----------------" << endl;
  for(i = 0; i < currModel->numFaces * 3 * 3; i++)
  {
    if(i % 3 == 0) outStream << endl;
    outStream << currModel->vertList[i] << " ";
  }
  
  outStream << endl << endl;
  
  // -------------------
  // Write out texcoords
  // -------------------
  if(currModel->isTextured)
  {
    outStream << "TexCoords:" << endl;
    outStream << "-----------------" << endl;
    for(i = 0; i < currModel->numFaces * 3 * 2; i++)
    {
      if(i % 2 == 0) outStream << endl;
      outStream << currModel->texCoordList[i] << " ";
    }
  }
  
  outStream << endl << endl;

  // -----------------
  // Write out normals
  // -----------------
  if(currModel->hasNormals)
  {
    outStream << "Normals:" << endl;
    outStream << "-----------------" << endl;
    for(i = 0; i < currModel->numFaces * 3 * 3; i++)
    {
      if(i % 3 == 0) outStream << endl;
      outStream << currModel->normalList[i] << " ";
    }
  }

  outStream << endl << endl;

  // --------------------
  // Done with this model
  // --------------------

  outStream << "--------------------------------------------" << endl;  
  outStream << "End of " << currModel->modelName << " data"   << endl;
  outStream << "--------------------------------------------" << endl;
  outStream.close();
}

//-- E O F ------------------------------------------------------------------
