#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <array>
#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include <chrono>
#include <memory>
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include <unordered_set>

#include "game.hpp"

class ThreadPool
{
public:
    ThreadPool(size_t);
    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();

private:
    // running threads
    std::vector<std::thread> workers;
    // task queue
    std::queue<std::function<void()>> tasks;

    // synchronisation
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

std::string root_directory = "/home/josefdra/ZOCK/g01";

std::array<std::tuple<std::string, uint8_t>, 20> maps{
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2012_Map_fuenf.map", 3),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2013_comp_1_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2013_comp_4_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2013_comp_5_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2013_comp_9_2.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2014_comp_1_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2014_comp_2_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2014_comp_2_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2014_comp_3_8p.map", 8),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2019_comp_03_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2019_comp_05_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2019_comp_07_4p_0.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/2019_comp_07_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2017_02_3p.map", 3),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2019_01_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_01_8p.map", 8),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_02_2p.map", 2),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_02_3p.map", 3),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_02_4p.map", 4),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_02_8p.map", 8)};

int total_games = 67 * 25 * 25 * 25;
int total_finished_games = 0;
uint8_t most_won_games = 0;
uint8_t best_mult1 = 1;
uint8_t best_mult2 = 1;
uint8_t best_mult3 = 1;
uint8_t mult1;
uint8_t mult2;
uint8_t mult3;
bool finished = false;
int threads = std::thread::hardware_concurrency();

inline ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                                             [this]
                                             { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
    }
}

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace([task]()
                      { (*task)(); });
    }
    condition.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
    {
        worker.join();
    }
}

void run_game(std::string &map_path, uint8_t mult1, uint8_t mult2, uint8_t mult3, uint8_t position, std::vector<bool> &won_games_in_match, std::vector<bool> &finished_games_in_match, uint8_t game_number)
{
    Game game(map_path, best_mult1, best_mult2, best_mult3, mult1, mult2, mult3, position);
    std::cout << "starting game " << (int)game_number << " with configuration: " << (int)mult1 << " " << (int)mult2 << " " << (int)mult3 << std::endl;
    bool result = game.run();
    if (result)
    {
        won_games_in_match[game_number] = true;
    }
    finished_games_in_match[game_number] = true;
}

void run_match(uint8_t mult1, uint8_t mult2, uint8_t mult3, ThreadPool &pool)
{
    bool match_finished = false;
    std::vector<bool> won_games_in_match(67, false);
    std::vector<bool> finished_games_in_match(67, false);
    uint8_t game_number = 0;
    for (auto &map : maps)
    {
        std::string map_path;
        uint8_t player_count;
        std::tie(map_path, player_count) = map;

        for (uint8_t p = 0; p < player_count; p++)
        {
            // pool.enqueue(run_game, map_path, mult1, mult2, mult3, p, won_games_in_match, finished_games_in_match, game_number);
            run_game(map_path, mult1, mult2, mult3, p, won_games_in_match, finished_games_in_match, game_number);
            game_number++;
        }
    }
    while (!match_finished)
    {
        usleep(5000);
        uint8_t counter = 0;
        for (bool f : finished_games_in_match)
        {
            if (f)
            {
                counter++;
            }
        }
        if (counter == 67)
        {
            match_finished = true;
        }
    }
    total_finished_games += 67;
    match_finished = false;
    uint8_t counter = 0;
    for (bool f : won_games_in_match)
    {
        if (f)
        {
            counter++;
        }
    }
    std::cout << "won " << (int)counter << "/67 games with configuration: " << (int)mult1 << " " << (int)mult2 << " " << (int)mult3 << std::endl;
    std::cout << "current best configuration: " << (int)best_mult1 << " " << (int)best_mult2 << " " << (int)best_mult3 << std::endl;
    std::cout << std::setprecision(5) << (float)total_finished_games / float(total_games) << "% done" << std::endl;
    if (counter > most_won_games)
    {
        most_won_games = counter;
        best_mult1 = mult1;
        best_mult2 = mult2;
        best_mult3 = mult3;
        mult1 = 1;
        mult2 = 1;
        mult3 = 1;
        total_finished_games = 0;
    }
    if (total_finished_games == total_games)
    {
        finished = true;
    }
}

int main()
{
    std::string filename = root_directory + "/ges_log.txt";
    // freopen(filename.c_str(), "w", stdout);
    auto start_time = std::chrono::steady_clock::now();
    ThreadPool pool(threads);
    uint8_t variations = 2;
    for (uint8_t mult1 = 1; mult1 < variations; mult1++)
    {
        for (uint8_t mult2 = 1; mult2 < variations; mult2++)
        {
            for (uint8_t mult3 = 1; mult3 < variations; mult3++)
            {
                // one match is all maps with all player positions
                run_match(mult1, mult2, mult3, pool);
            }
        }
    }
    while (!finished)
    {
        sleep(1);
    }
    auto end_time = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "The best configuration was: " << (int)best_mult1 << " " << (int)best_mult2 << " " << (int)best_mult3 << std::endl;
    std::cout << "Elapsed time: " << seconds.count() << " seconds" << std::endl;
    // fclose(stdout);
    return 0;
}
