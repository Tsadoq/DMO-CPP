#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp" 
#include<bits/stdc++.h> 
#include"TSforInitialSolution.hpp"


using namespace std;


int TSforInitialSolution::tabu_search(Solution *sol, int n_exams, int n_timeslot, vector<vector<int>> conflict_matrix){
    
    vector <int> tabu_list=vector<int>();
    vector <int> not_assigned_timeslot;
    vector <int> exams_at_time_t;
    int exam_idx;
    int num_min_conflicts;
    int num_conflict_now;
    // iterator
    int i=0;
    bool forbidden;
    int best_time;

    // initializa vector of exam to be assigned to a timeslot
    for(int i=0;i<n_exams;i++){
        if (sol->timeslot_per_exams[i]==-2){
            // index of exams with no timeslot
            not_assigned_timeslot.push_back(i);
        }
    }    
    
    cout<<"Number of timeslot with problems before TS: "<<not_assigned_timeslot.size()<<endl;

    while(i<maxIter && not_assigned_timeslot.size()>0){

        num_min_conflicts=INT_MAX;
        exam_idx=not_assigned_timeslot[0];        
        best_time=-1;

        for (int t=1;t<=n_timeslot;t++){ 
            forbidden=false;
            exams_at_time_t=vector<int>();
            for(int h=0;h<n_exams;h++){
                if(sol->timeslot_per_exams[h]==t){
                    exams_at_time_t.push_back(h);
                }
            }
            for (auto tl:tabu_list){
                for (auto exam:exams_at_time_t){                    
                    if (tl==exam && conflict_matrix[tl][exam_idx]>0){
                        forbidden=true;
                    }
                }
            }                 
            // se la mossa non è proibita da TL || spostare non mi crea conflitti
            if (forbidden==false){
                // se non è proibita:guardo se mi conviene provare a spostarlo
                // calcolo quanti conflitti                
                num_conflict_now=0;
                for(auto ex:exams_at_time_t){
                    num_conflict_now+=conflict_matrix[ex][exam_idx];
                }
                // guardo se ottengo meno conflitti di prima
                if (num_conflict_now<num_min_conflicts){
                    num_min_conflicts=num_conflict_now;
                    best_time=t;                  
                }
            }
        }        
        i++;
        if(best_time==-1){  
            cout<<"Number of timeslot with problems after TS hey: "<<not_assigned_timeslot.size()<<endl;          
            return 3;
        }else{
            not_assigned_timeslot.erase(not_assigned_timeslot.begin());
            // aggiorno i timeslot interessati e exam_idx
            sol->timeslot_per_exams[exam_idx]=best_time; 
            for (auto neigh:sol->all_exams[exam_idx]->conflict_exams){                  
                if (sol->timeslot_per_exams[neigh-1]==best_time){
                    sol->timeslot_per_exams[neigh-1]=-2;
                    not_assigned_timeslot.push_back(neigh-1);
                }
            }
            // aggiorno i timeslot conflittuali e i timeslot negli Exam
            sol->update_timeslots(n_exams);
            // aggiorno la TL
            if(tabu_list.size()<=dim){
                tabu_list.push_back(exam_idx);
            }else{// FIFO UPDATE
                tabu_list.erase(tabu_list.begin());
                tabu_list.push_back(exam_idx);
            }
        }
    }
    cout<<"Number of timeslot with problems after TS hey: "<<not_assigned_timeslot.size()<<endl;
    return 5;
}
