#include<string>
#include <iostream>
#include <vector>
#include "Exam.hpp"

using namespace std;

int read_file_exm(string file_name);
int read_file_slo(string file_name);
vector<vector<int>> read_file_stu(char* file_name,int n_exams,int &total_number_students);
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) ;
vector<int> graph_coloring_greedy(vector<Exam*> all_exams, int n_timeslot, vector<size_t> sorted_index, int n_exams);
vector<vector<int>> neighbours_by_mutation(vector<Exam*> all_exams, vector<size_t> order_for_mutation,int num_mutation,vector<int> possible_timeslots);


