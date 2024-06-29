#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <iostream>

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

void print_total_statistics();
void print_statistics();

#endif // STATISTICS_HPP