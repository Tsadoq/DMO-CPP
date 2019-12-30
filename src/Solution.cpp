# pragma once
#include "Solution.hpp"
#include "Exam.hpp"
#include <vector>
#include <iostream>
using namespace std;

/*Solution::Solution(std::vector<int> t, std::vector<Exam> e){
    timeslot_per_exams = t;
    exams = e;
}*/

int Solution::check_feasibility(std::vector<int> t, std::vector<Exam*> e) {
    int flag = 0;
    for(int i=0;i<e.size();i++){
        for(int j=0; j<e[i]->conflict_exams.size(); j++){
            if(t[i]==e[i]->conflict_times[j]){
                flag=1;
                return flag;
            }
        }
    }
    return flag;
}