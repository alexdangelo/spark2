// CModelData.cpp (Spark2)
// by Alex D'Angelo
// Created on May 05, 2000
// Last modified on June 14, 2000
//
// Overview: contains all the modeldata reading functions
//
// June 14, 2000: Fixed tex coords! The textures are now displayed correctly,
//                instead of upside-down and backwards
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
//
void CModel::initModel(tModelData *newModel, char *modelNamePrefix)
{
  memset(newModel, 0, sizeof(tModelData));

  newModel->face        = NULL;
  newModel->vertex      = NULL;
  newModel->texCoord    = NULL;
  newModel->instHead    = NULL;
  newModel->modelName   = NULL;
  newModel->tex         = NULL;

  newModel->modelName   = new char[ strlen(modelNamePrefix) + 1 ];
  strcpy(newModel->modelName, modelNamePrefix);
    
  newModel->instHead    = new tModelInst;
  memset(newModel->instHead, 0, sizeof(tModelInst));
  newModel->instHead->prev = newModel->instHead;
  newModel->instHead->next = newModel->instHead;

  newModel->instHead->modelInstName = new char[ strlen(modelNamePrefix) + strlen("Head") + 1 ];
  strcpy(newModel->instHead->modelInstName, modelNamePrefix);
  strcat(newModel->instHead->modelInstName, "Head");

  
  initOpenGLData(newModel);
}

//---------------------------------------------------------------------------
// The ASE format for model inst + data is at the bottom of this CPP file
// 
void CModel::getModelData(tModelData *currModel)
{
  int done = 0;
  
  while(!aseStream->eof() && !done)
  {
    *aseStream >> buffer;

    if(!strcmp(buffer, "*TIMEVALUE"))
    {
      // do nothing. we aren't supporting anim yet
    }
    else if(!strcmp(buffer, "*MESH_NUMVERTEX"))
    {
      *aseStream >> currModel->numVerts;
    }
    else if(!strcmp(buffer, "*MESH_NUMFACES"))
    {
      *aseStream >> currModel->numFaces;
    }
    else if( !strcmp(buffer, "*MESH_VERTEX_LIST") )
    {
        // cout << "Reading verts" << endl;
        getVerts(currModel);
    }
    else if( !strcmp(buffer, "*MESH_FACE_LIST") )
    {
        // cout << "Reading faces" << endl;
        getFaces(currModel);
    }
    else if( !strcmp(buffer, "*MESH_NUMTVERTEX") )
    {
        *aseStream >> currModel->numTverts;
    }
    else if( !strcmp(buffer, "*MESH_TVERTLIST") )
    {
        // cout << "Reading tex verts" << endl;
        getTexVerts(currModel);
    }
    else if( !strcmp(buffer, "*MESH_NUMTVFACES") )
    {
        *aseStream >> currModel->numTfaces;
    }
    else if( !strcmp(buffer, "*MESH_TFACELIST") )
    {
        // cout << "Reading tex faces" << endl;
        getTexFaces(currModel);
    }
    else if( !strcmp(buffer, "*MESH_NUMCVERTEX") )
    {
        // do nothing. we aren't supporting vertex coloring (for now)
    }
    else if( !strcmp(buffer, "*MESH_NORMALS") )
    {
        // cout << "Reading normals" << endl;

        currModel->hasNormals = 1;
        getFaceNormals(currModel);
        done = 1;                                   // LOOK at loop termin.! 
                                                    // There isn't any useful
                                                    // data after the normals
                                                    // anyway (for this func)
    }
    else if( !strcmp(buffer, "*PROP_MOTIONBLUR") || // Hopefully one of these
             !strcmp(buffer, "*PROP_CASTSHADOW") || // tags exist! (if there
             !strcmp(buffer, "*PROP_RECVSHADOW") || // aren't any normals)
             !strcmp(buffer, "*MATERIAL_REF")     )
    {
      done = 1;
    }
    else aseStream->getline(buffer, sizeof(buffer));

  }

  translateModel(currModel);

  // cout << endl;
}

//---------------------------------------------------------------------------
// Returns 0 if not enough mem for verts and faces can be allocated
//
bool CModel::getVerts(tModelData *pM)
{
  int index;
  int vertexCounter = 0;

  // ----------
  // Alloc mem
  // ----------
  pM->vertex = NULL;
  pM->vertex = new tVert3[ pM->numVerts ];

  if(pM->vertex == NULL)
  {
    // cout << "Error: getVerts: couldn't alloc enough mem for verts in "
    //      << pM->modelName << endl;

    return 0;  
  }
  else if(pM->numVerts == 0)
  {
    // cout << "Error: getVerts: there are no verts (number of verts is 0)! (for "
    //      << pM->modelName << " )" << endl;
    
    return 0;
  }

  // ----------
  // Get verts
  // ----------
  vertexCounter = pM->numVerts;

  while(!aseStream->eof() && vertexCounter)
  {
    *aseStream >> buffer;

    if(!strcmp(buffer, "*MESH_VERTEX"))
    {
      *aseStream >> index;
      *aseStream >> pM->vertex[index].x //swap y and z for 3ds
                >> pM->vertex[index].z
                >> pM->vertex[index].y;
 
      // in 3ds negative z goes out of the screen; we want it to go in
      pM->vertex[index].z = -pM->vertex[index].z;
      vertexCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }

  return 1;
}

//---------------------------------------------------------------------------
bool CModel::getFaces(tModelData *pM)
{
  int index;
  int faceCounter;
  char dummy[4];
  
  // ----------
  // Alloc mem
  // ----------
  pM->face   = NULL;
  pM->face   = new tTriangle[ pM->numFaces ]; // this allocates enough mem
                                              //  for all texcoords, normals,
                                              //  whether they exist or not,
                                              //  as a by-product
  if(pM->face == NULL)
  {
    // cout << "Error: getFaces: couldn't alloc enough mem for faces in "
    //      << pM->modelName << endl;

    return 0;  
  }
  else if(pM->numFaces == 0)
  {
    // cout << "Error: getFaces: there are no faces (number of faces is 0)! "
    //      << "(for " << pM->modelName << " )" << endl;
    
    return 0;
  }

  // ----------
  // Get faces
  // ----------
  faceCounter = pM->numFaces;

  while(!aseStream->eof() && faceCounter)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*MESH_FACE"))
    {
      *aseStream >> index;                                 // 0

      *aseStream >> dummy                                  // :
                >> dummy >> pM->face[index].vertIndex[0]  // A: 5
                >> dummy >> pM->face[index].vertIndex[1]  // B: 0
                >> dummy >> pM->face[index].vertIndex[2]; // C: 6

      faceCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }
  
  return 1;
}

//-----------------------------------------------------------------------------
// Note: the modelInst data-gathering is and must *always* be called before
//       getting the model data. 
//       
//       the texture id is read during the modelInst data-gathering code
//
bool CModel::getTexVerts(tModelData *pM)
{
  int index;
  int tcoordCounter;
  
  // ----------
  // Alloc mem
  // ----------
  pM->texCoord   = NULL;
  pM->texCoord   = new tVert2[ pM->numTverts ];

  if(pM->texCoord == NULL)
  {
    // cout << "Error: getTexVerts: couldn't alloc enough mem for faces in "
    //      << pM->modelName << endl;

    return 0;  
  }
  else if(pM->numTverts == 0)
  {
    // cout << "Error: getFaces: there are no tex verts (number of tverts is 0)! "
    //      << "(for " << pM->modelName << " )" << endl;
    
    return 0;
  }
  else
  {
    memset(pM->texCoord, 0, sizeof(tVert2) * pM->numTverts );
  }
  
  // -------------
  // Get textures
  // -------------
  // Derived form (obsolete): tTexture *currTexture = (tTexture *)CTexture::find(pM->instHead->next->texId);
  // Local form: CTexture::tTexture *currTexture = (CTexture::tTexture *)tex->find(pM->instHead->next->texId);
  pM->tex = (CTexture::tTexture *)texData->find(pM->instHead->next->texId);
  
  if(pM->tex == NULL)
  {
    // cout << "Couldn't locate information for tex id " 
    //      << pM->instHead->texId << endl;
    return 0;
  }

  tcoordCounter = pM->numTverts;

  while(!aseStream->eof() && tcoordCounter)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*MESH_TVERT"))
    {
      *aseStream >> index;
      *aseStream >> pM->texCoord[index].x
                >> pM->texCoord[index].y; // we can ignore the w (aka z) value

      // Normally it would be:
      // pM->texCoord[index].x *= pM->tex->uTile;
      // pM->texCoord[index].y *= pM->tex->vTile;
      //
      // However, we've changed coord system for OpenGL, so the converted way is:
      // pM->texCoord[index].x *= pM->tex->uTile;
      // pM->texCoord[index].y = (1 - pM->texCoord[index].y) * pM->tex->vTile;

      pM->texCoord[index].x *= pM->tex->uTile;
      pM->texCoord[index].y = (1 - pM->texCoord[index].y) * pM->tex->vTile;
 
      tcoordCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }
  
  return 1;
}

//-----------------------------------------------------------------------------
// There might potentially be a bug here, with the amount of mem being alloc'd
// by the getFace() func. If (numTexFaces > numFaces) then we won't have enough
// mem alloc'd. However, I don't think that will happen (hopefully).
// 
bool CModel::getTexFaces(tModelData *pM)
{
  int index;
  int texFaceCounter;

  texFaceCounter = pM->numTfaces;

  while(!aseStream->eof() && texFaceCounter)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*MESH_TFACE"))
    {
      *aseStream >> index;
      
      *aseStream >> pM->face[index].tcoordIndex[0]
                >> pM->face[index].tcoordIndex[1]
                >> pM->face[index].tcoordIndex[2];
 
      texFaceCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }
  
  return 1;
}

//-----------------------------------------------------------------------------
void CModel::getFaceNormals(tModelData *pM)
{
  int index;
  int numFaceNormalsCounter;

  numFaceNormalsCounter = pM->numFaces; // implied; each face has a normal

  while(!aseStream->eof() && numFaceNormalsCounter)
  {
    *aseStream >> buffer;
    
    if(!strcmp(buffer, "*MESH_FACENORMAL"))
    {
      *aseStream >> index;
      
      *aseStream >> pM->face[index].norm[0]  // 3ds likes to swap y & z; swap it back 
                >> pM->face[index].norm[2]  
                >> pM->face[index].norm[1];
 
	    // in 3dsm negative z goes out of the screen, we want it to go in
      pM->face[index].norm[2] = -pM->face[index].norm[2];

      numFaceNormalsCounter--;
    }
    else aseStream->getline(buffer, sizeof(buffer)); // finish reading line
  }
}

//-----------------------------------------------------------------------------
// 3d studio pre-scales everything, but doesn't translate to (0,0,0)
//
void CModel::translateModel(tModelData *pMD)
{
  int i;

  //center object at (0,0,0), otherwise instances
  // won't work because we translate WRT the center of the world
  //then scale the object
  for(i = 0; i < pMD->numVerts; i++)
  {
    pMD->vertex[i].x -= pMD->instHead->next->worldpos[0];
    pMD->vertex[i].y -= pMD->instHead->next->worldpos[1];
    pMD->vertex[i].z -= pMD->instHead->next->worldpos[2];
  }
}

//-----------------------------------------------------------------------------
// For reference for the main model-data gathering loop
//
/* 
*GEOMOBJECT {                             <------------|
	*NODE_NAME "track"                                   |
	*NODE_TM {                                           |
		*NODE_NAME "track"                                 |
		*INHERIT_POS 0 0 0                                 |
		*INHERIT_ROT 0 0 0                                 |
		*INHERIT_SCL 0 0 0                                 |
		*TM_ROW0 1.0000	0.0000	0.0000                     -
		*TM_ROW1 0.0000	1.0000	0.0000                     
		*TM_ROW2 0.0000	0.0000	1.0000                modelInst
		*TM_ROW3 -13.2597	-1.8416	0.0000                   
		*TM_POS -13.2597	-1.8416	0.0000                   -
		*TM_ROTAXIS 0.0000	0.0000	0.0000                 |
		*TM_ROTANGLE 0.0000                                |
		*TM_SCALE 1.0000	1.0000	1.0000                   |
		*TM_SCALEAXIS 0.0000	0.0000	0.0000               |
		*TM_SCALEAXISANG 0.0000                            |
	}                                       <------------|
	*MESH {
		*TIMEVALUE 0
		*MESH_NUMVERTEX 4
		*MESH_NUMFACES 2
		*MESH_VERTEX_LIST {
			*MESH_VERTEX    0	-171.6390	-131.4917	0.0000
			*MESH_VERTEX    1	145.1197	-131.4917	0.0000
			*MESH_VERTEX    2	-171.6390	127.8085	0.0000
			*MESH_VERTEX    3	145.1197	127.8085	0.0000
		}
		*MESH_FACE_LIST {
			*MESH_FACE    0:    A:    2 B:    0 C:    3 AB:    1 BC:    0 CA:    1	 *MESH_SMOOTHING 1 	*MESH_MTLID 0
			*MESH_FACE    1:    A:    1 B:    3 C:    0 AB:    1 BC:    0 CA:    1	 *MESH_SMOOTHING 1 	*MESH_MTLID 0
		}
		*MESH_NUMTVERTEX 8
		*MESH_TVERTLIST {
			*MESH_TVERT 0	0.0000	0.0000	0.0000
			*MESH_TVERT 1	1.0000	0.0000	0.0000
			*MESH_TVERT 2	0.0000	0.0000	0.0000
			*MESH_TVERT 3	1.0000	0.0000	0.0000
			*MESH_TVERT 4	0.0000	0.0000	0.0000
			*MESH_TVERT 5	1.0000	0.0000	0.0000
			*MESH_TVERT 6	0.0000	1.0000	0.0000
			*MESH_TVERT 7	1.0000	1.0000	0.0000
		}
		*MESH_NUMTVFACES 2
		*MESH_TFACELIST {
			*MESH_TFACE 0	6	4	7
			*MESH_TFACE 1	5	7	4
		}
		*MESH_NUMCVERTEX 0
		*MESH_NORMALS {
			*MESH_FACENORMAL 0	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 2	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 0	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 3	0.0000	0.0000	1.0000
			*MESH_FACENORMAL 1	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 1	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 3	0.0000	0.0000	1.0000
				*MESH_VERTEXNORMAL 0	0.0000	0.0000	1.0000
		}
	}
	*PROP_MOTIONBLUR 0
	*PROP_CASTSHADOW 1
	*PROP_RECVSHADOW 1
	*MATERIAL_REF 0
*/

//-- E O F ------------------------------------------------------------------