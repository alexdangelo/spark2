
// I N C L U D E //////////////////////////////////////////////////////////////

#include "Global.h"
#include "CConsole.h"

// F U N C T I O N S //////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Func: ConCmdSndInfo (console command: sndinfo)
// Desc: Print to console to current DS status
// More: PARAM: NONE
// Rtrn: Success = 1
//-----------------------------------------------------------------------------

bool CConsole::ConCmdSndInfo(char *szCmdParam)
{
/*
  if((g_DSResult = g_lpDS->GetCaps(&g_DSCaps)) != DS_OK)
  {
    switch(g_DSResult)
    {
    case DSERR_GENERIC:
      WriteToConsole("Unknown Error");
      break;

    case DSERR_INVALIDPARAM:
      WriteToConsole("Invalid Param");
      break;

    case DSERR_UNINITIALIZED:
      WriteToConsole("Uninitialized");
      break;

    default:
      WriteToConsole("Mystery Error");
    }

    return(0);
  }
  else
  {
    int tmp = 1;
    char msg[100] = {NULL};

    WriteToConsole("DSCaps Dump:");
    tmp = g_DSCaps.dwSize;                         sprintf(msg, "dwSize:                         %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFlags;                        sprintf(msg, "dwFlags:                        %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMinSecondarySampleRate;       sprintf(msg, "dwMinSecondarySampleRate:       %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxSecondarySampleRate;       sprintf(msg, "dwMaxSecondarySampleRate:       %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwPrimaryBuffers;               sprintf(msg, "dwPrimaryBuffers:               %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHwMixingAllBuffers;        sprintf(msg, "dwMaxHwMixingAllBuffers:        %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHwMixingStaticBuffers;     sprintf(msg, "dwMaxHwMixingStaticBuffers:     %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHwMixingStreamingBuffers;  sprintf(msg, "dwMaxHwMixingStreamingBuffers:  %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHwMixingAllBuffers;       sprintf(msg, "dwFreeHwMixingAllBuffers:       %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHwMixingStaticBuffers;    sprintf(msg, "dwFreeHwMixingStaticBuffers:    %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHwMixingStreamingBuffers; sprintf(msg, "dwFreeHwMixingStreamingBuffers: %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHw3DAllBuffers;            sprintf(msg, "dwMaxHw3DAllBuffers:            %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHw3DStaticBuffers;         sprintf(msg, "dwMaxHw3DStaticBuffers:         %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxHw3DStreamingBuffers;      sprintf(msg, "dwMaxHw3DStreamingBuffers:      %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHw3DAllBuffers;           sprintf(msg, "dwFreeHw3DAllBuffers:           %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHw3DStaticBuffers;        sprintf(msg, "dwFreeHw3DStaticBuffers:        %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHw3DStreamingBuffers;     sprintf(msg, "dwFreeHw3DStreamingBuffers:     %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwTotalHwMemBytes;              sprintf(msg, "dwTotalHwMemBytes:              %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwFreeHwMemBytes;               sprintf(msg, "dwFreeHwMemBytes:               %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwMaxContigFreeHwMemBytes;      sprintf(msg, "dwMaxContigFreeHwMemBytes:      %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwUnlockTransferRateHwBuffers;  sprintf(msg, "dwUnlockTransferRateHwBuffers:  %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwPlayCpuOverheadSwBuffers;     sprintf(msg, "dwPlayCpuOverheadSwBuffers:     %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwReserved1;                    sprintf(msg, "dwReserved1:                    %d", tmp); WriteToConsole(msg);
    tmp = g_DSCaps.dwReserved2;                    sprintf(msg, "dwReserved2:                    %d", tmp); WriteToConsole(msg);
  }

  */
  return(1);

}

// E O F //////////////////////////////////////////////////////////////////////