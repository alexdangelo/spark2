//---------------------------------------------------------------------------
// CShip.h
// Created: April 11, 2000 - Alex D'Angelo
// Last modified Dec 14, 2000 - Alex D'Angelo
// Notes: *A ship, including the properties and the model
//---------------------------------------------------------------------------

#ifndef _CSHIP_H_
#define _CSHIP_H_

//-- I N C L U D E ----------------------------------------------------------
#include <math.h>
#include "Global.h"
#include "ConvCommon.h"
#include "CScene.h"   // We need the ship to have access to the world

#include "lin3tri3.h" // the line-tri intersection header

//---------------------------------------------------------------------------
// The ship class.
//
// Since this class has it's own unique list of models, we can have a ship
// that acts as one model but really consists of a bunch of smaller models.
// This is useful for making the ship explode into a bunch of pieces (instead
// of *or* in addition to generating random chunks). This way we can swap in
// parts, such as guns, tail fins, thrusters, etc and the model maker doesn't
// have to keep making and merging a static mesh each time. 
//
// To do: come up with a list of keywords for game data for the ship
//        some example words: mass, topspeed, accel, material
//
// Note: All the loading and conversion functions are in CShipLoad.cpp
//
//---------------------------------------------------------------------------

class CShip
{
  //--------------------
  // Model loading code
  //--------------------
  public:
    ifstream aseStream;
    char buffer[CONV_BUFFER_LEN];

    CShip();
    ~CShip();
    void init();
    bool initialized;               // 0 = not initialized, 1 = initialized

  public:
    bool openFile(char *aseFilename);
    void closeFile();

    bool load   (char *aseFilename);
    bool convert(char *aseFilename);// we're converting this file into a ship
    void unload ();                 // unloads all the models, textures, game data

  private:
    char *findString(char *string); // finds a case-insensitive string in a file stream
                                                                          
  //--------------------
  // Game data
  //--------------------

  public:
    CModel   model;                 // Ship model
    CTexture texture;               // Textures for the ship model // we need to work it in better

  private:
    CScene *scene;                  // a link to the outside "world". We need to have access to this somehow
    CTrack *trackPtr;               // a cleaner way to access it than scene.track

  public:
    bool attachToScene(CScene *existingScene); // gives the ship access to the world and track
    bool connectToTrack(CTrack *currTrack);    // just a nicer interface for accessing the track
    void detachFromScene() { scene = NULL; trackPtr = NULL; }

  public:// these should be private // game vars, ie vel, shieldStrength, ...
    float da;                       // amount to inc acc by when up is pressed
    float acc;                      // current scalar acceleration
    float minAcc;                   // absolute minimum acceleration threshold before setting acc to 0
    float maxAcc;                   // maximum absolute acceleration 

    float vel;                      // current scalar velocity (ok, so it's really speed if you want to be technical)
    float minVel;                   // absolute minimum velocity threshold before setting acc to 0
    float maxVel;                   // maximum absolute velocity

    float friction;                 // in units/sec. vel += (dv = (acc * dt)) - (friction * dt)
    
    float heading;                  // current direction ship is facing and moving (in degrees)
    float dDeg;                     // amount we can turn by ( in deg/sec )
    float roll;                     // how much the ship leans on turning, in degrees
    float minRoll;                  // minimum absolute degree threshold before setting roll to 0

    vect3 pos;
    vect3 oldpos;                   // used with vect3 pos to create a line for cd with gates, powerups, etc
    float dt;                       // in seconds
    int shieldStrength;             // percentage of shield strength (0 - 100)

  public:
    void draw();
    void reset();
    void update( float dTime );     // in seconds
    void setPos( vect3 newpos ) { pos[0] = newpos[0]; pos[1] = newpos[1]; pos[2] = newpos[2]; }
      
    void turnLeft();
    void turnRight();
    void accelerate();
    void decelerate();
    void updatePhysics();
    
    void drawWireBox(float halfx, float halfy, float halfz);
    void drawAxis();

//------------------------------
// Collision Detection
//------------------------------
  private:
    int closestNodeIdxA;            // these are the two nodes the ship is between
    int closestNodeIdxB;
    vect3 closestSplinePos;         // the spot between nodes A&B that is closest too and a line drawn through
                                    // this point and the ship would be perpendicular to the spline segment
    void checkForCollision();       // not even used or implemented! just a placeholder for now
  
  public:
    void clampPosToTrack();
    void getClosestNodesAndDrawThem(); // calc based on ship position and stores in vars closest{NodeIdx,SplinePos}
    void ChkVectorCurrPosMinusLastPosIntersectedATri(); // ANOTHER COL DET METHOD 
    void JustDrawAllTheTrackTriangles(); // DEBUGGING TO SEE IF WE'RE READING ALL THE TRIS CORRECTLY

//------------------------------
// Checkpoints (CShipLogic.cpp)
//------------------------------
  private:
    int   finishlineId;
    int   currLapNumber;
    int   nextGateToVisit;
    float feelerRadius; // this is the ship's "feeler" for detecting gates. it's a line
                        //  along the z-axis and is the *half* length when detecting gates,
                        //  but the radius of a sphere where detecting powerups
                        // this value may vary from ship to ship, depending on the dimensions of the ship

    void updateGateState(); // called in this->update()

// powerups
  private:
    void checkForPowerups();  // called in this->update(); checks for collisions with powerups

  public:
    bool isPlayer; // set to true so we know to just load the sound for this ship
//------------------------------
//  Sound
//------------------------------
    FSOUND_SAMPLE *sndEngine;
    int channel;

    void loadSounds(); // yes there should be a global sound manager (ie CAudio), but tough. This will work until the next rewrite (Spark3)
};

//---------------------------------------------------------------------------

#endif // _CSHIP_H_

//-- E O F ------------------------------------------------------------------

