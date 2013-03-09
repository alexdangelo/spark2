///////////////////////////////////////////////////////////////////////////////
// CNetwork.cpp
// Created: June 12, 2000 - 5:30pm - Ryan McGrail
///////////////////////////////////////////////////////////////////////////////

#include "CNetwork.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//
//  CNetwork::CNetwork
// 
//  CNetwork::CNetwork()
//
//  Description
//    CNetwork constructor
//
//  Parameters
//    None.
//
//  Return Values
//    None.
//
//-----------------------------------------------------------------------------

CNetwork::CNetwork()
{
  Init();
}

//-----------------------------------------------------------------------------
//
//  CNetwork::~CNetwork
// 
//  CNetwork::~CNetwork()
//
//  Description
//    CNetwork destructor
//
//  Parameters
//    None.
//
//  Return Values
//    None.
//
//-----------------------------------------------------------------------------

CNetwork::~CNetwork()
{
  Shutdown();

//  SAFE_DELARY( serverName );
//  SAFE_DELARY( sessionName );
}

//-----------------------------------------------------------------------------
//
//  CNetwork::Reset
//  
//  CNetwork::Reset()
//
//  Description
//    Resets CNetwork, can be used to try to recover from a failure elsewhere
//    in CNetwork.
//
//  Parameters
//    None.
//
//  Return Values
//    None.
//  
//-----------------------------------------------------------------------------

//
// ! WARNING !
// This function has a HIGH probability of crashing, or causing a MAJOR
// memory leak; be sure to check it.
// 

void CNetwork::Reset()
{
  CNetwork *pTempNet = new CNetwork();
  pTempNet = this;
  this->~CNetwork(); // <-- ok?
  memcpy( pTempNet, new CNetwork, sizeof( CNetwork ) );
  Init();
}

//-----------------------------------------------------------------------------
//
//  CNetwork::Init
//
//  CNetwork::Init()
//
//  Description
//    Sets class members to default values.
//
//  Parameters
//    None.
//
//  Return Values
//    true  - memory successfully allocated for class members.
//    false - out of memory.
//
//-----------------------------------------------------------------------------

bool CNetwork::Init()
{
/*
  m_IP = NULL;
  m_hWnd = NULL;
  m_socket = INVALID_SOCKET;

  m_state = NETSTATE_DISCONNECTED;
  m_serverPort = DEFAULT_SERVERPORT;
  m_wsVersion  = MAKEWORD( WINSOCK_VERSIONMAJOR, WINSOCK_VERSIONMINOR );

  if( (m_serverName  = new char[ strlen( DEFAULT_SERVERNAME  ) + 1 ]) == NULL ) return (false);
  if( (m_sessionName = new char[ strlen( DEFAULT_SESSIONNAME ) + 1 ]) == NULL ) return (false);
  strcpy( m_serverName,  DEFAULT_SERVERNAME  );
  strcpy( m_sessionName, DEFAULT_SESSIONNAME );
*/
  return (true);

}

//-----------------------------------------------------------------------------
//
//  CNetwork::Startup
//
//  CNetwork::Startup( int appMode, HWND wHandle )
//
//  Decription
//    Startup winsock, prepare it for generating sockets
//
//  Parameters
//    int appMode
//      indicates the mode the application is running in.
//        IS_SERVER
//        IS_CLIENT
//
//  Return Values
//    true  - Winsock startup successful
//    false - Winsock startup failed, use WSAGetLastError() for error #
//
//-----------------------------------------------------------------------------

bool CNetwork::Startup( int mode, HWND hWnd )
{
  /*
  long returned;
  m_hWnd = hWnd;
  m_mode = mode;
  m_serverPort = DEFAULT_SERVERPORT;
  m_socketFlags = FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE;

  if( m_mode == IS_SERVER ) 
    m_socketFlags |= (FD_ACCEPT | FD_OOB);

  // Winsock startup
  if( (returned = WSAStartup( m_wsVersion, &m_wsData )) > 0  )
  {
    switch (returned)
    {
      case WSAVERNOTSUPPORTED:
        error.msg( "WSOCKA ERROR: Requested: %d.%d, Found: %d.%d.", 
          WINSOCK_VERSIONMAJOR,
          WINSOCK_VERSIONMINOR, 
          LOBYTE( m_wsData.wVersion ),
          HIBYTE( m_wsData.wVersion ) );
        WSACleanup();
        break;

      default:
        error.msg( "WSOCKA ERROR: %d", WSAGetLastError() );
        break;
    }

    return (false);
  }

  if( gethostname( m_serverName, sizeof( m_serverName ) ) > 0 )
  {
    error.msg( "WSOCKA ERROR: %d", WSAGetLastError() );
    return (false);
  }
*/
  return (true);
}

//-----------------------------------------------------------------------------
//
//  CNetwork::Shutdown
//
//  CNetwork::Shutdown()
//
//  Description
//    Shutdown / Cleanup of connections & sockets
//
//  Parameters
//    None.
//
//  Return Values
//    None.
//
//-----------------------------------------------------------------------------

bool CNetwork::Shutdown()
{
  /*
  //Disconnect();
  WSACleanup();
  */
  return (true);
}

//-----------------------------------------------------------------------------
//
//  CNetwork::GetHost
//
//  CNetwork::GetHost( char *address, int port ) 
//
//  Description
//    Gets the handle to the current host
//
//  Parameters
//    char *address
//      
//
//  Return Values
//    None.
//
//-----------------------------------------------------------------------------

/*
HANDLE cComm::GetHost( char *address, int port ) 
{
  /*
  if( m_state != STATE_DISCONNECTED )
    return NULL;

  m_state = STATE_CONNECTING;

  if( port ) m_serverPort = port;
  if( address ) strcpy( m_serverName, address );

  m_address.sin_addr.s_addr = inet_addr( m_serverName );  // htonl could be used instead of inet_addr
  m_address.sin_port = htons( m_serverPort );
  m_address.sin_family = AF_INET;

  if( m_address.sin_addr.s_addr == INADDR_NONE )
    m_hHost = WSAAsyncGetHostByName( m_hWnd, SM_GETHOST, m_serverName, m_hostentbuf, MAXGETHOSTSTRUCT );
  else
    m_hHost = WSAAsyncGetHostByAddr( m_hWnd, SM_GETHOST, (const char *)&m_address.sin_addr.s_addr, sizeof( IN_ADDR ), AF_INET, m_hostentbuf, MAXGETHOSTSTRUCT );

  return NULL; //m_hHost;
}
*/

// E O F //////////////////////////////////////////////////////////////////////
/*



int cComm::OnGetHost( WPARAM wparam, LPARAM lparam ) {
  if( (HANDLE)wparam != m_hostHandle ) return 1;
  if( WSAGETASYNCERROR( lparam ) ) {  // upper 16 bits of (DWORD?)
    Disconnect();
    return 2;
  }

  if( CreateSocket() == INVALID_SOCKET ) {
    Disconnect();
    return 3;
  }

  if( ChangeWindows( m_hwnd ) == SOCKET_ERROR ) {
    Disconnect();
    return 4;
  }

  if( AddressHost() ) {
    Disconnect();
    return 5;
  }

  return 0;
}


SOCKET cComm::CreateSocket() {
  if( m_socket != INVALID_SOCKET ) return m_socket;  // already exists

#if defined( UDP_PROTOCOL )
  m_socket = socket( AF_INET, SOCK_DGRAM, 0 );
#else
  m_socket = socket( AF_INET, SOCK_STREAM, 0 );
#endif

  return m_socket;
}


int cComm::ChangeWindows( HWND hwnd ) {
  m_hwnd = hwnd;

  return WSAAsyncSelect( m_socket, m_hwnd, SM_ASYNC, m_socketFlags );
}


int cComm::AddressHost() {
  LPHOSTENT lphostent;

  lphostent = (LPHOSTENT)m_hostentbuf;
  // Bad C++ - check lphostent = NULL?
  strcpy( m_IP, inet_ntoa( *(LPIN_ADDR)lphostent->h_addr_list[ 0 ] ) );

  if( !m_runFlag ) {
    memcpy( &m_address.sin_addr, (LPIN_ADDR)lphostent->h_addr_list[ 0 ], sizeof( IN_ADDR ) );
  } else {  // server code
    m_address.sin_addr.s_addr = INADDR_ANY;
    if( bind( m_socket, (LPSOCKADDR)&m_address, sizeof( SOCKADDR ) ) == SOCKET_ERROR ) {
      closesocket( m_socket );
      return 1;
    }
#if !defined( UDP_PROTOCOL )
    if( listen( m_socket, SOMAXCONN ) == SOCKET_ERROR ) {
      closesocket( m_socket );
      return 2;
    }
#endif
  }
  return 0;
}


int cComm::AttemptConnect() {
  return connect( m_socket, (LPSOCKADDR)&m_address, sizeof( SOCKADDR_IN ) );
}


int cComm::Connected( int errmsg ) {
  // Chance to quit if something went wrong, print appropriate message here.
  m_state = STATE_CONNECTED;

  return 0;
}


int cComm::Disconnect() {
  switch( m_state ) {
    case STATE_CONNECTED:
      m_state = STATE_DISCONNECTING;
      return shutdown( m_socket, SD_SEND );
    case STATE_CONNECTING:
      // there is no pending data
      closesocket( m_socket );
      m_socket = INVALID_SOCKET;
      m_state = STATE_DISCONNECTED;
      break;
    case STATE_DISCONNECTING:
      // read in any pending data on the socket
      closesocket( m_socket );
      m_socket = INVALID_SOCKET;
      m_state = STATE_DISCONNECTED;
      break;
    default: break;
  }

  return 0;
}


int cComm::OnAsync( WPARAM wparam, LPARAM lparam ) {
  return 1;
}


int cComm::ReadIntoClientQueue( cClient *pClient ) {
  long                    error, lasterror, size;
  char                    *pBuffer;
  tcLinkedList<cMsgQueue> *pQueue;
  int                     type;
#if defined( UDP_PROTOCOL )
  SOCKADDR                from;
  int                     fromLen;
#endif

  pQueue = NULL;

  // Can only be receiving one data block at a time per client
  if( !pClient->GetReceiving() ) {
#if defined( UDP_PROTOCOL )
    error = recvfrom( pClient->GetSocket(), (char *)&type, sizeof( int ), &from, &fromLen );
#else
    error = recv( pClient->GetSocket(), (char *)&type, sizeof( int ), 0 );
#endif
    if( error == SOCKET_ERROR ) {
      if( WSAGetLastError() == WSAEWOULDBLOCK ) return 2;
      else return WSAGetLastError();
    }

    size = DetermineSize( type );

    pBuffer = new char[ size ];  // Allocate a buffer of needed size
    if( !pBuffer ) return 3;  // uh oh
    pQueue = new tcLinkedList<cMsgQueue>;
    if( !pQueue ) {
      delete pBuffer;
      return 4;
    }
    pQueue->SetBuffer( pBuffer, size, 1 );  // 1 - was allocated as an array
    ( (cMsg *)pBuffer )->m_type = type;

    pQueue->AddTransmitted( error );  // we've already read in pMsgId, so compensate
    pClient->AddInMsgQueue( pQueue );
    size -= error;
    if( !size ) return 0;

    pClient->SetReceiving( 1 );
    pBuffer += error;
  } else {
    tcLinkedListItterator<cMsgQueue> itter;

    for( itter = pClient->GetNextMsgIn(); itter(); ++itter ) {
      if( itter()->GetSize() != itter()->GetTransmitted() ) break;
    }
    if( !itter() ) return 5;  // big problem

    pQueue = itter();
    pBuffer = (char *)pQueue->GetBuffer() + pQueue->GetTransmitted();
    size = pQueue->GetSize() - pQueue->GetTransmitted();
  }

#if defined( UDP_PROTOCOL )
  error = recvfrom( pClient->GetSocket(), pBuffer, sizeof( int ), 0, &from, &fromLen );
#else
  error = recv( pClient->GetSocket(), pBuffer, size, 0 );
#endif
  if( error == SOCKET_ERROR ) {
    lasterror = WSAGetLastError();
    if( lasterror != WSAEWOULDBLOCK ) {
      return WSAGetLastError();
    }
    if( lasterror != WSAEMSGSIZE ) error = 0;
    // in the event that it IS WSAEpMsgSIZE we succesfully read in the data
    // so don't go apeshit.
  }

  if( !( size - error ) ) {
    pClient->SetReceiving( 0 );
    pQueue->AddTransmitted( error );
    return 0;
  }

  pQueue->AddTransmitted( error );
  return 1;
}


int cComm::WriteFromClientQueue( cClient *pClient ) {
  int                     error, length;
  tcLinkedList<cMsgQueue> *pMsg;
  char                    *ptr;
#if defined( UDP_PROTOCOL )
  int                     toLen;
  SOCKADDR                to;
#endif

  pMsg = pClient->GetNextMsgOut();
  if( !pMsg ) return 3;  // shouldn't happen...

  ptr = (char *)pMsg->GetBuffer() + pMsg->GetTransmitted();
  length = pMsg->GetSize() - pMsg->GetTransmitted();

#if defined( UDP_PROTOCOL )
  // A serious modification would have to be made to client for this
  // to work server-side and still work client-side (to and toLen
  // need to be initialized) and I'm not feeling up to it right now.
  error = sendto( pClient->GetSocket(), ptr, length, 0, pClient->GetAddress(), sizeof( SOCKADDR ) );
#else
  error = send( pClient->GetSocket(), ptr, length, 0 );
#endif
  if( error == SOCKET_ERROR ) {
    if( WSAGetLastError() == WSAEWOULDBLOCK ) error = 0;
    else return WSAGetLastError();
  }

  ptr += error;
  length -= error;
  pMsg->AddTransmitted( error );

  if( !length ) {
    delete pMsg;

    return 0;
  }

  return 0;
}


int cComm::Debug() {
  FILE *fDebug;
  fDebug = fopen( "debug.txt", "at" );
  fprintf( fDebug, "cComm --------------------\n" );
  fprintf( fDebug, "m_serverPort =%d\n", m_serverPort );
  fprintf( fDebug, "m_serverName =%s\n", m_serverName );
  fprintf( fDebug, "m_socketFlags=%d\n", m_socketFlags );
  fclose( fDebug );

  return 0;
}
*/
