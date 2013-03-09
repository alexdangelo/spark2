///////////////////////////////////////////////////////////////////////////////
// CClient.cpp
// Created: April 11, 2000 - 9:46pm - Ryan McGrail
// Last modified: July 19, 2000
///////////////////////////////////////////////////////////////////////////////
#include "CClient.h"

bool paused = 1;

// C C L I E N T //////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Func: CClient() - Constructor
//-----------------------------------------------------------------------------
CClient::CClient( CServer *pServer )
{
  localServer = pServer;
}

//-----------------------------------------------------------------------------
// Func: OpenWindow()
//-----------------------------------------------------------------------------
bool CClient::OpenWindow( WNDPROC proc, char *title )
{
  SDisplay ds;

  if ( !loadConfigEx( ds, 0, NULL ) )
  {
    error.msg( "WARNING: Unable to load config, using defaults" );

    ds.xPos = 100;
    ds.yPos = 100;
    ds.width = 640;
    ds.height = 480;
    ds.fullscreen = 0;
  }

  ds.wndProc = proc;
  strcpy( ds.wndTitle, title );

  if( display.init( ds ) != 1 )
  {
    error.msg( "CLIENT ERROR: Unable to open Window" );
    return (false);
  }

  return (true);
}

//-----------------------------------------------------------------------------
// Func: loadConfig + Ex - Constructor
//-----------------------------------------------------------------------------

bool CClient::loadConfigEx( SDisplay &ds, bool applyChanges, char *chkPath )
{
  fstream file;
  char seper[] = " ,=;\"\'";
  char *token;
  char *line = new char[512];
  char *path = new char[512];

  if ( chkPath == NULL )
    strcpy ( path, "configs\\default.cfg" );
  else
    sprintf( path, "configs\\%s", chkPath );

  file.open( path, ios::in );
  if ( file.fail() )
  {
    error.msg( "CLIENT ERROR: Unable to open config file." );
    file.clear();
    file.close();
    return (false);
  }

  while( !file.eof() )
  {
    file.getline( line, 512, '\n' );

    if( file.fail() )
    {
      error.msg( "CLIENT ERROR: An unknown error has occurred." );
      file.clear();
      file.close();
      return ( false );
    }

    // BRUTE FORCE, wooohoooo!!! take that CS teachers!!! =Þ
    token = strtok( line, seper );
    if ( token != NULL )
    {
      if ( _stricmp( token, "colordepth" ) == 0 )
        ds.colorDepth = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "fov" ) == 0 )
        ds.fov = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "fullscreen" ) == 0 )
        ds.fullscreen = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "height" ) == 0 )
        ds.height = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "width" ) == 0 )
        ds.width = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "xpos" ) == 0 )
        ds.xPos = atol( strtok( NULL, seper ) );

      else if ( _stricmp( token, "ypos" ) == 0 )
        ds.yPos = atol( strtok( NULL, seper ) );

      else ; // do nothing.
    } // while( token )
  } // while( file )

  file.clear();
  file.close();

  if( applyChanges )
  {
    // display.changeSettings( ds );
  }

  SAFE_DELARY(line);
  SAFE_DELARY(path);
  return (true);
}

//-----------------------------------------------------------------------------
// Func: init() - initialize everything needed to start up the client
//-----------------------------------------------------------------------------
bool CClient::init()
{
  //console->splash(1);
  console->splash(0);

// TODO: Menu init.

//-----------------
// Scenery Init (this should actually be handled through the menus)
//-----------------
  
  // nulltrk.ase
  if( !scene.load("demo1.ase") )
  // if( !scene.load("checkpoints3.ase") )
  // if( !scene.load("nulltrk.ase") )
  {
    error.msg( "CLIENT: Unable to Define World Space" );
    return false;
  }

  console->print( 0, "CLIENT: World Space Defined" );

//-----------------
// Player + Ship Init (this should actually be handled through the menus)
//-----------------
  player.ship.isPlayer = true;
  player.ship.attachToScene( &scene ); // must be done first!!!!
  
  if( !player.load("speeder01.ase") )
  //if( !player.load("nullshp.ase") )
  {
    error.msg( "CLIENT: Unable to Define Player Space" );
    return false;
  }
  player.ship.pos[1] = 16.0f; // temp placement
  player.ship.reset();
  console->print( 0, "CLIENT: Player Space Defined" );

//-----------------
// AI Init (this should actually be handled through the menus)
//-----------------

  ai.attachToScene(&scene);
  ai.loadShips();

/*
  ai.init(2); // lets add some AI ships
  ai.loadShip(0, "ships/michael.ase");
  ai.loadShip(1, "ships/ship01tc.ase");
*/  
  //ai.setShipsAtStartLine(); // I don't think this works

//-----------------
// Misc
//-----------------
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // try GL_MODULATE

  worldcam.init(-100, 300, -100,
                   0,   0, -400,
                   0,   1,    0);
  worldcam.turnOn();
  worldcam.followShip(&player.ship);

//-----------------
  timer.init(); // from COTD at Flipcode
  timer.reset(); // this should also be called on loading a map and/or on the beginning of a race
//-----------------
  return (true);
}

//-----------------------------------------------------------------------------
// Func: CClient::update() -- the game loop
//-----------------------------------------------------------------------------

u16 CClient::update()
{
  timer.update();

  getInput(); // make this pause-able
  float dt = timer.getFrameTime();
  if(!paused)
  {
    //scene.tick(dt); // ~0.1s on my computer
    scene.update();
  }
  scene.track.update();     // make these pause-able
  player.ship.update( dt ); 
  

  //ai.update();
  // updateEvents();   // do explosions, etc
  // calculateRanks(); // for all the ships

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  
  glPushMatrix();
  {
    // worldcam.view(); // contains gluLookAt
    //
    // from spark v1
    // have the camera follow the ship from behind
    GLfloat dist = worldcam.distFromTarget;
    GLfloat angle = (player.ship.heading + worldcam.heading) * PIOVER180;

    worldcam.eyepos[0] = player.ship.pos[0] - dist * (GLfloat)sin(angle);
    worldcam.eyepos[2] = player.ship.pos[2] - dist * (GLfloat)cos(angle);

    gluLookAt(worldcam.eyepos[0], worldcam.eyepos[1], worldcam.eyepos[2],
              player.ship.pos[0], player.ship.pos[1], player.ship.pos[2],
              0,1,0);

    // end from spark v1    

  
    glPushMatrix();
    {
      scene.draw();

      player.ship.clampPosToTrack();
      player.ship.draw();
      
      //player.ship.getClosestNodesAndDrawThem();
      //player.ship.JustDrawAllTheTrackTriangles();
      //player.ship.ChkVectorCurrPosMinusLastPosIntersectedATri();
      
      scene.track.drawGates();     // just so we can see that it's working
      scene.track.drawStartPos();
      scene.track.drawPowerups();

      ai.draw();
    }
    glPopMatrix();

  }
  glPopMatrix();

  // Draw 2D, Order: Splash, Menu, Console
  if( console->active() || console->splash() /* || menu->active()*/ )
  {
    display.setOrtho();

    // HACKED SPLASH SCREEN, see above order.
    if( console->active() || console->splash() )
      console->draw();
/*
    if( menu->active() )
      menu->draw();
*/

    display.setProspective();
  }

  glFlush();

  return (1);
}

//-----------------------------------------------------------------------------
// Func: getInput()
//-----------------------------------------------------------------------------

u16 CClient::getInput( u8 vkey )
{
  if ( vkey )
  {
    switch( vkey )
    {
    case VK_TILDE:
      {
        if ( console->active() )
          console->deactivate();
        else
          console->activate();
      }
      break;
    default: {}
    }
  }
  else if( !console->active() && display.hasFocus() )
  {
    //-- switch gfx mode ------------------------

    if(KEYDOWN('1') && !console->active()) //if(KEYDOWN('1') && !tmpGCon->active())
    {
  	  glDisable(GL_TEXTURE_2D);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glDisable(GL_CULL_FACE);
    }

    if(KEYDOWN('2') && !console->active()) //if(KEYDOWN('2') && !tmpGCon->active())
    {
      glEnable(GL_TEXTURE_2D);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glEnable(GL_CULL_FACE);
    }

/*
    if(KEYDOWN('3'))
    {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
  
    if(KEYDOWN('4'))
    {
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }

    if(KEYDOWN('0'))
    {
      if ( splash.Active() )
      {
        splash.Deactivate();
      }
      else
      {
        splash.Activate();
      }

      Sleep(100);
    }
*/
    //----------------
    // Ship Controls
    //----------------

    if(KEYDOWN(VK_UP))
    {
      //player.ship.thrust(5.0f);
      player.ship.accelerate();
    }  

    if(KEYDOWN(VK_DOWN))
    {
      //player.ship.brake(5.0f);
      player.ship.decelerate();
    }

    if(KEYDOWN(VK_LEFT))
    {
      //player.ship.turn(5.0f);
      player.ship.turnLeft();
    }

    if(KEYDOWN(VK_RIGHT))
    {
      //player.ship.turn(-5.0f);
      player.ship.turnRight();
    }

    if(KEYDOWN('R'))
    {
      player.ship.reset();
    }

    // temp for michael
    if(KEYDOWN('T'))
    {
      player.ship.pos[1] += 5.0f;
    }

    if(KEYDOWN('G'))
    {
      player.ship.pos[1] -= 5.0f;
    }

    //----------------
    // Camera Controls
    //----------------
/*
    if(KEYDOWN('E'))
    {
      worldcam.move(5);
    }  
    if(KEYDOWN('D'))
    {
      worldcam.move(-5);
    }
*/
    if(KEYDOWN('S'))
    {
      worldcam.rotate(-5.0f);
    }

    if(KEYDOWN('F'))
    {
      worldcam.rotate(5.0f);
    }

    if(KEYDOWN('A'))
    {
      worldcam.moveIn();
    }

    if(KEYDOWN('Z'))
    {
      worldcam.moveOut();
    }

    if(KEYDOWN('E'))
    {
      worldcam.moveUp();
    }

    if(KEYDOWN('D'))
    {
      worldcam.moveDown();
    }

    if(KEYDOWN(VK_HOME))
    {
      //worldcam.orbitUp(5.0f);
    }

    if(KEYDOWN(VK_END))
    {
      //worldcam.orbitDown(5.0f);
    }

    if(KEYDOWN(VK_DELETE))
    {
      worldcam.orbitXZ(1.0f);
    }

    if(KEYDOWN(VK_NEXT))
    {
      worldcam.orbitXZ(-1.0f);
    }

    if(KEYDOWN(VK_SPACE))
    {
      // Sleep(150);
    }

    if(KEYDOWN('P'))
    {
      if( paused == 1 ) { paused = 0; }
      else              { paused = 1; }
      Sleep(150);
    }
  }

  return (1);
}

// E O F //////////////////////////////////////////////////////////////////////