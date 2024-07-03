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

int score = 0;
int total_score = 0;
int enemy_move_value = 0;
int total_enemy_move_value = 0;
int enemy_stone_value = 0;
int total_enemy_stone_value = 0;
int enemy_protected_fields_value = 0;
int total_enemy_protected_fields_value = 0;
int enemy_static_eval = 0;
int total_enemy_static_eval = 0;
int our_move_value = 0;
int total_our_move_value = 0;
int our_stone_value = 0;
int total_our_stone_value = 0;
int our_protected_fields_value = 0;
int total_our_protected_fields_value = 0;
int our_static_eval = 0;
int total_our_static_eval = 0;
int before_bonus_value = 0;
int total_before_bonus_value = 0;
int before_choice_value = 0;
int total_before_choice_value = 0;
int overwrite_value = 0;
int total_overwrite_value = 0;
int no_move_value = 0;
int total_no_move_value = 0;
int bonus_value = 0;
int total_bonus_value = 0;
int choice_value = 0;
int total_choice_value = 0;

void print_total_time_statistics()
{
    LOG_INFO("Total nodes calculated: " + std::to_string(total_nodes_calculated));
    LOG_INFO("Total leafs calculated: " + std::to_string(total_leafs_calculated));
    LOG_INFO("Average next player calculation time: " + std::to_string(total_average_next_player_calculation_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average evaluation time: " + std::to_string(total_average_evaluation_time / static_cast<double>(total_leafs_calculated)));
    LOG_INFO("Average set up moves time: " + std::to_string(total_set_up_moves_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average sorting time: " + std::to_string(total_sorting_time / static_cast<double>(total_nodes_calculated)));
    LOG_INFO("Average move execution time: " + std::to_string(total_move_execution_time / static_cast<double>(total_nodes_calculated)));
}

void print_time_statistics()
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
void print_total_evaluation_statistics()
{
    double average_enemy_move_value = static_cast<double>(total_enemy_move_value) / static_cast<double>(total_leafs_calculated);
    double average_enemy_stone_value = static_cast<double>(total_enemy_stone_value) / static_cast<double>(total_leafs_calculated);
    double average_enemy_protected_fields_value = static_cast<double>(total_enemy_protected_fields_value) / static_cast<double>(total_leafs_calculated);
    double average_enemy_static_eval = static_cast<double>(total_enemy_static_eval) / static_cast<double>(total_leafs_calculated);
    double average_our_move_value = static_cast<double>(total_our_move_value) / static_cast<double>(total_leafs_calculated);
    double average_our_stone_value = static_cast<double>(total_our_stone_value) / static_cast<double>(total_leafs_calculated);
    double average_our_protected_fields_value = static_cast<double>(total_our_protected_fields_value) / static_cast<double>(total_leafs_calculated);
    double average_our_static_eval = static_cast<double>(total_our_static_eval) / static_cast<double>(total_leafs_calculated);
    double average_before_bonus_value = static_cast<double>(total_before_bonus_value) / static_cast<double>(total_leafs_calculated);
    double average_before_choice_value = static_cast<double>(total_before_choice_value) / static_cast<double>(total_leafs_calculated);
    double average_overwrite_value = static_cast<double>(total_overwrite_value) / static_cast<double>(total_leafs_calculated);
    double average_no_move_value = static_cast<double>(total_no_move_value) / static_cast<double>(total_leafs_calculated);
    double average_bonus_value = static_cast<double>(total_bonus_value) / static_cast<double>(total_leafs_calculated);
    double average_choice_value = static_cast<double>(total_choice_value) / static_cast<double>(total_leafs_calculated);

    LOG_INFO("Average enemy move value: " + std::to_string(average_enemy_move_value));
    LOG_INFO("Average enemy stone value: " + std::to_string(average_enemy_stone_value));
    LOG_INFO("Average enemy protected fields value: " + std::to_string(average_enemy_protected_fields_value));
    LOG_INFO("Average enemy static eval: " + std::to_string(average_enemy_static_eval));
    LOG_INFO("Average our move value: " + std::to_string(average_our_move_value));
    LOG_INFO("Average our stone value: " + std::to_string(average_our_stone_value));
    LOG_INFO("Average our protected fields value: " + std::to_string(average_our_protected_fields_value));
    LOG_INFO("Average our static eval: " + std::to_string(average_our_static_eval));
    LOG_INFO("Average before bonus value: " + std::to_string(average_before_bonus_value));
    LOG_INFO("Average before choice value: " + std::to_string(average_before_choice_value));
    LOG_INFO("Average overwrite value: " + std::to_string(average_overwrite_value));
    LOG_INFO("Average no move value: " + std::to_string(average_no_move_value));
    LOG_INFO("Average bonus value: " + std::to_string(average_bonus_value));
    LOG_INFO("Average choice value: " + std::to_string(average_choice_value));
}



void ajdust_time_values()
{
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

void adjust_evaluation_values()
{
    total_score += score;
    total_enemy_move_value += enemy_move_value;
    total_enemy_stone_value += enemy_stone_value;
    total_enemy_protected_fields_value += enemy_protected_fields_value;
    total_enemy_static_eval += enemy_static_eval;
    total_our_move_value += our_move_value;
    total_our_stone_value += our_stone_value;
    total_our_protected_fields_value += our_protected_fields_value;
    total_our_static_eval += our_static_eval;
    total_before_bonus_value += before_bonus_value;
    total_before_choice_value += before_choice_value;
    total_overwrite_value += overwrite_value;
    total_no_move_value += no_move_value;
    total_bonus_value += bonus_value;
    total_choice_value += choice_value;
    score = 0;
    enemy_move_value = 0;
    enemy_stone_value = 0;
    enemy_protected_fields_value = 0;
    enemy_static_eval = 0;
    our_move_value = 0;
    our_stone_value = 0;
    our_protected_fields_value = 0;
    our_static_eval = 0;
    before_bonus_value = 0;
    before_choice_value = 0;
    overwrite_value = 0;
    no_move_value = 0;
    bonus_value = 0;
    choice_value = 0;
}
