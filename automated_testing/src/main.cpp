#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <vector>

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

int main()
{
    uint8_t player_count = 2;

    // Path to the binaries
    const char *server = "/home/josefdra/ZOCK/g01/automated_testing/server_binary/server_nogl";
    const char *client = "/home/josefdra/ZOCK/g01/automated_testing/client_binary/client01";

    // Arguments for the server
    std::vector<const char *> arguments_server = {"/home/josefdra/ZOCK/g01/automated_testing/maps/2019_comp_05_2p.map", nullptr};
    start_binary(server, arguments_server);

    // Vector to store child PIDs
    std::vector<pid_t> client_pids;

    for (uint8_t p = 0; p < player_count; p++)
    {
        usleep(500000);
        // Arguments for the clients
        // Convert player number to std::string and then to const char*
        std::string player_num_str = std::to_string(static_cast<int>(p + 1));
        const char *player_num_cstr = player_num_str.c_str();
        std::vector<const char *> arguments_clients = {player_num_cstr, nullptr};
        pid_t client_pid = start_binary(client, arguments_clients);
        if (client_pid > 0)
        {
            client_pids.push_back(client_pid);
        }
    }

    for (pid_t client_pid : client_pids)
    {
        int status;
        pid_t result = waitpid(client_pid, &status, 0);
        if (result > 0)
        {
            if (WIFEXITED(status))
            {
                int exit_status = WEXITSTATUS(status);
                std::cout << "Child process with PID " << client_pid << " exited with status " << exit_status << std::endl;
            }
        }
        else
        {
            // Handle errors from waitpid
            std::cerr << "Error waiting for child process with PID " << client_pid << std::endl;
        }
    }

    return 0;
}