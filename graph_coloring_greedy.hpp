#include <iostream>
#include <vector>
#include "Exam.hpp"


using namespace std;

vector<int> graph_coloring_greedy(vector<Exam*> all_exams, int n_timeslot, vector<size_t> sorted_index, int n_exams);