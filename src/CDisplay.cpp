///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "CDisplay.h"

//-----------------------------------------------------------------------------
// Func: CDisplay() - Constructor
//-----------------------------------------------------------------------------

CDisplay::CDisplay()
{
  focus = false;
  params.fov = 90.0;
  params.aspect = 0;
  params.fullscreen = 0;
  params.xPos = params.yPos = 0;
  params.width = params.height = 0;
  params.hDC = 0;  params.hRC = 0;
  params.wndProc = 0; params.hWnd = 0;
}

//-----------------------------------------------------------------------------
// Func: ~CDisplay() - Destructor
//-----------------------------------------------------------------------------

CDisplay::~CDisplay()
{
  ReleaseDC( params.hWnd, params.hDC );
  DestroyWindow( params.hWnd );

  wglMakeCurrent( params.hDC, NULL );
  wglDeleteContext( params.hRC );

  if ( params.fullscreen )
  {
    ChangeDisplaySettings( NULL, 0 );
  }
}

//-----------------------------------------------------------------------------
// Func:  init() - Initialization
// Rtrn:  Success: 1, Failure: errCode
//-----------------------------------------------------------------------------

bool CDisplay::init( SDisplay &ds, long exWndParams /*= 0*/, u8 initGL /*= 1*/ )
{
  params.width = ds.width;
  params.height = ds.height;
  params.wndProc = ds.wndProc;
  params.fullscreen = ds.fullscreen;
  strcpy( params.wndTitle, ds.wndTitle );

  if ( ds.height == 0 ) ds.height = 1;
  params.aspect = ( float(ds.width) / float(ds.height) );

  if ( params.fullscreen )
  {
    params.xPos = params.yPos = 0;
  }
  else
  {
    params.xPos = ds.xPos;
    params.yPos = ds.yPos;
  }

  if ( !setupWindow( exWndParams ) )
  {
    error.msg( "SYSTEM ERROR: Window setup FAILED." );
    return (false);
  }

  if( initGL )
  {
    if( !setupOpenGL() )
    {
      error.msg( "OPENGL ERROR: OpenGL setup FAILED." );
      return (false);
  }}

  return (true);
}

//-----------------------------------------------------------------------------
// Func:  SetupWindow
// Rtrn:  Success: 1, Failure: errCode
// Desc:  Does the acctual window creation
//-----------------------------------------------------------------------------

bool CDisplay::setupWindow( long exWndParams /*= 0*/ )
{
  WNDCLASS wc;
  static HINSTANCE hInstance = GetModuleHandle( NULL );

  if ( hInstance != NULL )
  {
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = params.wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHELTER));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = params.wndTitle;

    if ( !RegisterClass(&wc) )
    {
       error.msg( "SYSTEM ERROR: Unable to register window class." );
       return (false);
    }
  }
  else
  {
    error.msg( "SYSTEM ERROR: Unable to create new instance handle." );
    return (false);
  }

  if ( params.fullscreen )
  {
    if ( !displayMode() )
    {
      error.msg( "SYSTEM ERROR: Unable to switch to fullscreen." );
      return (false);
    }

    params.hWnd = CreateWindowEx(exWndParams, params.wndTitle, params.wndTitle,
      WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE,
      params.xPos, params.yPos, params.width, params.height, NULL, NULL, hInstance, NULL);

  }
  else
  {
    params.hWnd = CreateWindowEx(exWndParams, params.wndTitle, params.wndTitle,
      WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, //WS_POPUP | WS_OVERLAPPEDWINDOW,
      params.xPos, params.yPos, params.width, params.height, NULL, NULL, hInstance, NULL);
  }

  if ( params.hWnd == NULL )
  {
    error.msg( "SYSTEM ERROR: Unable to create window" );
    return (false);
  }

  ShowWindow( params.hWnd, SW_SHOWNORMAL );
  UpdateWindow( params.hWnd );

  return (true);
}

//-----------------------------------------------------------------------------
// Func:  SetupOpenGL
// Rtrn:  Success: 1, Failure: error code
// Desc:  Setups everything up for opengl to be used
//-----------------------------------------------------------------------------

bool CDisplay::setupOpenGL()
{
  int pf;

  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
    1,                              // version number
    PFD_DRAW_TO_WINDOW |            // support window
    PFD_SUPPORT_OPENGL |            // support OpenGL
    PFD_DOUBLEBUFFER,               // double buffered
    PFD_TYPE_RGBA,                  // RGBA type
    24,                             // 24-bit color depth
    0, 0, 0, 0, 0, 0,               // color bits ignored
    0,                              // no alpha buffer
    0,                              // shift bit ignored
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // accum bits ignored
    32,                             // 32-bit z-buffer
    0,                              // no stencil buffer
    0,                              // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main layer
    0,                              // reserved
    0, 0, 0                         // layer masks ignored
  };

  if ( (params.hDC = GetDC( params.hWnd )) == NULL )
  {
    error.msg( "GFXSYS ERROR: Unable to get device context." );
    return (false);
  }

  if ( !(pf = ChoosePixelFormat( params.hDC, &pfd )) )
  {
    error.msg( "GFXSYS ERROR: Unable to match an appropriate pixel format." );
    return (false);
  }

  if ( SetPixelFormat( params.hDC, pf, &pfd ) == FALSE )
  {
    error.msg( "GFXSYS ERROR: Unable to set pixel format." );
    return (false);
  }

  if ( !DescribePixelFormat( params.hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd ) )
  {
    error.msg( "GFXSYS ERROR: Unable to obtain information about the pixel format." );
    return (false);
  }

//-----------------
// WGL
//-----------------

  if ( (params.hRC = wglCreateContext( params.hDC )) == NULL )
  {            
    error.msg( "OPENGL ERROR: Unable to create a new OpenGL rendering context." );
    return (false);
  }

  if ( wglMakeCurrent( params.hDC, params.hRC ) == FALSE )
  {
    error.msg( "OPENGL ERROR: Unable to make rendering context current." );
    return (false);
  }

//-----------------
// init vars
//-----------------
  
  glPointSize( 1.0f );
  glLineWidth( 1.0f );

  glClearDepth( 1.0f );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  glEnable( GL_BLEND );
  glEnable( GL_CULL_FACE );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_POLYGON_OFFSET_LINE );

  glDisable( GL_LIGHTING );
  
  glCullFace( GL_BACK );
  glFrontFace( GL_CCW );
  glDepthFunc( GL_LEQUAL );
  glShadeModel( GL_FLAT );
  glPolygonMode( GL_FRONT, GL_FILL );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glEnable(GL_TEXTURE_2D);

  return (true);
}

//-----------------------------------------------------------------------------
// Func:  SetDisplayMode
// Rtrn:  Success: 1, Failure: 0
// Desc:  Switches to full screen
//-----------------------------------------------------------------------------

bool CDisplay::displayMode()
{
  DEVMODE deviceMode;

  HWND hDesktop = GetDesktopWindow();
  HDC  hDCDesk  = GetDC( hDesktop );

  deviceMode.dmSize = sizeof(DEVMODE);
  deviceMode.dmBitsPerPel = GetDeviceCaps( hDCDesk, BITSPIXEL );

  ReleaseDC( hDesktop, hDCDesk );

  deviceMode.dmPelsWidth = params.width;
  deviceMode.dmPelsHeight = params.height;
  deviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

  if( ChangeDisplaySettings( &deviceMode, CDS_FULLSCREEN ) == DISP_CHANGE_SUCCESSFUL )
    return (true);

  return (false);
}

//-----------------------------------------------------------------------------
// Func:  SetOrtho
// Rtrn:  Success: 1, Failure: uh... none.
// Desc:  Cahnges to an orthographic display mode
//-----------------------------------------------------------------------------

bool CDisplay::setOrtho()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float w = float(params.width  >> 1);
  float h = float(params.height >> 1);

  glOrtho(-w, w, -h, h, 10.0f, 1000.0f);
  glTranslatef(0.0f, 0.0f, -100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return (true);
}

//-----------------------------------------------------------------------------
// Func:  SetProspective
// Rtrn:  Success: 1, Failure: uh... none.
// Desc:  Changes to a prospective display mode
//-----------------------------------------------------------------------------

bool CDisplay::setProspective()
{
  glViewport( 0, 0, params.width, params.height );

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective( params.fov, params.aspect, 1.0, 2001 );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return(true);
}

// E O F //////////////////////////////////////////////////////////////////////   