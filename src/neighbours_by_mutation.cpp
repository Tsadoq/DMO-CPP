#include "header.hpp" 
#include <string> 
#include <iostream> 
#include <numeric>  
#include <random>   
#include <algorithm> 
#include <vector> 
#include <iterator> 
#include "Exam.hpp" 
#include "Solution.hpp" 
 
using namespace std; 
 
vector<vector<int>> neighbours_by_mutation(Solution* solution, vector<size_t> order_for_mutation,int num_mutation, 
                    vector<int> possible_timeslots,double perc,int n_exams){ 
                         
    vector<vector<int>> mutations_vector;  
    vector<int> single_mutation=vector<int> (2);                                        
    Exam* exam_mutate; 
    int is_void=0; 
    vector<int> not_available_timeslots; 
    vector<int> available_timeslots; 
    int k; 
    int new_timeslot; 
    int randomIndex; 
    int size_random=(int) (perc*n_exams); 
    std::vector<int> indexes= vector<int>(size_random); 
    std::iota(indexes.begin(), indexes.end(), 0); 
    std::random_device rd; 
    std::shuffle(indexes.begin(), indexes.end(), rd); 
    //std::random_shuffle(indexes.begin(), indexes.end()); 
         
    for(int i=0;i<num_mutation+is_void && i<size_random;i++){ 
        // exam I'm trying to mutate 
        available_timeslots=vector<int> (); 
        exam_mutate=solution->all_exams[order_for_mutation[indexes[i]]]; 
        not_available_timeslots=exam_mutate->conflict_times; 
        not_available_timeslots.push_back(exam_mutate->timeslot); 
        // sort vector because set_difference works with sorted arrays 
        sort(not_available_timeslots.begin(), not_available_timeslots.end()); 
        // find available timeslot 
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_timeslots.begin(),  
                        not_available_timeslots.end(),inserter(available_timeslots, available_timeslots.begin())); 
        if(available_timeslots.size()==0){ 
            is_void++; 
        }else{ 
            randomIndex = rand() % available_timeslots.size(); 
            new_timeslot=available_timeslots[randomIndex]; 
            // update timeslot in exam I want to mutate 
            exam_mutate->timeslot=new_timeslot; 
             
            // update timeslot in all conflicting exam with respect to exam_mutate 
            for (auto j : exam_mutate->conflict_exams){ 
                k=0; 
                while(solution->all_exams[j-1]->conflict_exams[k] != exam_mutate->id_exam){ 
                    k++; 
                } 
                solution->all_exams[j-1]->conflict_times[k]=new_timeslot; 
            } 
            // insert exam I want to mutate 
            single_mutation[0]=exam_mutate->id_exam; 
            // insert time slot in which I want to schedule exam 
            single_mutation[1]=new_timeslot; 
            solution->timeslot_per_exams[exam_mutate->id_exam-1]=new_timeslot;             
            mutations_vector.push_back(single_mutation); 
        } 
    } 
    return mutations_vector; 
}