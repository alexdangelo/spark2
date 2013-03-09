/////////////////////////////////////////////////////////////////////////////
// CNetwork.h
// Created: April 7, 2000 - 3:03am - Ryan McGrail
/////////////////////////////////////////////////////////////////////////////

#ifndef _CNETWORK_H
#define _CNETWORK_H

// D E F I N E //////////////////////////////////////////////////////////////

#define WINSOCK_VERSIONMAJOR    2
#define WINSOCK_VERSIONMINOR    0

// Default values
#define DEFAULT_SERVERIP        "127.0.0.1"
#define DEFAULT_SERVERNAME      "127.0.0.1" // TODO: replace with a name.
#define DEFAULT_SERVERPORT      7000 // TODO: get new port.
#define DEFAULT_SESSIONNAME     "default session" // TODO: rename to "GAMENAME"

#define IDLE_TIMEOUT            5000  // milliseconds

// Protocols - If UDP is not wanted then TCP will be used
//#define UDP_PROTOCOL

// Messages for asynchronous notification
#define SM_GETHOST	            ( WM_USER + 1 )
#define SM_ASYNC	              ( WM_USER + 2 )

// CNetwork mode, global
#define NETMODE_CLIENT 1
#define NETMODE_SERVER 2

// CNetwork mode, local aliases
#define IS_CLIENT   NETMODE_CLIENT
#define IS_SERVER   NETMODE_SERVER

// CNetwork state
#define NETSTATE_CONNECTED     0
#define NETSTATE_CONNECTING    1
#define NETSTATE_DISCONNECTED  2
#define NETSTATE_DISCONNECTING 3

// I N C L U D E ////////////////////////////////////////////////////////////

#include "Global.h" // <-- #include <winsock2.h>

// T Y P E D E F / S T R U C T / C L A S S //////////////////////////////////

struct SWinsockParams
{
  // TODO: figure out if this is neccessary.
};

class CNetwork
{
  public:
    CNetwork();
    virtual ~CNetwork();

    bool                  Init();
    bool                  Startup( int mode, HWND hwnd );
    bool                  Shutdown();
    void                  Reset();

/*
    HANDLE                GetHost( char *address = NULL, int port = 0 );
    SOCKET                CreateSocket();
    int                   ChangeWindows( HWND hwnd );
    int                   AddressHost();  // called after GetHost
    int                   AttemptConnect();
    int                   Disconnect();

    int                   Connected( int errmsg );

    int                   OnAsync( WPARAM wparam, LPARAM lparam );
    int                   OnGetHost( WPARAM wparam, LPARAM lparam );

    int                   ReadIntoClientQueue( cClient *pClient );
    int                   WriteFromClientQueue( cClient *pClient );

    int                   Debug();
*/
    // meant to be overridden
    virtual long          DetermineSize( int type ) { return 0; }

    // data access methods
    int     GetPort()             { return serverPort;  }
    int     GetState()            { return state;       }
    char   *GetIP()               { return IP;          }
    char   *GetServerName()       { return serverName;  }
    char   *GetSessionName()      { return sessionName; }
    SOCKET  GetSocket() const     { return socket;      }
    HANDLE  GetHostHandle() const { return hHost;       }
    
    void   SetPort( int port )          { serverPort = port;           }
    void   SetState( int netState )     { state      = netState;       }
    void   SetSocket( SOCKET socket )   { socket     = socket;         }
    void   SetSessionName( char *name ) { strcpy( sessionName, name ); }

  protected: // OLD VARIABLES
    HWND            hWnd;       // window we are sending messages to
    WSADATA         wsData;     // WAS: winsockData;
    WORD            wsVersion;  // WAS: winsockVersion;
    SOCKET          socket;     // differs from clients' sockets
    SOCKADDR_IN     address;
    HANDLE          hHost;      // WAS: hostHandle;
    char            hostentbuf[ MAXGETHOSTSTRUCT ];
    char            *sessionName;
    char            *serverName;
    char            *IP;
    int             state;
    int             socketFlags;
    int             serverPort;
    int             mode;       // WAS: runFlags;  // server or client communications?
    DWORD           version;    // hiword is curr version.  loword is lowest supported
  
  protected: // NEW VARIABLES
    HWND            m_hWnd;       // WAS: hwnd;
    HANDLE          m_hHost;      // WAS: hostHandle;
    WSADATA         m_winsockData;
    DWORD           m_winsockVersion;

    SOCKET          m_socket;     // differs from clients' sockets
    int             m_socketFlags;


    SOCKADDR_IN     m_address;

    
    char            m_hostentbuf[ MAXGETHOSTSTRUCT ];
    char           *m_sessionName;
    char           *m_serverName;
    char           *m_IP;
    int             m_state;

    int             m_serverPort;
    int             m_mode;       // WAS: runFlags;  // server or client communications?



  private:

};

/////////////////////////////////////////////////////////////////////////////

#endif // _CNETWORK_H

// E O F ////////////////////////////////////////////////////////////////////
/*

*/
