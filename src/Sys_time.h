
// COTD Entry submitted by Gaz Iqbal [devvoid@home.com]

#ifndef VOID_SYS_TIMER
#define VOID_SYS_TIMER

#include <windows.h>
#include <mmsystem.h>

/*
====================================================
A Simple Timer class which tries to use the 
Win32 HighPerformance Timer. Will default to 
the Windows Multimedia timer if the High Performance
Timer is not found.

I call the GetCurrent/Frame time functions from a 
pair of globally accessible functions which I define 
in namespace System{} to hide the implementation of 
CTime from all the other game code.
====================================================
*/
/*
  I'm using this because it's clean and easy to use.

  Changed the case of the function names and
  moved the headers in the cpp file to this h file
  Also changed the class name and function names
  because they conflicted with existing ones. The 
  explaination by the author mentions using a 
  namespace, but since we're not using them (and I
  don't fully understand them anyway), I'll just
  change the names (unless everything works).
  -AD 2000-09-05
*/
class CTimer
{
public:

	CTimer();
	~CTimer();

	void init();
	void reset();
	
	//Call this onces per game frame
	void update();

	//Access funcs
	inline const float & getCurrentTime() const { return m_fCurrentTime; }
	inline const float & getFrameTime()   const { return m_fFrameTime;   } 

private:

	typedef float (CTimer::* TimeUpdateFunc) ();
	TimeUpdateFunc m_pfnUpdate;

	float	m_fBaseTime;
	float	m_fLastTime;
	float	m_fSecsPerTick;

	float	m_fCurrentTime;
	float	m_fFrameTime;

	//QueryPerformancesCounter related 64bit integers.
	//These are Microsoft specific, and will have to be changed for
	//different compilers.
	_int64	m_dTimerStart;
	_int64  m_dCurTime;

	//One of these gets bound to the TimeUpdateFunc pointer
	float	getPerformanceCounterTime();
	float	getMMTime();
};

#endif