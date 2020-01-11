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
Solution* sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int n_timeslot, string current_instance);
void alternativeColoring(Solution* initial_solution, int n_timeslot,  int n_exams,vector<size_t> sorted_index);
bool unscheduling(Solution* sol, int num_unsched);
bool rescheduling(Solution* sol, int totTimeslots);
void localSearch(Solution* solution, vector<int> possible_timeslots,vector<size_t> sorted_exams,int n_timeslot);
bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap);