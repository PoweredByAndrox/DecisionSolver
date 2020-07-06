#include "..\pch.h"
#include "Timer.h"

void Timer::Tick(function<void(void)> update)
{
	setFunc(update);

	m_running = true;
	m_thread = thread([&]()
	{
		while (m_running)
		{
			// Query the current time.
			LARGE_INTEGER currentTime;

			QueryPerformanceCounter(&currentTime);

			UINT64 timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

			m_qpcLastTime = currentTime;
			m_qpcSecondCounter += timeDelta;

			// Clamp excessively large time deltas (e.g. after paused in the debugger).
			if (timeDelta > m_qpcMaxDelta)
			{
				timeDelta = m_qpcMaxDelta;
			}

			// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
			timeDelta *= TicksPerSecond;
			timeDelta /= m_qpcFrequency.QuadPart;

			UINT32 lastFrameCount = m_frameCount;

			if (m_isFixedTimeStep)
			{
				// Fixed timestep update logic

				// If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
				// the clock to exactly match the target value. This prevents tiny and irrelevant errors
				// from accumulating over time. Without this clamping, a game that requested a 60 fps
				// fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
				// accumulate enough tiny errors that it would drop a frame. It is better to just round 
				// small deviations down to zero to leave things running smoothly.

				if (abs(static_cast<int>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
					timeDelta = m_targetElapsedTicks;

				m_leftOverTicks += timeDelta;

				while (m_leftOverTicks >= m_targetElapsedTicks)
				{
					m_elapsedTicks = m_targetElapsedTicks;
					m_totalTicks += m_targetElapsedTicks;
					m_leftOverTicks -= m_targetElapsedTicks;
					m_frameCount++;

					if (m_func)
						//		{
						m_func();
					//this_thread::sleep_until(timeDelta);
		//		}
				}

				//if (!SkipDial)
				//	if (m_targetElapsedTicks == 0 ||
				//		m_targetElapsedTicks >= m_qpcMaxDelta ||
				//		m_leftOverTicks >= m_qpcMaxDelta)
				//	{
				//		OutputDebugStringA("Maybe Endless Loop?\n");
				//		int Btn = MessageBoxA(0, "Maybe Endless Loop?\nTry to skip it?", "Warning",
				//			MB_CANCELTRYCONTINUE | MB_ICONWARNING | MB_DEFBUTTON2);
				//		switch (Btn)
				//		{
				//		case IDCONTINUE:
				//			return;
				//		case IDTRYAGAIN:
				//			// Check For Stuck In SetFixedTimer
				//			SetFixedTimeStep(false);
				//			SetTargetElapsedSeconds(0.f);
				//			m_targetElapsedTicks = 0;
				//			ResetElapsedTime();
				//			break;
				//		case IDCANCEL:
				//			SkipDial = true;
				//			break;
				//		}
				//	}
			}
			else
			{
				// Variable timestep update logic.
				m_elapsedTicks = timeDelta;
				m_totalTicks += timeDelta;
				m_leftOverTicks = 0;
				m_frameCount++;

				if (m_func)
					//	{
					m_func();
				//		this_thread::sleep_until(timeDelta);
				//	}
			}

			// Track the current framerate.
			if (m_frameCount != lastFrameCount)
			{
				m_framesThisSecond++;
			}

			if (m_qpcSecondCounter >= static_cast<UINT64>(m_qpcFrequency.QuadPart))
			{
				m_framesPerSecond = m_framesThisSecond;
				m_framesThisSecond = 0;
				m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
			}
		}
	});
	m_thread.detach();
}

Timer *Timer::setFunc(function<void(void)> func)
{
	if (!m_func.operator bool())
		m_func = func;
	return this;
}

void Timer::SetTargetElapsedTicks(UINT64 targetElapsed)
{
	if (targetElapsed == 0 || targetElapsed <= 0)
	{
		SkipDial = false;
		return;
	}
	else
		m_targetElapsedTicks = targetElapsed;
}

void Timer::SetTargetElapsedSeconds(float targetElapsed)
{
	if (targetElapsed == 0 || targetElapsed <= 0)
	{
		SkipDial = false;
		return;
	}
	else
		m_targetElapsedTicks = SecondsToTicks(targetElapsed);
}

void Timer::ResetElapsedTime()
{
	QueryPerformanceCounter(&m_qpcLastTime);

	m_leftOverTicks = 0;
	m_framesPerSecond = 0;
	m_framesThisSecond = 0;
	m_qpcSecondCounter = 0;

	stop();
	Tick(m_func);
}
