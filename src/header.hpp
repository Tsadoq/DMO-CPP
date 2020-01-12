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
Solution* sa(Solution* solution, struct timeb start, int timelimit,std::string current_instance, bool inception);
void alternativeColoring(Solution* initial_solution, std::vector<size_t> sorted_index);
bool unscheduling(Solution* sol, int num_unsched);
bool rescheduling(Solution* sol);
void localSearch(Solution* solution, std::vector<size_t> sorted_exams);
bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap);
void neighbours_by_mutation(Solution* solution, std::vector<size_t>  order_for_mutation, int num_mutation, double perc);
void neighbours_by_swapping(Solution* solution);