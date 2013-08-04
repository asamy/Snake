/*
 * Copyright (c) 2013 Ahmed Samy  <f.fallen45@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "timer.h"

Timer::Timer()
{
	m_thread = std::thread(std::bind(&Timer::timerThread, this));
}

Timer::~Timer()
{

}

void Timer::start(int msec)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	m_interval = msec;
}

void Timer::setTimerFunc(const TimerFunc& func)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	m_func = func;
}

void Timer::timerThread()
{
	while (true) {
		const auto& now = std::chrono::system_clock::now();
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_condition.wait_until(lock, now + std::chrono::milliseconds(m_interval)) == std::cv_status::timeout) {
			lock.unlock();
			m_func();
		}
	}
}

