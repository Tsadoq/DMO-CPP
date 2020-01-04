#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp" 
#include<bits/stdc++.h> 
#include"TSforInitialSolution.hpp"


using namespace std;

void TSforInitialSolution::tabu_search(Solution *sol, int n_exams, int n_timeslot){
    
    vector <int> tabu_list=vector<int>();
    vector <int> num_neighbour_exams=sol->num_neighbours_for_exams;

    vector <int> not_assigned_timeslot;
    for(int i=0;i<n_exams;i++){
        if (sol->timeslot_per_exams[i]==-2){
            // index of exams with no timeslot
            not_assigned_timeslot.push_back(i);
        }
    }    
    int index=0;
    int exam_idx;
    int num_min_conflicts;
    int num_conflict_now;
    int i=0;
    bool not_forbidden;
    bool no_conflicts;
    bool not_done;
    int count;
    cout<<"Number of TS with problems before TS: "<<not_assigned_timeslot.size()<<endl;
    while(i<maxIter && not_assigned_timeslot.size()>0){
        if (index>=not_assigned_timeslot.size()){
            cout<<"Number of TS with problems after TS: "<<not_assigned_timeslot.size()<<endl;
            return;
        }
        count=0;
        for(int u=0;u<n_exams;u++){
            if(sol->timeslot_per_exams[u]<0)
                count++;
        }
        cout<<count<<endl;

        for(auto rr:not_assigned_timeslot){
            cout<<rr<<" ";
        }
        cout<<"\n";
        not_done=true;
        num_min_conflicts=INT_MAX;
        exam_idx=not_assigned_timeslot[index];
        not_forbidden=true;

        for (int t=1;t<=n_timeslot;t++){
            for (auto tl:tabu_list){
                if (tl==exam_idx){
                    not_forbidden=false;
                }
            }            
            no_conflicts=true;
            for (auto time:sol->all_exams[exam_idx]->conflict_times){
                if (time==t && time>0){
                    no_conflicts=false;
                }
            }
            // guardo se la mossa non è proibita da TL || spostare non mi crea conflitti
            if (not_forbidden || no_conflicts){
                // se non è proibita:guardo se mi conviene provare a spostarlo
                // calcolo quanti conflitti                
                num_conflict_now=0;
                for(int timeslot=0; timeslot<sol->all_exams[exam_idx]->conflict_times.size();timeslot++){
                    if (sol->all_exams[exam_idx]->conflict_times[timeslot]==t){
                        num_conflict_now+=sol->all_exams[exam_idx]->conflict_weights[timeslot];
                    }
                }
                // guardo se ottengo meno conflitti di prima
                if (num_conflict_now<num_min_conflicts){
                    not_done=false;
                    num_min_conflicts=num_conflict_now;
                    not_assigned_timeslot.erase(not_assigned_timeslot.begin()+index);
                    //index--;
                    // aggiorno i timeslot interessati e exam_idx
                    sol->timeslot_per_exams[exam_idx]=t; 
                    for (auto neigh:sol->all_exams[exam_idx]->conflict_exams){                  
                        if (sol->timeslot_per_exams[neigh-1]==t){
                            sol->timeslot_per_exams[neigh-1]=-2;
                            not_assigned_timeslot.push_back(neigh-1);
                        }
                    }

                    // aggiorno i timeslot conflittuali e i timeslot negli Exam
                    sol->update_timeslots(n_exams);                   
                }
            }
        }
        
        i++;
        if(not_done){            
            index++;
        }else{
            // aggiorno la TL
            if(tabu_list.size()<=dim){
                tabu_list.push_back(exam_idx);
            }else{// FIFO UPDATE
                tabu_list.erase (tabu_list.begin());
                tabu_list.push_back(exam_idx);
        }
        }
    }
    cout<<"Number of TS with problems after TS hey: "<<not_assigned_timeslot.size()<<endl;
}
