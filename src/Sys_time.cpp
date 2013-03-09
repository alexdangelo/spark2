
// COTD Entry submitted by Gaz Iqbal [devvoid@home.com]

//#include "Sys_hdr.h"
#include "Sys_time.h"

/*
=====================================
Constructor/Destructor
=====================================
*/
CTimer::CTimer() : 
    m_pfnUpdate(0),
    m_fBaseTime(0.0f),
    m_fLastTime(0.0f),
    m_fSecsPerTick(0.0f),
    m_fCurrentTime(0.0f),
    m_fFrameTime(0.0f),
    m_dTimerStart(0),
    m_dCurTime(0)
{
}

CTimer::~CTimer()
{            
  m_pfnUpdate = 0;
}

/*
=====================================
Initialize the timer
=====================================
*/
void CTimer::init()
{
  _int64  dTicksPerSec =0;

  if (QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&dTicksPerSec)))
  { 
    // performance counter is available, use it instead of multimedia timer
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&m_dTimerStart));
    m_fSecsPerTick = (1.0f)/(static_cast<float>(dTicksPerSec));
    m_pfnUpdate = &CTimer::getPerformanceCounterTime;
  }
  else
  { 
    //Use MM timer if unable to use the High Frequency timer
    m_dTimerStart = timeGetTime();
    m_fSecsPerTick = 1.0f/1000.0f;
    m_pfnUpdate = &CTimer::getMMTime;
  }
}

/*
================================================
Update time
================================================
*/
void CTimer::update()
{
  //The ->* operator binds the function pointer to the object pointed to by
  //the right hand pointer. which is THIS below
  m_fCurrentTime =  (this->*m_pfnUpdate)() - m_fBaseTime;
  m_fFrameTime = m_fCurrentTime - m_fLastTime;
  m_fLastTime = m_fCurrentTime;
}

/*
=====================================
Reset the Timer
=====================================
*/
void CTimer::reset()
{
  m_fBaseTime =  (this->*m_pfnUpdate)();
  m_fLastTime = m_fCurrentTime = m_fFrameTime = 0.0;
}

/*
=====================================
Time update funcs
=====================================
*/
float CTimer::getPerformanceCounterTime()
{
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&m_dCurTime));
  return (m_fSecsPerTick * (m_dCurTime - m_dTimerStart));
}

float CTimer::getMMTime()
{ return (m_fSecsPerTick * (timeGetTime()- m_dTimerStart));
}