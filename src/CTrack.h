// CTrack.h
// by Alex D'Angelo
// Created on Aug 08, 2000
// Last modified on Aug 30, 2000
//
//---------------------------------------------------------------------------

#ifndef CTRACK_H
#define CTRACK_H

#include "ConvCommon.h"
#include "lin3tri3.h" // line-triangle intersection

const int PU_UNKNOWN = 0; // powerup ids
const int PU_SPEED   = 1;
const int PU_TIME    = 2;

//---------------------------------------------------------------------------
// CTrack combines the models and splines required for a track into a 
//   single object.
//---------------------------------------------------------------------------
class CTrack
{ 
  protected:
    ifstream *aseStream;
    char buffer[CONV_BUFFER_LEN];

  public:
    void setFileStream(ifstream *stream)
    {
      aseStream = stream;
    }

  public:
    CTrack();
    ~CTrack();
    CModel model;
    CSpline spline;
    void init();
    void unload();
    bool initialized;               // 0 = not initialized, 1 = initialized

  protected:
    //--------------
    // Gates
    //--------------
    struct tGate
    {
      Triangle3 tri[2];             // Dave Eberly's triangle type used for
                                    // the line-tri intersection

      tVert3    vert[4];            // this makes a quad using two triangles
      tTriangle face[2];            // we might need to calc our own norm
      vect3     pos;                // where's it located in world coords
                                    
      bool isStartline;             // flags. 1 => it's true
      bool isFinishline;            //        0 => it's false
      bool isCheckpoint;            
      int  id;                      // id = 0 for the startline.
                                    // id = numCheckpoints+1 for the finishline
                                    // 0 < id < numCheckpoints+1 for checkpoints
    };

    int  numGates;
    int  numStartline;              // should always be 1 (for error checking)
    int  numFinishline;             // should always be 1 (for error checking)
    int  numCheckpoints;            // doesn't include start + finish gate
    int  finishlineId;              // the id so we don't have to always calc it
    tGate *gate;                    // an array of gates

    //--------------
    // Start Pos
    //--------------
    int  numStartPos;
    vect3 *shipStartPos;            // an array that has a start pos for each ship (ai or player)

    //--------------
    // Power-ups
    //--------------
    struct tPowerup
    {
      char szName[255];             // what type? "speed", "autopilot", "..."
      int typeId;                   //  I should probably enum them as well
      vect3 pos;
      int spawnDelay;               // the counter starts at a given time and counts down to 0
      int spawnReset;               // the given time for this powerup
      bool active;                  // when the powerup is !active. At count == 0, active = 1;
    };

    int  currPowerupId;             // used just for loading in powerups
    int  numPowerups;               // the number of powerups present in the level
    float powerupDetectionRad;      // the radius of the sphere used for detection against the ship
    tPowerup *powerup;              // an array of powerups
    
    //--------------
    // Misc
    //--------------
    int  numLaps;                   // the total number of laps needed to complete the race    
    int  numPlayersMax;

  public:
    vect3 playerStartPos;           // where the player starts out. This should be integreted into the general startPos[]

  public:  
    bool initGates     ();          // calls getNumGatesASE() and allocates mem
    int  getNumGatesASE();          // counts the number of checkpoints plus the start and finish line
    void getCheckpoint ();          // gets the id and calls getGate()
    void getStartline  ();          // gets the position and quad region of the startline zone
    void getFinishline ();          // gets the position and quad region of the finishline zone
    void getGate       (int id);    // common core for getting the checkpoint, startline and finish line
    void calcGateCollisionZones();  // puts triangle data into Triangle3 format for line-tri interesection
    void drawGates     ();          // draws the detection zones for checkpoints, start, and finish line

    bool initStartPos  ();          // calls getNumStartPos() and allocates mem
    void getStartPos   ();          // gets a startpos "pad" (xyz) for a ship
    int  getNumStartPosASE();       // in ASE file stream, gets the number of start pos ==> max num players on curr level
    vect3 *getStartPos ( int i );   // returns the i'th start position 
    void drawStartPos  ();          // places a cube at each start position

    bool initPowerups  ();          // calls getNumPowerups() and allocates mem
    int  getNumPowerupsASE();       // counts the number of powerups in an ASE file stream
    void getPowerup    ();          // gets a powerup type and pos from an ASE file stream
    void drawPowerups  ();          // places a cube at each powerup position

    void getLaps       ();          // gets total number of laps
    int  getNumStartPos    () { return numStartPos; }       // gets the number of start pos ==> max num players on curr level
    int  getMaxNumOfPlayers() { return numStartPos; }       // the number of start positions implies the max number of players for a given level
    int  getNumGates       () { return numGates; }
    int  getFinishlineId   () { return numCheckpoints + 1; }// returns the index number of the finish line in the gate array
    int  getNumPowerups    () { return numPowerups; }

    char *findString(char *string); // finds a case-insensitive string in a text file stream
    void drawCube(float size);

    int  getClosestNode( vect3 pos );  // this assumes that a valid trackspline exists
    int  getNextClosestNode( vect3 pos, int closestNodeIndex ); // pass the closest node index from getClosestNode and the next idx is returned
    void getPointOnSegment( tVert3 *a, tVert3 *b, tVert3 *c, tVert3 *nearest );
    float trackWidth;
    float getTrackWidth() { return trackWidth; }

    int  getGateCollisionId( Line3 &shipPathSegment );
    bool intersect( Line3 &shipPathLine, tGate *gate );
    int  getPowerupCollisionId( vect3 pos, float shipRadius );
    
    // called from CShip to get collision detection with the ship info
    int getLineIntersectTriangle( Line3 &lineThroughShipAndSpline ); // figure out what triangle is next to the ship. it doesn't necessarily
                                                                     // mean that we're colliding with it. CShip will determine that.
    Triangle3 getTriangle( int triId ); // returns a triangle with index triId
    void drawTriangle( Triangle3 currTri ); // just the glBegin() and end() for drawing a Triangle3
    void drawTriangleWire( Triangle3 currTri ); // draws a wireframe triangle
    void update(); // updates the powerups (countdown before respawn)
};

//---------------------------------------------------------------------------

#endif

//-- E O F ------------------------------------------------------------------
