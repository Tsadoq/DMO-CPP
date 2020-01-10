#include <string>
#include <iostream>
#include <vector>
#include "Exam.hpp"
#include "Solution.hpp"
#include <time.h>
#include <sys/timeb.h>

using namespace std;

int read_file_exm(string file_name);
int read_file_slo(string file_name);
vector<vector<int>> read_file_stu(char* file_name,int n_exams,int &total_number_students);
vector<Exam*> inizialize_all_exams();
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) ;
void graph_coloring_greedy(Solution* initial_solution, int n_timeslot, vector<size_t> sorted_index, int n_exams);
vector<vector<int>> neighbours_by_mutation(Solution* solution, vector<size_t> order_for_mutation,int num_mutation,vector<int> possible_timeslots,double perc,int n_exams);
Solution* sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot, string current_instance,double t0);
void neighbours_by_swapping(Solution* solution, int totTimeslots);
double temperature_init(Solution *solution,int n_exams, int total_number_students, int n_timeslot);
void neighbour_by_crossover(Solution* actual_sol,Solution* best_sol, int n_exams, int n_timeslot);
int alternativeColoring(Solution* initial_solution, int n_timeslot,  int n_exams,vector<size_t> sorted_index);
void neighbourhood_by_obj_fun(int index_exam, Solution* solution, int n_timeslot, vector<int> possible_timeslots, int n_exams);
void unscheduling(Solution* sol, int num_unsched);
int rescheduling(Solution* sol, int totTimeslots);
int directional_mutation(Solution* sol, int totTimeslots);
double directional_mutation_final(Solution* sol, int totTimeslots, double best_obj, int num_stud);
void localSearch(Solution* solution, int n_exams, vector<int> possible_timeslots,vector<size_t> sorted_exams);
bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap, int total_number_student);
void neighbours_by_mutation_no_order(Solution* solution,int num_mutation,vector<int> possible_timeslots,int n_exams);