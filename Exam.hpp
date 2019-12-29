# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Exam{
public:
    int id_exam;
    vector<int> conflict_exams; 
    vector<int> conflict_weights;
    vector<int> conflict_times;
    double weight_in_obj_fun;
    int timeslot;
};
