# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Solution{
    public:
    vector <int> timeslot_per_exams;
    vector <Exam*> total_exams;
    //Solution(std::vector <int>, std::vector <Exam>);
    int check_feasibility(std::vector<int>, std::vector<Exam*>);
};