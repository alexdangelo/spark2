// ConvCommon.h (Spark2)
// by Alex D'Angelo
// Created on April 26, 2000
// Last modified July 20, 2000
//
// July 20, 2000:
// Got rid of CBase. It wasn't doing anything.
//
// Notes: Model instances still cannot be rotated, or one model
//        can define the rotation and all the other models must
//        have the same rotation. This needs to be fixed! (May 08, 2000)
//
//        If I were to rewrite this again, I'd use templates for all the
//        linked-list functions.
//
//        To use the objects, each object's stream must be set to the 
//        current open stream using setFileStream(ifstream stream). If this
//        isn't done then the object will fail
//
//---------------------------------------------------------------------------
#ifndef CONVCOMMON_H
#define CONVCOMMON_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h> // for atoi()
#include "Global.h"
#include "Vector.h"

using namespace std;

//---------------------------------------------------------------------------
/* already defined in Global.h
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELARY(p)  { if(p) { delete [] (p);  (p)=NULL; } }
*/
const int CONV_BUFFER_LEN = 500; // the CONVerter buffer len; A unique name
                                 //  so we don't use other BUFFER_LENs

//---------------------------------------------------------------------------
class CLight
{
  protected:
    struct tLight
    {
      tLight *prev;       // more fun with linked-lists!
      tLight *next;
      
      int   id;
      char  typeName[10]; // Omni, Target, ...
      bool  isActive;     // bool on or off. 1 is on, 0 is off
      bool  hasShadow;    // bool 1 to cast shadow, 0 to not cast shadow //*LIGHT_SHADOWS Off
      vect3 color;        // *LIGHT_COLOR 1.0000	1.0000	1.0000
      float intens;       // *LIGHT_INTENS 1.0000
      vect3 worldpos;     // vect3

      vect3 targetpos;    // vect3 if this is a spotlight, define the target
    };

    tLight *lightHead;    // headlight? light snack? running the speed of light? <g>
    int    numLights;

  protected: //public: // I don't know why this used to be public... (July 20, 2000)
    ifstream* aseStream;
    char buffer[CONV_BUFFER_LEN];

  public:
    CLight();
    ~CLight();
    void init();          // CLight() calls this; essentially a constructor-in-a-function
    bool initialized;     // 0 = not initialized, 1 = initialized

    void add();           // public interface for adding
    void unload();        // frees all the lights from memory
  
    void setFileStream(ifstream *stream)
    {
      aseStream = stream;
    }

  protected:
    void initLight     (tLight  *currLight);
    void getLight      (tLight  *currLight);
    void getLightParams(tLight  *currLight);
    
    void releaseLights ();
    void linkLight     (tLight  *currLight);
    void unlinkLight   (tLight **currLight);

    char *findString   (char *string);

};

//---------------------------------------------------------------------------
class CSpline
{
  protected:
    struct tSpline
    {
      tSpline *prev;          // yep, this is a linked-list too
      tSpline *next; 

      //int     id;             // the spline id (do we still need this?)
      bool    isClosed;       // bool is the track a closed loop?
      int     numVerts;       // int
      vect3   *vert;          // tVert3 *(float[3]) usage: vert[i][0/1/2]
      vect3   wireframeColor; // vect3 in case we want to view the spline
      float   segmentLength;  // the dist between two nodes (if const for all nodes)

      //-- Pos ----------------------------------------
      vect3   worldpos;       // vect3

      //-- Rotation Transform info --------------------
      vect3   row0;           // vect3
      vect3   row1;           // vect3
      vect3   row2;           // vect3
      vect3   row3;           // vect3

      GLfloat   *vertList;     // vertex array list for drawing using OpenGL
      GLuint    *vertIndex;    // index of all the verts -- 1, 2, 3, 4, ...
    };

  public: // public until I find a better way to access a spline's data, then it'll be protected (July 20, 2000)
    tSpline *splineHead;                        // splineHead? splendid :P
  protected:
    int  numSplines;                            // number of splines depending on what this
                                                //   object is used in, ie either normal
                                                //   splines or track splines, but not both. 

  protected:
    void initOpenGLData(tSpline *currSpline);
    void genVertArray  (tSpline *currSpline); // creates vertex array for drawing the spline using OpenGL
    void genVerts      (tSpline *currSpline); // gen a continous list of verts
    void genVertIndex  (tSpline *currSpline); // gen index of all the verts -- 1, 2, 3, 4, ...
    virtual void draw  (tSpline *currSpline); // draws in OpenGL using vertex arrays; include opengl stuff elsewhere

  public:
    void genVertArrayAll();                     // Generates vert arrays for all of the splines
    void drawAll();                             // draws all the splines!

  private: //public: // I don't know why this used to be public... (July 20, 2000)
    ifstream *aseStream;
    char buffer[CONV_BUFFER_LEN];

  public:
    CSpline();
    ~CSpline();
    void init();                                // CSpline() calls this; essentially a constructor-in-a-function
    bool initialized;                           // 0 = not initialized, 1 = initialized

  public:
    void add(char *splineName);                 // pub interface for adding
    void unload();                              // calls releaseSplines()

  protected:
    void initSpline     (tSpline  *currSpline); // set to 0 & NULL
    bool getSpline      (tSpline  *currSpline); // get the spline
    bool getSplineInfo  (tSpline  *currSpline); // get the spline info...
    void getSplineData  (tSpline  *currSpline); // get the spline verts
    void releaseSplines ();                     // unlinks, frees all splines
    void translateSpline(tSpline  *currSpline); // center it at (0,0,0)
    void linkSpline     (tSpline  *currSpline); // link to spline list
    void unlinkSpline   (tSpline **currSpline); // unlinks & frees from spline list

  public:
    void setFileStream(ifstream *stream)
    {
      aseStream = stream;
    }
};

//---------------------------------------------------------------------------
class CTexture
{
  public:
    struct tTexture
    {
      tTexture *prev;         // yep, it's a linked list
      tTexture *next;

      char  *texFilename;     // texture filename
      unsigned int texId;     // texture 3ds id
      GLuint glTexId;         // OpenGL tex "name" (for glBindTexture)
      bool  isLoaded;         // has the tex already been loaded?
      float uTile;            // u tiling of texture
      float vTile;            // v tiling of texture
      float uOffset;          // u offset of texture
      float vOffset;          // v offset of texture
      GLenum face;            // GLenum To what this texture is applied:
                              //   GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
                              //   Default: GL_FRONT
      vect3 ambient;          // vect3 ambient component of texture
      vect3 diffuse;          // vect3 diffuse component of texture
      vect3 specular;         // vect3 specular component of texture
      float shine;            // float shininess
      float shineStrength;    // float
      float transparency;     // float alpha component of texture
      bool  isTiled;          // 1 if it is. We determine this by saying 
                              //  if(uTile == vTile == 1.0) then isTiled = 0 
                              //  else isTiled = 1
      bool  isDecal;          // bool 1 if it is a decal (then use GL_DECAL)
    };

  protected: //public: // I don't know why this used to be public... (July 20, 2000)
    ifstream *aseStream;
    char buffer[CONV_BUFFER_LEN];

  public:
    CTexture();
    ~CTexture();
    void init();              // CTexture() calls this; essentially a constructor-in-a-function
    bool initialized;         // 0 = not initialized, 1 = initialized

    void add();               // creates a new tex, fills it out, adds to a linked list
    void unload();            // unloads all textures from memory
  
    void setFileStream(ifstream *stream)      // sets this class' file stream to point
    {                                         //  to the same place as the "master" stream
      aseStream = stream;
    }

  protected:
    tTexture *texHead;
    int numTextures;

    void readTex       (tTexture  *newTex);
    void linkTex       (tTexture  *newTex);   // add texture to texture linked-list
    void unlinkTex     (tTexture **currTex);
    void getTexFilename(tTexture  *currTex);  // extracts texture filename from a path in the ase file
    bool validateTexFilename(char *texFilename); // returns 1 if file exists
    void useMaterialProperties(tTexture *currTex); // does glMaterialfv() if !isDecal

  public:
    CTexture::tTexture *find(unsigned int texId); // returns a (CTexture::tTexture *) to the specified texture
    GLuint loadTexWithId(unsigned int texId); // returns gl tex "name" if successful, else returns 0
    GLuint loadTex(tTexture *currTex);        // loads a tex with info from a given tTexture struct
    void   loadAll();                         // loads all the textures
};

//---------------------------------------------------------------------------
class CModel
{
  protected:
    struct tModelInst
    {
      tModelInst *prev;       // creates a linked list of instances
      tModelInst *next;

      char *modelInstName;    // the 3dStudio *NODE_NAME name for the model inst

      bool    isTextured;     // is it textured? then get the texId
      unsigned int texId;     // the tex id extracted from the ASE file 
                              //  we use this to access CTexture::tex[id]... for all the tex info (utile, vtile, etc)
      GLuint  glTexId;        // the OpenGL texture "name" (id)
  
      vect3   worldpos;       // position in the world

      bool    isBillboard;    //think about how to implement this
      vect3   wireframecolor;             
      
      vect3   row0;           // Rotation Transform info
      vect3   row1;           // not used right now, but should be applied
      vect3   row2;           // using the first inst of a model
      vect3   row3;
    };

    //-----------------------------------------------
    struct tModelData
    {
      // we should really add isTextured to this instead of the inst
      // because the model will either be textured or not for all the instances,
      // depending on what the first instance is

      tModelData *prev;       // a linked-list of models
      tModelData *next;
      tModelInst *instHead;   // a linked-list of instances that refer to this model
      
      int numVerts;
      int numFaces;
      int numTverts;
      int numTfaces;
      int numTextures;
      bool hasNormals;
      bool isTextured;        // is it textured? All instances either are or
                              // aren't. It just depends on the model inst.
                              // By default tModelData.isTextured is 0 (not textured)
                              // (tModelData.isTextured = currInst.isTextured)

      char *modelName;        // the 3dStudio *NODE_NAME name for the base model
  
      float boundingX,        // bounding box, not used right now
            boundingY,        // these represent the max(x,y,z) of
            boundingZ;        // the bb. The other 8 coords are derived
      float boundingSphere;   // from this (note: model centered at (0,0,0))
  
      tVert3    *vertex;
      tVert2    *texCoord;
      tTriangle *face;

      CTexture::tTexture *tex;// pointer to a texture struct, which has already been loaded in CTexture

      GLfloat   *vertList;    // vertex array list for drawing using OpenGL
      GLfloat   *texCoordList;  
      GLfloat   *normalList;
      GLuint    *faceIndex;
      GLuint    numVertListVerts; // verts aren't shared when converted to being displayed with OpenGL
    };

    struct tNode              // bintree of model names to decide if we need to make instances
    {
      tNode *left;            // navigates the bin tree
      tNode *right;

      char  *modelName;       // This particular node's name
      tModelData *pMD;        // Points to the model who's name is attached to this node
    };

  protected: //public: // I don't know why this used to be public... (July 20, 2000)
    ifstream *aseStream;
    char buffer[CONV_BUFFER_LEN];

  public:
    CModel();
    void init(); // CModel() calls this; essentially a constructor-in-a-function
    ~CModel();
    bool initialized;         // 0 = not initialized, 1 = initialized

    void add(char *modelName);// interface for adding models (and instances)
    void unload();            // unloads all the models from memory

  protected:
    tNode *bintreeRoot;       // The root of the bin tree that has all the model names in it
  public: // yeah, I had to change this to make things work. Diesel will be better
    tModelData *modelHead;    // The linked-list of models (and insts inside)
  protected:
    int numModels;            // How many models are loaded? Doesn't include instances. 
    int numInsts;             // How many instances, in total, of all the models (not per model)

    void  searchTree      (tNode  *root, tNode **target, char *modelName);
    CModel::tNode *insertTree(tNode **root, char *modelName); // returns a pointer to the newly inserted modelName
    void  releaseBintree  (tNode **root);

    void  addInst         (tModelData  *currModel, char *modelInstName);
      void  initInst      (tModelInst  *currInst,  char *modelInstName);
      void  getInstData   (tModelInst  *currInst);
    void  releaseInst     (tModelInst  *currInst);
    void  linkInst        (tModelInst  *currInst,  tModelData *currModel);
    void  unlinkInst      (tModelInst **currInst,  tModelData *currModel);

    void  initModel       (tModelData  *newModel,  char *modelNamePrefix); // initializes the model struct;
    void  getModelData    (tModelData  *currModel); // get the verts, texcoords, etc
      bool  getVerts      (tModelData  *currModel); // returns 0 if not enough mem could be allocated
      bool  getFaces      (tModelData  *currModel); // returns 0 if not enough mem could be allocated
      bool  getTexVerts   (tModelData  *currModel);
      bool  getTexFaces   (tModelData  *currModel);
      void  getFaceNormals(tModelData  *currModel);
      void  translateModel(tModelData  *currModel); // centers model at (0,0,0)
    void  releaseModel    (tModelData  *currModel);
    void  linkModel       (tModelData  *currModel); // this links the new_model to the model linked list
    void  unlinkModel     (tModelData **currModel);

// New May 22, 2000
  protected:
    void initOpenGLData(tModelData *currModel);
    void genVertArray  (tModelData *currModel); // creates vertex array for drawing
      void genVerts    (tModelData *currModel); //  the model using OpenGL
      void genNormals  (tModelData *currModel);
      void genTexCoords(tModelData *currModel); // this will have to search through the insts until it finds a textured inst and then use that data
      void genFaces    (tModelData *currModel);
    virtual void draw( tModelData *currModel, tModelInst *currInst); // draws in OpenGL using vertex arrays; include opengl stuff elsewhere

  public:
    void genVertArrayAll();                     // Generates vert arrays for all of the models
    void drawAll();                             // draws all the models!
    
    // debugging routine:
    void writeVertArrayToFile(char *outFile, tModelData *currModel);  // outputs all the verts, texcoords, normals to a text file in a 
                                                                      // format similar to ase2lev
// End new

  private:
    int id;                   // this is the "global-to-the-class" id. it is incremented
                              // every time a new model is added. The inc'd id is then
                              // assigned to the model.id
                              // I don't think this is being used right now...

  public:
    CTexture *texData;                      // we need access to the texture list, which is stored in CTexture
    void setTexPointer(CTexture *texClass)  // this is necessary for correctly calculating the texture coordinates, 
    {                                       // such as when the textures are offset or tiled.
      texData = texClass;
    }

    void releaseTexPointer()
    {
      texData = NULL;
    }

    void setFileStream(ifstream *stream)
    {
      aseStream = stream;
    }

  // new May 25, 2000
  public:
    void loadAllTextures(); // loads all the textures for each inst of each model and assigns OpenGL tex names (ids)
  // end new May 25, 2000

};

//---------------------------------------------------------------------------

#endif

//-- E O F ------------------------------------------------------------------
