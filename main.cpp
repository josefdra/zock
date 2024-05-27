#include <chrono>
#include <iostream>
#include <bitset>

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

    uint32_t exception_time = 5000;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    uint32_t m_time_limit;
};

int main()
{
    std::bitset<20000> set;
    bool asdfdsa = false;
    set.set(0);
    set.set(5000);
    set.set(10000);
    set.set(15000);
    set.set(19999);
    std::bitset<20000> set2;
    set2.set(1);
    set2.set(5010);
    set2.set(10100);
    set2.set(15100);
    set2.set(19199);
    Timer timer;
    set2 |= set;
    std::cout << timer.get_elapsed_time() << std::endl;
    return 0;
}