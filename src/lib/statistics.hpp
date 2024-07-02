#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <iostream>

// Time statistics
extern int total_nodes_calculated;
extern int nodes_calculated;
extern int total_leafs_calculated;
extern int leafs_calculated;
extern double total_average_next_player_calculation_time;
extern double average_next_player_calculation_time;
extern double total_average_evaluation_time;
extern double average_evaluation_time;
extern double total_set_up_moves_time;
extern double set_up_moves_time;
extern double total_sorting_time;
extern double sorting_time;
extern double total_move_execution_time;
extern double move_execution_time;

// Evaluation statistics
extern int score;
extern int total_score;
extern int this_calculation_score;
extern int enemy_move_value;
extern int total_enemy_move_value;
extern int this_calculation_enemy_move_value;
extern int enemy_stone_value;
extern int total_enemy_stone_value;
extern int this_calculation_enemy_stone_value;
extern int enemy_protected_fields_value;
extern int total_enemy_protected_fields_value;
extern int this_calculation_enemy_protected_fields_value;
extern int enemy_static_eval;
extern int total_enemy_static_eval;
extern int this_calculation_enemy_static_eval;
extern int our_move_value;
extern int total_our_move_value;
extern int this_calculation_our_move_value;
extern int our_stone_value;
extern int total_our_stone_value;
extern int this_calculation_our_stone_value;
extern int our_protected_fields_value;
extern int total_our_protected_fields_value;
extern int this_calculation_our_protected_fields_value;
extern int our_static_eval;
extern int total_our_static_eval;
extern int this_calculation_our_static_eval;
extern int before_bonus_value;
extern int total_before_bonus_value;
extern int this_calculation_before_bonus_value;
extern int before_choice_value;
extern int total_before_choice_value;
extern int this_calculation_before_choice_value;
extern int overwrite_value;
extern int total_overwrite_value;
extern int this_calculation_overwrite_value;
extern int no_move_value;
extern int total_no_move_value;
extern int this_calculation_no_move_value;
extern int bonus_value;
extern int total_bonus_value;
extern int this_calculation_bonus_value;
extern int choice_value;
extern int total_choice_value;
extern int this_calculation_choice_value;

void print_total_time_statistics();
void print_time_statistics();

void print_total_evaluation_statistics();
void print_evaluation_statistics();

void ajdust_time_values();
void adjust_evaluation_values();

#endif // STATISTICS_HPP
