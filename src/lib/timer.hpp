#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <stdint.h>

class TimeLimitExceededException : public std::exception
{
    const char *what() const throw()
    {
        return "Time limit exceeded";
    }
};

class Timer
{
public:
    Timer(uint32_t time_limit)
    {
        m_start_time = std::chrono::high_resolution_clock::now();
        m_time_limit = time_limit * 1000;
    }

    Timer()
    {
        m_start_time = std::chrono::high_resolution_clock::now();
    }

    double return_rest_time() const
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - m_start_time);
        return m_time_limit - duration.count();
    }

    double get_elapsed_time() const
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - m_start_time);
        return duration.count();
    }

    uint32_t exception_time = 10000;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    uint32_t m_time_limit;
};

#endif // TIMER_HPP