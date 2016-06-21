// ---------------------------------------------------------------------------
//
// High Resolution Timer
//
// ---------------------------------------------------------------------------

#ifndef ESGI_TIMER
#define ESGI_TIMER

// --- Includes --------------------------------------------------------------

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN		1
#define WIN32_EXTRA_LEAN		1
#define VC_EXTRALEAN			1
#include <windows.h>

// --- Classes ---------------------------------------------------------------

class EsgiTimer
{
public:
	EsgiTimer() : m_StartTime(0), m_StopTime(0)
	{
        LARGE_INTEGER QueryFrequency;
		::QueryPerformanceFrequency(&QueryFrequency);
		m_Resolution = 1.0 / (double)QueryFrequency.LowPart;
	}
    
	~EsgiTimer()
	{
	}
    
	// ---
    
	void Begin()
	{
        LARGE_INTEGER QueryTime;
		::QueryPerformanceCounter(&QueryTime);
		m_StartTime = QueryTime.QuadPart;
		m_StopTime = m_StartTime;
	}
    
	void End()
	{
        LARGE_INTEGER QueryTime;
		::QueryPerformanceCounter(&QueryTime);
		m_StopTime = QueryTime.QuadPart;
	}
    
	// ---
    
    inline __int64 GetStartTime() const
	{
		return m_StartTime;
	}
    
    inline __int64 GetStopTime() const
	{
		return m_StopTime;
	}
    
	inline double GetElapsedTime()
	{
		return ((double)(m_StopTime - m_StartTime) * m_Resolution);
	}
    
	// ---
    
	static inline double GetTimerValue()
	{
		LARGE_INTEGER QueryTime;
		::QueryPerformanceCounter(&QueryTime);
		return (double)QueryTime.QuadPart;
	}
    
	static inline double GetElapsedTimeSince(double initial)
	{
		double current = GetTimerValue();
		current -= initial;
        
		LARGE_INTEGER QueryFrequency;
		::QueryPerformanceFrequency(&QueryFrequency);
		return (current / (double)QueryFrequency.LowPart);
	}
    
private:
	double m_Resolution;
    __int64 m_StartTime;
    __int64 m_StopTime;
};

#elif defined(__APPLE__)

// --- Includes --------------------------------------------------------------

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

// --- Classes ---------------------------------------------------------------

class EsgiTimer
{
public:
	EsgiTimer() : m_StartTime(0), m_StopTime(0)
	{
		mach_timebase_info_data_t TimebaseInfo;
        mach_timebase_info(&TimebaseInfo);
        m_Resolution = (double)TimebaseInfo.numer / ((double)(TimebaseInfo.denom)*NSEC_PER_SEC);
	}
    
	~EsgiTimer()
	{
	}
    
	// ---
    
	void Begin()
	{
		m_StartTime = mach_absolute_time();
		m_StopTime = m_StartTime;
	}
    
	void End()
	{
		m_StopTime = mach_absolute_time();
	}
    
	// ---
    
	inline uint64_t GetStartTime() const
	{
		return m_StartTime;
	}
    
	inline uint64_t GetStopTime() const
	{
		return m_StopTime;
	}
    
	inline double GetElapsedTime()
	{
		return ((double)(m_StopTime - m_StartTime) * m_Resolution);
	}
    
	// ---
    
	static inline double GetTimerValue()
	{
		return (double)mach_absolute_time();
	}
    
	static inline double GetElapsedTimeSince(double initial)
	{
		double current = GetTimerValue();
		double elapsed = current - initial;
        
		static double resolution;
        if (resolution == 0) {
            mach_timebase_info_data_t TimebaseInfo;
            mach_timebase_info(&TimebaseInfo);
            
            resolution = (double)TimebaseInfo.numer / ((double)(TimebaseInfo.denom)*NSEC_PER_SEC);
        }
        
        return elapsed * resolution;
	}
    
private:
	double m_Resolution;
	uint64_t m_StartTime;
	uint64_t m_StopTime;
};

#else

// --- Includes --------------------------------------------------------------

#include <sys/time.h>

// --- Classes ---------------------------------------------------------------

class EsgiTimer
{
public:
    EsgiTimer()
    {
        
    }
    
    ~EsgiTimer()
    {
    }
    
    // ---
    
    void Begin()
    {
        gettimeofday(&m_StartTime, NULL);
        stop = m_StartTime;
    }
    
    void End()
    {
        gettimeofday(&m_StopTime, NULL);
    }
    
    // ---
    
    inline timeval GetStartTime() const
    {
        return m_StartTime;
    }
    
    inline timeval GetStopTime() const
    {
        return m_StopTime;
    }
    
    inline double GetElapsedTime()
    {
        return ((double)(m_StartTime.tv_usec - m_StopTime.tv_usec))*0.0000001f;
    }
    
    static inline double GetTimerValue()
    {
        timeval tmp;
        gettimeofday(&tmp, NULL);
        return tmp.tv_usec*0.0000001f;
    }
    
    static inline double GetElapsedTimeSince(double initial)
    {
        double elapsed = GetTimerValue() - initial;
        if(elapsed > 0){
            return elapsed;
        }else{
            return 0;
        }
    }
    
private:
    timeval m_StartTime, m_StopTime;
    
};

#endif

#endif // ESGI_TIMER