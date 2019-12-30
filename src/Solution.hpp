# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Solution{
    public:
    vector <int> timeslot_per_exams;
    vector <Exam*> all_exams;
    vector <int> num_neighbours_for_exams;
    void solution_update(vector<vector<int>>, int);
    int check_feasibility(std::vector<int>, std::vector<Exam*>);
};