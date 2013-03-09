// CModel.cpp (Spark2)
// by Alex D'Angelo
// Created on April 26, 2000
// Last modified on May 10, 2000
//
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
CModel::CModel()
{
  initialized = 0;
  init();
}

//---------------------------------------------------------------------------
void CModel::init()
{
  if(!initialized)
  {
    bintreeRoot = NULL;

    modelHead = new tModelData;           // init model list

    initModel(modelHead, "model");
    modelHead->prev = modelHead;
    modelHead->next = modelHead;

    numModels = 0;
    numInsts  = 0;

    id = 0;

    initialized = 1;
  }
}

//---------------------------------------------------------------------------
CModel::~CModel()
{
  unload();
}

//---------------------------------------------------------------------------
void CModel::unload()
{
  if(modelHead != NULL) // only free the models once
  {
    tModelData *currModel = modelHead->next;
  
    while(currModel != modelHead)
    {
      // cout << "------------------------------------------" << endl;
      // cout << "Releasing " << currModel->modelName << endl;
      releaseModel(currModel);
      unlinkModel(&currModel); // automatically does currModel=currModel->next
    }
    // cout << "------------------------------------------" << endl;
    // cout << "Releasing model head" << endl;
    SAFE_DELETE(modelHead);

    // cout << "Releasing bintree" << endl;
    releaseBintree(&bintreeRoot);
    // cout << "Done releasing bintree" << endl;
  }
  
  texData = NULL; // make sure the texture pointer doesn't point anywhere
                  // (the stuff it points to is deallocated in CTexture)

  initialized = 0;
}

//---------------------------------------------------------------------------
void CModel::add(char *modelName)
{
  tNode *currModel = NULL;

  // Get the modelname prefix:
  // A model name, such as "tree04", has a prefix of "tree". The number implies that it's a copy of an existing model.
  char *modelNamePrefix = new char [ strlen(modelName) + 1 ];
  strcpy(modelNamePrefix, modelName);
  int pos = strcspn( modelName, "0123456789 " ); // search string for any of these chars
  modelNamePrefix[pos] = '\0';                   // terminate it at the first occurance of a number

  searchTree(bintreeRoot, &currModel, modelNamePrefix); // search the bintree for any duplicate models

  if( currModel != NULL ) // we found the name ==> the model data already exists. create an instance
  {
    // cout << "Adding another instance of the "<< modelNamePrefix << " model." << endl;
    addInst(currModel->pMD, modelName); // the model name in the tree contains a pointer to the actual data
  }
  else // create a new model and get the model inst and data
  {
    tModelData  *new_model = new tModelData;
    initModel   (new_model, modelNamePrefix);
    
    // cout << "Creating instance" << endl;

    addInst     (new_model, modelName);
    getModelData(new_model);
    linkModel   (new_model); // adds model to linked list of models

    // add a new entry to the bintree and connect it with the model. VERY IMPORTANT!
    // We use this connection between the bintree and models for adding an inst
    tNode *new_model_node = insertTree(&bintreeRoot, modelNamePrefix);
    new_model_node->pMD = new_model;

    numModels++;
  }


  SAFE_DELARY(modelNamePrefix);
}

//---------------------------------------------------------------------------
// Func: void CModel::loadAllTextures()
//
// Desc: Loads all the textures for each inst of each model and assigns
//       OpenGL texture names (ids). It does not load duplicate textures.
//
//      for each model
//        for each inst
//          load the tex
//---------------------------------------------------------------------------
//
void CModel::loadAllTextures()
{
  tModelData *currModel = modelHead->next;
  tModelInst *currInst = NULL;

  while(currModel != modelHead)
  {
    currInst = currModel->instHead->next;
    
    while(currInst != currModel->instHead)
    {
      if(currInst->isTextured)
      {
        currInst->glTexId = texData->loadTexWithId(currInst->texId);
      }
      currInst = currInst->next;
    }
    
    currModel = currModel->next;
  }
}

//---------------------------------------------------------------------------
// Do we need the if() statement, since SAFE_DELARY already does that?
//
void CModel::releaseModel(tModelData *currModel)
{
  // -----------------------
  // Release curr model data
  // -----------------------

  // cout << "Releasing verts, faces, etc" << endl;

  if(currModel->vertex != NULL)
    SAFE_DELARY(currModel->vertex);

  if(currModel->face != NULL)
    SAFE_DELARY(currModel->face);

  if(currModel->texCoord != NULL)
    SAFE_DELARY(currModel->texCoord);

  if(currModel->modelName != NULL)
    SAFE_DELARY(currModel->modelName);

  currModel->tex = NULL;

  // --------------------------------
  // Release all insts for curr model
  // --------------------------------
  tModelInst *currInst = currModel->instHead->next;
  
  while(currInst != currModel->instHead)
  {
    // cout << "releasing instance: " << currInst->modelInstName << endl;
    unlinkInst(&currInst, currModel); // automatic: currInst = currInst->next
    numInsts--;
  }

  // cout << "releasing instance: " 
  //      << currModel->instHead->modelInstName << endl;
  SAFE_DELARY(currModel->instHead->modelInstName);
  SAFE_DELETE(currModel->instHead);
  
  SAFE_DELARY(currModel->vertList); // Generated lists for drawing using OpenGL
  SAFE_DELARY(currModel->faceIndex);
  
  numModels--;
}

//-- E O F ------------------------------------------------------------------