// CModelLinks.cpp (Spark2)
// by Alex D'Angelo
// Created on May 04, 2000
// Last modified on May 25, 2000 (added pragma)
//
// Overview: contains all the linked list and bin tree functions for CModel
//
//---------------------------------------------------------------------------

#include "ConvCommon.h"

//---------------------------------------------------------------------------
// Func: CModel::tNode *CModel::insertTree(tNode **root, char *modelName)
// 
// Desc: Creates the modelName tree
//
// Returns a pointer to the newly inserted modelName
//---------------------------------------------------------------------------
//
#pragma warning( push )
#pragma warning( disable : 4715 ) // 'CModel::insertTree' : not all control
                                  //  paths return a value
CModel::tNode *CModel::insertTree(tNode **root, char *modelName)
{
  if(*root == NULL) // we didn't find what we were looking for at all
  {
    (*root) = new tNode;
    
    (*root)->pMD            = NULL;
    (*root)->left           = NULL;
    (*root)->right          = NULL;
    (*root)->modelName      = NULL;
    
    (*root)->modelName      = new char[ strlen(modelName) + 1 ];
    strcpy((*root)->modelName, modelName);

    return (*root); // return a pointer to the model name we just added
  }
  else              // keep searching the rest of the tree
  {
    if( strcmp(modelName, (*root)->modelName) < 0 )         // branch left
    {
      insertTree( &(*root)->left, modelName );
    }
    else if( strcmp(modelName, (*root)->modelName) > 0 )    // branch right
    {
      insertTree( &(*root)->right, modelName );
    }
    else                                                    // don't branch
    {
      // cout << modelName << " is a duplicate model!" << endl;
      return NULL;
    }
  }
}
#pragma warning( pop ) // restore the 4715 warning
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void CModel::searchTree(tNode *root, tNode **target, char *modelName)
{
  if(root == NULL)  // we didn't find what we were looking for at all
  {
    target = NULL;
  }
  else if( strcmp(modelName, root->modelName) < 0 ) // branch left
  {
    searchTree(root->left, target, modelName);
  }
  else if( strcmp(modelName, root->modelName) > 0 ) // branch right
  {
    searchTree(root->right, target, modelName);
  }
  else
  {
    *target = root; // we found it! (the modelName in this case)
  }
}

//---------------------------------------------------------------------------
void CModel::linkModel(tModelData *new_model)
{
  new_model->next  = NULL;
  new_model->prev  = NULL;

  new_model->next  = modelHead->next;
  modelHead->next  = new_model;
  new_model->prev  = modelHead;
  new_model->prev  = new_model->next->prev;
  new_model->next->prev = new_model;
}


//---------------------------------------------------------------------------
// This func assumes that the model's data and inst have been released
//
void CModel::unlinkModel(tModelData **currModel)
{
  tModelData *temp = *currModel;

  (*currModel)->next->prev = (*currModel)->prev;
  (*currModel)->prev->next = (*currModel)->next;

  if((*currModel)->next == modelHead)
  {
    *currModel = (*currModel)->prev;
  }
  else
  {
    *currModel = (*currModel)->next;
  }

  delete temp;
}

//---------------------------------------------------------------------------
// Links the current instance to the current model
//
void CModel::linkInst(tModelInst *currInst, tModelData *currModel)
{
  currInst->next            = currModel->instHead->next;
  currModel->instHead->next = currInst;
  currInst->prev            = currModel->instHead;
  currInst->prev            = currInst->next->prev;
  currInst->next->prev      = currInst;
}

//---------------------------------------------------------------------------
// Unlinks the current instance from the current model and frees the instance
//
void CModel::unlinkInst(tModelInst **currInst, tModelData *currModel)
{
  tModelInst *temp = *currInst;

  (*currInst)->next->prev = (*currInst)->prev;
  (*currInst)->prev->next = (*currInst)->next;

  if((*currInst)->next == currModel->instHead)
  {
    *currInst = (*currInst)->prev;
  }
  else
  {
    *currInst = (*currInst)->next;
  }

  SAFE_DELARY(temp->modelInstName); // this needs to be done, I think
  SAFE_DELETE(temp);
}

//---------------------------------------------------------------------------
// Releases the binary tree from memory
//
void CModel::releaseBintree(tNode **root)
{
  if( (*root) == NULL )
  {
    return;
  }
  if( (*root)->left != NULL )
  {
    releaseBintree( &(*root)->left );
  }
  if( (*root)->right != NULL )
  {
    releaseBintree( &(*root)->right );
  }
  if( (*root) != NULL )
  {
    SAFE_DELARY( (*root)->modelName );
    SAFE_DELETE(*root);
  }
}

//-- E O F ------------------------------------------------------------------