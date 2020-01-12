#include <string>
#include <iostream>
#include <vector>
#include "Exam.hpp"
#include "Solution.hpp"
#include <time.h>
#include <sys/timeb.h>



int read_file_exm(std::string file_name);
int read_file_slo(std::string file_name);
std::vector<std::vector<int>> read_file_stu(char* file_name,int n_exams,int &total_number_students);
std::vector<Exam*> inizialize_all_exams();
template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) ;
Solution* sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int n_timeslot, std::string current_instance);
void alternativeColoring(Solution* initial_solution, int n_timeslot,  int n_exams,std::vector<size_t> sorted_index);
bool unscheduling(Solution* sol, int num_unsched);
bool rescheduling(Solution* sol, int totTimeslots, std::vector<int> possible_timeslots);
void localSearch(Solution* solution, std::vector<int> possible_timeslots,std::vector<size_t> sorted_exams,int n_timeslot);
bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap);