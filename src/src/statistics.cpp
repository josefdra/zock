#include "statistics.hpp"
#include "logging.hpp"

int total_nodes_calculated = 0;
int nodes_calculated = 0;
int total_leafs_calculated = 0;
int leafs_calculated = 0;
double total_average_next_player_calculation_time = 0;
double average_next_player_calculation_time = 0;
double total_average_evaluation_time = 0;
double average_evaluation_time = 0;
double total_set_up_moves_time = 0;
double set_up_moves_time = 0;
double total_sorting_time = 0;
double sorting_time = 0;
double total_move_execution_time = 0;
double move_execution_time = 0;

void print_total_statistics()
{
    LOG_INFO("Total nodes calculated: " + std::to_string(total_nodes_calculated));
    LOG_INFO("Total leafs calculated: " + std::to_string(total_leafs_calculated));
    LOG_INFO("Average next player calculation time: " + std::to_string(total_average_next_player_calculation_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average evaluation time: " + std::to_string(total_average_evaluation_time / static_cast<double>(total_leafs_calculated)));
    LOG_INFO("Average set up moves time: " + std::to_string(total_set_up_moves_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average sorting time: " + std::to_string(total_sorting_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average move execution time: " + std::to_string(total_move_execution_time / static_cast<double>(total_nodes_calculated)));
}

void print_statistics()
{
    LOG_INFO("Nodes calculated: " + std::to_string(nodes_calculated));
    LOG_INFO("Leafs calculated: " + std::to_string(leafs_calculated));
    LOG_INFO("Average next player calculation time: " + std::to_string(average_next_player_calculation_time / static_cast<double>(nodes_calculated)));
    LOG_INFO("Average evaluation time: " + std::to_string(average_evaluation_time / static_cast<double>(leafs_calculated)));
    LOG_INFO("Average set up moves time: " + std::to_string(set_up_moves_time / static_cast<double>(nodes_calculated)));
    LOG_INFO("Average sorting time: " + std::to_string(sorting_time / static_cast<double>(nodes_calculated)));
    LOG_INFO("Average move execution time: " + std::to_string(move_execution_time / static_cast<double>(nodes_calculated)));
    total_nodes_calculated += nodes_calculated;
    nodes_calculated = 0;
    total_leafs_calculated += leafs_calculated;
    leafs_calculated = 0;
    total_average_next_player_calculation_time += average_next_player_calculation_time;
    average_next_player_calculation_time = 0;
    total_average_evaluation_time += average_evaluation_time;
    average_evaluation_time = 0;
    total_set_up_moves_time += set_up_moves_time;
    set_up_moves_time = 0;
    total_sorting_time += sorting_time;
    sorting_time = 0;
    total_move_execution_time += move_execution_time;
    move_execution_time = 0;
}