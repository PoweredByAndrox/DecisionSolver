/*
	COPYRIGHT © 2018 Ringo Hoffmann (zekro Development)
	READ BEFORE USING: https://zekro.de/policy
*/

#pragma once
#include "..\pch.h"
#include <mutex>

/**
 *  Create asynchronous timers which execute specified
 *  functions in set time interval.
 *
 *  @param func		Function which sould be executed
 *  @param interval	Interval of time in which function will be executed
 *					(in milliseconds)
 */
class Timer
{
public:
	Timer(std::function<void(void)> func) { m_func = func; }
	Timer():
		m_elapsedTicks(0),
		m_totalTicks(0),
		m_leftOverTicks(0),
		m_frameCount(0),
		m_framesPerSecond(0),
		m_framesThisSecond(0),
		m_qpcSecondCounter(0),
		m_isFixedTimeStep(false),
		m_targetElapsedTicks(TicksPerSecond / 60)
	{
		QueryPerformanceFrequency(&m_qpcFrequency);
		QueryPerformanceCounter(&m_qpcLastTime);

		// Initialize max delta to 1/10 of a second.
		m_qpcMaxDelta = m_qpcFrequency.QuadPart / 1;
	}
	~Timer() { stop(); }

	/*
	*  Stopping the timer and destroys the thread.
	*/
	void stop()
	{
		m_running = false;
		m_thread.~thread();
	}

	/*
	 *  Check if timer is running.
	 *
	 *  @returns boolean is running
	 */
	bool isRunning() { return m_running; }


	/*
	*  Set the method of the timer after
	*  initializing the timer instance.
	*
	*  @returns boolean is running
	*  @return  Timer reference of this
	*/
	Timer *setFunc(std::function<void(void)> func);

	// Get elapsed time since the previous Update call.
	UINT64 GetElapsedTicks() const { return m_elapsedTicks; }
	float GetElapsedSeconds() const { return TicksToSeconds(m_elapsedTicks); }

	// Get total time since the start of the program.
	UINT64 GetTotalTicks() const { return m_totalTicks; }
	float GetTotalSeconds() const { return TicksToSeconds(m_totalTicks); }

	// Get total number of updates since start of the program.
	UINT32 GetFrameCount() const { return m_frameCount; }

	// Get the current framerate.
	UINT32 GetFramesPerSecond() const { return m_framesPerSecond; }

	// Set whether to use fixed or variable timestep mode.
	void SetFixedTimeStep(bool isFixedTimestep) { m_isFixedTimeStep = isFixedTimestep; }

	bool GetIsFixedTimeStep() { return m_isFixedTimeStep; }

	// Set how often to call Update when in fixed timestep mode.
	void SetTargetElapsedTicks(UINT64 targetElapsed);
	void SetTargetElapsedSeconds(float targetElapsed);

	// Integer format represents time using 10,000,000 ticks per second.
	static const UINT64 TicksPerSecond = 10000000;

	static float TicksToSeconds(UINT64 ticks) { return static_cast<float>(ticks) / TicksPerSecond; }
	static UINT64 SecondsToTicks(float seconds) { return static_cast<UINT64>(seconds * TicksPerSecond); }

	// After an intentional timing discontinuity (for instance a blocking IO operation)
	// call this to avoid having the fixed timestep logic attempt a set of catch-up 
	// Update calls.

	void ResetElapsedTime();

	// Update timer state, calling the specified Update function the appropriate number of times.
	void Tick(std::function<void(void)> update);

	void BeginTime() { begin = chrono::high_resolution_clock::now(); }

	void EndTime() { end = chrono::high_resolution_clock::now(); }
	chrono::duration<float> GetResultTime() { return end - begin; }
private:
	// Source timing data uses QPC units.
	LARGE_INTEGER m_qpcFrequency;
	LARGE_INTEGER m_qpcLastTime;
	UINT64 m_qpcMaxDelta;

	// Derived timing data uses a canonical tick format.
	UINT64 m_elapsedTicks;
	UINT64 m_totalTicks;
	UINT64 m_leftOverTicks;

	// Members for tracking the framerate.
	UINT32 m_frameCount;
	UINT32 m_framesPerSecond;
	UINT32 m_framesThisSecond;
	UINT64 m_qpcSecondCounter;

	// Function to be executed fater interval
	std::function<void(void)> m_func;

	// Thread timer is running into
	std::thread m_thread;
	// Status if timer is running
	bool m_running = false;

	// Members for configuring fixed timestep mode.
	bool m_isFixedTimeStep, SkipDial = false;
	UINT64 m_targetElapsedTicks;

	chrono::time_point<chrono::steady_clock> begin, end;
	condition_variable wait;
};