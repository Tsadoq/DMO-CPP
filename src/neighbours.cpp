#include "header.hpp" 
#include <string> 
#include <iostream> 
#include <numeric>  
#include <random>   
#include <algorithm> 
#include <vector> 
#include <chrono>
#include <iterator> 
#include "Exam.hpp" 
#include "Solution.hpp" 
 
using namespace std; 
 
bool unscheduling(Solution* sol, int num_unsched){
    vector<int> positions;
    int h;
    if (num_unsched==0){
        return true;
    }
    for (int i= 0; i<sol->all_exams.size(); i++){
        positions.push_back(i);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
    std::shuffle(positions.begin(), positions.end(),std::default_random_engine(seed) );
    // random order so that I can choose from the num_unsch first positions
    for (int j= 0; j < num_unsched; j++){
        sol->timeslot_per_exams[positions[j]] = -1; // mark the unscheduled exams
        sol->all_exams[positions[j]]->timeslot = -1;
        // now we need to change also this timeslots in the conflict matrix of his conflictuals
         for (auto k : sol->all_exams[positions[j]]->conflict_exams){ 
                h=0; 
                while(sol->all_exams[k-1]->conflict_exams[h] != sol->all_exams[positions[j]]->id_exam){ 
                    h++;
                } 
                sol->all_exams[k-1]->conflict_times[h]= -1; 
            }
    }
    return false;
}

bool rescheduling(Solution* sol, int totTimeslots, int n_exams){

    int fail=0;
    int max_fail=n_exams/3;
    //vector<int> unsched_exams_pos=vector<int>();
    vector<Exam*> unsched_exams=vector<Exam*>();
    //vector<int> num_available=vector<int>();
    int k;
    int counter = 0; 

    for (int i =0; i< sol->all_exams.size(); i++){
        if (sol->timeslot_per_exams[i] == -1){
            //unsched_exams_pos.push_back(i);
            // numero di conflitti per l'esame i
            unsched_exams.push_back(sol->all_exams[i]);
        }
        sol->all_exams[i]->num_conflict = sol->all_exams[i]->conflict_exams.size();
    }   

    vector<int> possible_timeslots=vector<int>();
    for (int i=0; i<totTimeslots;i++){  // possible timeslot rimane uguale per tutti, e il vettore [1, ntimeslot]
            possible_timeslots.push_back(i+1);
    }

    vector<int> not_available;
    vector<int> possible_mutation;
    vector<size_t> order_not_schedule;
    vector<int> not_available_time;
    vector<int> available_time;



    int pos;
    int kk;
    int rand_index;
    int pos_conf;
    
    while (unsched_exams.size()!=0){
        //cout<<unsched_exams.size()<<endl;
        //cout<<unsched_exams_pos.size()<<endl;

        if (fail>=max_fail){
            //cout<<"fail "<<unsched_exams.size()<<endl;
            return false;
        }

        not_available=vector<int>();
        possible_mutation=vector<int>();
        not_available_time=vector<int>();
        available_time=vector<int>();
        order_not_schedule=vector<size_t>();

        for(auto un_ex: unsched_exams){
            not_available_time=un_ex->conflict_times;
            std::sort(not_available_time.begin(), not_available_time.end());  
            std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_time.begin(),  
                    not_available_time.end(),inserter(available_time, available_time.begin()));
                    un_ex->available_times=available_time.size();
            available_time=vector<int>();
            not_available_time=vector<int>();
        }

        std::sort(unsched_exams.begin(), unsched_exams.end(), [](Exam* a, Exam* b) {
            if ( a->available_times!= b->available_times) return a->available_times < b->available_times;
                return a->num_conflict > b->num_conflict;
        });

        /*for(auto u:unsched_exams){
            cout<<"exam "<<u->id_exam<<" available time "<<u->available_times<<" num conf "<<u->num_conflict<<endl;
        }*/
        //order_not_schedule=sort_indexes(weight_unsched);        
        
        // esame più difficile da piazzare
        //cout<<"size: "<<unsched_exams_pos.size()<<endl;
        pos = unsched_exams[0]->id_exam-1;
        // vado a vedere quali timeslot sono disponibili
        for (int j = 0; j <sol->all_exams[pos]->conflict_times.size(); j++){
            if (sol->all_exams[pos]->conflict_times[j] != -1){
                not_available.push_back(sol->all_exams[pos]->conflict_times[j]);  // mi segno i timeslot non disponibili
                //cout<<"I am there too for the "<<j+1<<" time(s)"<<endl;
            }    
        }
        std::sort(not_available.begin(), not_available.end());  
        std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available.begin(),  
                    not_available.end(),inserter(possible_mutation, possible_mutation.begin()));
        if (possible_mutation.size()> 0){
            //cout<<"ok"<<endl;
            rand_index = rand() % possible_mutation.size();
            sol->all_exams[pos]->timeslot = possible_mutation[rand_index];
            sol->timeslot_per_exams[pos] = possible_mutation[rand_index];
            for (auto h : sol->all_exams[pos]->conflict_exams){ 
                k=0; 
                while(sol->all_exams[h-1]->conflict_exams[k] != sol->all_exams[pos]->id_exam){ 
                    k++;
                    //cout<<"riga 438"<<endl;
                } 
                sol->all_exams[h-1]->conflict_times[k]=possible_mutation[rand_index];
            } 
            //cout<<"riga 442"<<endl;
            //cout<<"pos "<<pos<<endl;
            unsched_exams.erase(unsched_exams.begin());
        }else{
            fail++;
            //cout<<"numero conflittuali "<<sol->all_exams[pos]->conflict_times.size()<<" esame "<<sol->all_exams[pos]->id_exam<<endl;
            // unschedulo tutti i conflittuali di pos
            for (int j = 0; j <sol->all_exams[pos]->conflict_times.size(); j++){
                
                if(sol->all_exams[pos]->conflict_times[j]!=-1){ 
                    //cout<<"unsched"<<endl;                      
                    sol->all_exams[pos]->conflict_times[j]=-1;
                    pos_conf=sol->all_exams[pos]->conflict_exams[j]-1;
                    unsched_exams.push_back(sol->all_exams[pos_conf]);
                    //weight_unsched.push_back(sol->all_exams[pos_conf]->conflict_exams.size());
                    sol->timeslot_per_exams[pos_conf]=-1;
                    sol->all_exams[pos_conf]->timeslot=-1;
                    // dico a tutti gli esami conflittuali dell'esame unschedulato che l'ho unschedulato
                    for(auto conf_exam : sol->all_exams[pos_conf]->conflict_exams){
                        kk=0; 
                        while(sol->all_exams[conf_exam-1]->conflict_exams[kk] != pos_conf+1){ 
                            kk++;
                        } 
                        sol->all_exams[conf_exam-1]->conflict_times[kk]=-1;                        
                    }
                }
            }    
        }
        
        //not_available.clear();
        //possible_mutation.clear();
    }
    return true;
}

bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap, int total_number_student){
    vector<int> exams = solution->timeslot_per_exams;
    
    int t1 = ii+1;
    int t2 = jj+1;
    
    vector<int> e1;
    vector<int> e2;
    for (int i = 0; i<exams.size(); i++){
        if (exams[i] == t1){
            exams[i]=t2;
        }else{
            if (exams[i] == t2){
                exams[i]=t1;
            }
        }
    }
    solution->update_timeslots(exams.size());
    solution->update_weights(exams.size());
    if(solution->objective_function(exams.size(), total_number_student)<obj_pre_swap){
        return true;
    }else{
        return false;
    }
}