// CScene.h (Spark2)
// by Alex D'Angelo
// Created on May 24, 2000
// Last modified May 24, 2000
//
//---------------------------------------------------------------------------
#ifndef _CSCENE_H_
#define _CSCENE_H_

#include "ConvCommon.h"
#include "CTrack.h"

//---------------------------------------------------------------------------
// Class: CScene
// Desc:  An entire 3d scene, which includes the track, splines, models,
//        lights, textures, and game data.
//
// Note:  All the loading and conversion functions are in CSceneLoad.cpp
//---------------------------------------------------------------------------
class CScene
{
  public:
    bool loadedOk; // sets var inside CScene so I know it loaded ok when debugging

  public:
    ifstream aseStream;
    char buffer[CONV_BUFFER_LEN];

    CScene();
    ~CScene();
    void init();
    bool initialized;               // 0 = not initialized, 1 = initialized
    
    bool openFile(char *aseFilename);
    void closeFile();

    bool load   (char *aseFilename);
    bool convert(char *aseFilename);// we're obviously converting a level. was: (aseFn, char *convType);
    void unload ();                 // frees the entire scene from memory

    void draw();                    // draws everything

  public:
    char *findString(char *string); // finds a case-insensitive string in a text file stream

  protected:
    int  numModels;
    int  numSplines;
    int  numTextures;

  public:
    CLight   light;                 // Lights
    CTrack   track;                 // The track model and spline
    CModel   model;                 // Generic models and insts
    CSpline  spline;                // Generic splines
    CTexture texture;               // Textures for everything

    void update();
    
    // Music!
    FMUSIC_MODULE *mod;
};

//---------------------------------------------------------------------------

#endif

//-- E O F ------------------------------------------------------------------