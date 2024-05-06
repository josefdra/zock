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

std::string log_directory;
std::vector<uint16_t> won_games;
std::vector<uint16_t> finished_games;
std::vector<std::array<uint16_t, 3>> mults;
int total_games = 75 * 25 * 25 * 25;
int game_number = 0;
int total_won_games = 0;
int finished_games_number = 0;
uint16_t most_won_games = 0;
uint16_t best_mult1 = 0;
uint16_t best_mult2 = 0;
uint16_t best_mult3 = 0;
bool finished = false;
int threads = 200;

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

pid_t start_binary(const char *path, const std::vector<const char *> &args, std::string &game_number, uint8_t client_position)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        int fd;
        if (args[0] != nullptr && std::strcmp(args[0], "-m") == 0)
        {
            std::string filename = root_directory + +"/automated_testing/server_binary/logs/game_" + game_number + "_server_client_position_" + std::to_string(client_position) + ".txt";
            // Open /dev/null for throwing console outputs away
            fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        else
        {
            // Open /dev/null for throwing console outputs away
            fd = open("/dev/null", O_WRONLY);
        }
        // Redirect stdout and stderr
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        // Close the opened file descriptor as it's no longer needed
        close(fd);

        // Prepare arguments for execvp
        std::vector<const char *> exec_args = {path};
        exec_args.insert(exec_args.end(), args.begin(), args.end());
        exec_args.push_back((nullptr)); // execvp expects a null-terminated array

        // Execute the binary with arguments
        execvp(path, const_cast<char *const *>(exec_args.data()));
        // If execvp returns, there was an error
        std::cerr << "Failed to execute " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Failed to fork
        std::cerr << "Failed to fork for " << path << std::endl;
    }
    return pid;
}

void run_map(std::string &map_path, uint8_t player_count, std::string string_game_number, uint8_t client_position, uint16_t mult1, uint16_t mult2, uint16_t mult3, int var, int port)
{
    // Path to the binaries
    std::string server = root_directory + "/automated_testing/server_binary/server_nogl";
    std::string client = root_directory + "/automated_testing/client_binary/client01";
    std::string trivial_ai = "/home/josefdra/ZOCK/ges/reversi-binaries/trivialai/ai_trivial";

    std::string port_string = std::to_string(port);
    std::string ai_port_string = "127.0.0.1:" + std::to_string(port);

    // Arguments for the server
    std::vector<const char *> arguments_server = {"-m", map_path.c_str(), "-d", "1", "-p", port_string.c_str(), nullptr};
    start_binary(server.c_str(), arguments_server, string_game_number, client_position);
    // std::cout << "success at starting server for game " << string_game_number << std::endl;

    // Vector to store child PIDs
    std::vector<pid_t> client_pids;
    // Arguments for the clients
    // Convert player number to std::string and then to const char*
    std::string player_num_str = std::to_string(static_cast<int>(1));
    const char *player_num_cstr = player_num_str.c_str();
    std::vector<const char *> arguments_clients = {player_num_cstr, string_game_number.c_str(), (std::to_string(client_position)).c_str(), (std::to_string(mult1)).c_str(), (std::to_string(mult2)).c_str(), (std::to_string(mult3)).c_str(), port_string.c_str(), nullptr};
    std::vector<const char *> arguments_trivial_ai = {"-s", ai_port_string.c_str(), nullptr};
    std::unordered_set<pid_t> client_ids;

    sleep(1);

    for (uint16_t p = 0; p < player_count; p++)
    {
        pid_t client_pid;
        if (p == client_position)
        {
            client_pid = start_binary(client.c_str(), arguments_clients, string_game_number, client_position);
            client_ids.insert(client_pid);
        }
        else
        {
            client_pid = start_binary(trivial_ai.c_str(), arguments_trivial_ai, string_game_number, client_position);
        }
        if (client_pid > 0)
        {
            if (p == client_position)
            {
                // std::cout << "success at starting client for game " << string_game_number << std::endl;
            }
            else
            {
                // std::cout << "success at starting trivial_ai " << p << " for game " << string_game_number << std::endl;
            }
            client_pids.push_back(client_pid);
        }
        usleep(500000);
    }
    bool disqualified = false;
    for (pid_t client_pid : client_pids)
    {
        int status;
        pid_t result = waitpid(client_pid, &status, 0);
        if (client_ids.find(client_pid) != client_ids.end())
        {
            finished_games[var] = finished_games[var] + 1;
            if (result > 0)
            {
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 1)
                    {
                        won_games[var] = won_games[var] + 1;
                    }
                    else if (exit_status == 2)
                    {
                        disqualified = true;
                        break;
                    }
                }
            }
            else
            {
                // Handle errors from waitpid
                std::cerr << "Error waiting for child process with PID " << client_pid << std::endl;
            }
        }
    }
    // std::cout << "Game " << string_game_number << " finished" << std::endl;
    if (!disqualified)
    {
        std::string command = "rm " + root_directory + "/automated_testing/server_binary/logs/game_" + string_game_number + "_server_client_position_" + std::to_string(client_position) + ".txt";
        system(command.c_str());
    }
}

void rate(int var)
{
    while (finished_games[var] != 75)
    {
        sleep(1);
    }

    std::cout << "won " << won_games[var] << "/75 with configuration: " << mults[var][0] << " " << mults[var][1] << " " << mults[var][2] << std::endl;
    if (most_won_games < won_games[var])
    {
        most_won_games = won_games[var];
        best_mult1 = mults[var][0];
        best_mult2 = mults[var][1];
        best_mult3 = mults[var][2];
    }
    total_won_games = total_won_games + won_games[var];
    std::string command = "rm " + log_directory + "/*.txt";
    system(command.c_str());
    finished_games_number = finished_games_number + 75;
    float status = ((float)finished_games_number / (float)total_games) * 100;
    std::cout << std::setprecision(10) << status << "% done" << std::endl;
    std::cout << "best configuration at the moment: " << best_mult1 << " " << best_mult2 << " " << best_mult3 << std::endl;
    if (var == 25 * 25 * 25)
    {
        finished = true;
    }
}

void run_games(uint16_t mult1, uint16_t mult2, uint16_t mult3, ThreadPool &pool)
{
    int var = won_games.size();
    won_games.push_back(0);
    finished_games.push_back(0);
    int port = 7000;
    for (auto &map : maps)
    {
        std::string map_path;
        uint8_t player_count;
        std::tie(map_path, player_count) = map;

        for (uint8_t p = 0; p < player_count; p++)
        {
            std::string string_game_number = std::to_string(game_number);
            pool.enqueue(run_map, std::ref(map_path), player_count, string_game_number, p, mult1, mult2, mult3, var, port);
            game_number++;
            if (port == 7008)
            {
                port = 7000;
            }
            else
            {
                port++;
            }
            sleep(2);
        }
    }
    pool.enqueue(rate, var);
}

void end(std::chrono::_V2::steady_clock::time_point &start_time)
{
    while (finished != true)
    {
        sleep(1);
    }
    auto end_time = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Played " << total_games << " games and won " << total_won_games << std::endl;
    std::cout << "The best configuration was: " << best_mult1 << " " << best_mult2 << " " << best_mult3 << ", winning " << most_won_games << "/75 games";
    std::cout << "Elapsed time: " << seconds.count() << " seconds" << std::endl;
    fclose(stdout);
}

int main()
{
    std::string filename = root_directory + +"/automated_testing/values/ges_log.txt";
    freopen(filename.c_str(), "w", stdout);
    auto start_time = std::chrono::steady_clock::now();
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    log_directory = std::string(cwd);
    ThreadPool pool(threads);
    uint8_t variations = 26;

    for (uint16_t mult1 = 1; mult1 < variations; mult1++)
    {
        for (uint16_t mult2 = 1; mult2 < variations; mult2++)
        {
            for (uint16_t mult3 = 1; mult3 < variations; mult3++)
            {
                run_games(mult1, mult2, mult3, pool);
                mults.push_back(std::array<uint16_t, 3>{mult1, mult2, mult3});
            }
        }
    }
    pool.enqueue(end, start_time);
    return 0;
}