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

std::string root_directory = "/home/josefdra/ZOCK/g01";
std::string log_directory;
std::vector<bool> disqualified;

struct FileInfo
{
    std::string path;
    time_t creation_time;
};

std::array<std::tuple<std::string, uint8_t>, 21> maps{
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
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/comp2020_02_8p.map", 8),
    std::tuple<std::string, uint8_t>(root_directory + "/automated_testing/maps/evenMoreTransitions.map", 4)};

pid_t start_binary(const char *path, const std::vector<const char *> &args)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        //  Open /dev/null for writing
        int fd = open("/dev/null", O_WRONLY);
        // Redirect stdout and stderr to /dev/null
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

void run_map(std::tuple<std::string, uint8_t> &map, std::string string_game_number)
{
    std::string map_path;
    uint8_t player_count;
    std::tie(map_path, player_count) = map;

    // Path to the binaries
    std::string server = root_directory + "/automated_testing/server_binary/server_nogl";
    std::string client = root_directory + "/automated_testing/client_binary/client01";
    std::string trivial_ai = "/home/josefdra/ZOCK/ges/reversi-binaries/trivialai/ai_trivial";

    // Arguments for the server
    std::vector<const char *> arguments_server = {"-m", map_path.c_str(), "-d", "3", nullptr};
    start_binary(server.c_str(), arguments_server);

    // Vector to store child PIDs
    std::vector<pid_t> client_pids;

    usleep(500000);
    // Arguments for the clients
    // Convert player number to std::string and then to const char*
    std::string player_num_str = std::to_string(static_cast<int>(1));
    const char *player_num_cstr = player_num_str.c_str();
    std::vector<const char *> arguments_clients = {player_num_cstr, string_game_number.c_str(), nullptr};
    pid_t client_pid = start_binary(client.c_str(), arguments_clients);
    if (client_pid > 0)
    {
        std::cout << "success at starting client " << 1 << " for game " << string_game_number << std::endl;
        client_pids.push_back(client_pid);
    }

    for (uint8_t p = 1; p < player_count; p++)
    {
        usleep(500000);
        std::vector<const char *> arguments_trivial_ai = {nullptr};
        pid_t client_pid = start_binary(trivial_ai.c_str(), arguments_trivial_ai);
        if (client_pid > 0)
        {
            std::cout << "success at starting trivial_ai " << p << " for game " << string_game_number << std::endl;
            client_pids.push_back(client_pid);
        }
    }
    char winner = '0';
    uint8_t counter = 1;
    for (pid_t client_pid : client_pids)
    {
        int status;
        pid_t result = waitpid(client_pid, &status, 0);
        if (result > 0)
        {
            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 1)
                {
                    std::cout << "won game " << string_game_number << std::endl;
                    winner += counter;
                }
                else if (exit_status == 2)
                {
                    winner += counter;
                    counter = 0;
                    break;
                }
            }
        }
        else
        {
            // Handle errors from waitpid
            std::cerr << "Error waiting for child process with PID " << client_pid << std::endl;
        }
        counter++;
    }
    std::cout << "Game " << string_game_number << " finished" << std::endl;
    if (counter == 0)
    {
        disqualified[atoi(string_game_number.c_str())] = true;
    }
}

int main()
{
    std::string filename = root_directory + +"/automated_testing/values/ges_log.txt";
    freopen(filename.c_str(), "w", stdout);

    auto start_time = std::chrono::steady_clock::now();
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    log_directory = std::string(cwd);
    int game_number = 0;
    std::string string_game_number = std::to_string(game_number);
    std::vector<std::thread> threads;
    std::vector<std::string> log_files;
    for (auto &map : maps)
    {
        threads.push_back(std::thread(run_map, std::ref(map), string_game_number));
        sleep(10);
        disqualified.push_back(false);
        game_number++;
        string_game_number = std::to_string(game_number);
    }
    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    std::vector<FileInfo> fileInfos;
    std::string command = "find " + std::string(log_directory) + " -type f -exec stat --format '%W %n' {} +";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Pipe could not be opened" << std::endl;
        return 1;
    }
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::istringstream iss(buffer);
        FileInfo fileInfo;
        iss >> fileInfo.creation_time >> fileInfo.path;
        if (fileInfo.creation_time > 0)
        {
            fileInfos.push_back(fileInfo);
        }
    }
    pclose(pipe);
    std::sort(fileInfos.begin(), fileInfos.end(), [](const FileInfo &a, const FileInfo &b)
              { return a.creation_time > b.creation_time; });
    for (auto &elem : fileInfos)
    {
        log_files.push_back(elem.path);
    }
    for (uint8_t i = 0; i < maps.size(); i++)
    {
        std::string command = "mv " + log_files[maps.size() - 1 - i] + " " + root_directory + "/automated_testing/server_binary/logs/game_" + std::to_string(i) + ".txt";
        system(command.c_str());
        if (disqualified[i] == false)
        {
            std::string command = "rm " + root_directory + "/automated_testing/server_binary/logs/game_" + std::to_string(i) + ".txt";
            system(command.c_str());
        }
    }
    auto end_time = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Played " << game_number << " games" << std::endl;
    std::cout << "Elapsed time: " << seconds.count() << " seconds" << std::endl;
    fclose(stdout);
    return 0;
}