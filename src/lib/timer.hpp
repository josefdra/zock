#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <stdint.h>
#include <string>
#include <thread>
#include <iostream>

class TimeLimitExceededException : public std::exception
{
public:
    explicit TimeLimitExceededException(const std::string &message) : m_message(message) {}

    const char *what() const throw()
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};
class Timer
{
public:
    Timer(uint32_t time_limit)
    {
        m_start_time = std::chrono::high_resolution_clock::now();
        m_time_limit = time_limit;
    }

    Timer()
    {
        m_start_time = std::chrono::high_resolution_clock::now();
    }

    double return_rest_time() const
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time);
        return m_time_limit - duration.count();
    }

    double get_elapsed_time() const
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time);
        return duration.count();
    }

    uint32_t exception_time = 100;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    uint32_t m_time_limit;
};

#endif // TIMER_HPP