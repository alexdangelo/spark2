// CAi.h
// by Alex D'Angelo
// Created on July 19, 2000
// Last modified July 19, 2000
//
// Notes: The ai class must be attached to the scene first! If not, the
//        scene pointer will fail and everything will go ka-boom!
//
//---------------------------------------------------------------------------
#ifndef CAI_H
#define CAI_H

#include "CScene.h"
#include "CShip.h"

//---------------------------------------------------------------------------

class CAi
{
  private:
    int    numAiShips;
    CScene *scene;
    CTrack *trackPtr;
    bool   initialized;

  public:    
    CShip  *aiShip;
     

  public:
    CAi();
    ~CAi();

    void unload();
    bool attachToScene(CScene *existingScene);
    bool init(int numberAiShips);
    void update();
    void draw();
   
    void loadShips();
      bool loadShip          ( int shipId, char *aseFilename);
      void setShipPos        ( int shipId, vect3 *startPos  );
      void alignShipWithTrack( int shipId ); // *** stub ***

    void setShipsAtStartLine();
};

//---------------------------------------------------------------------------

#endif

//-- E O F ------------------------------------------------------------------