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

 

 
bool unscheduling(Solution* sol, int num_unsched){
    std::vector<int> positions=std::vector<int>(sol->n_exams);
    int h;
    if (num_unsched==0){
        return true;
    }
    for (int i= 0; i<sol->n_exams; i++){
        positions.push_back(i);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
    std::shuffle(positions.begin(), positions.end(),std::default_random_engine(seed) );
    // random order so that I can choose from the num_unsch first positions
    for (int j= 0; j < num_unsched; j++){
        sol->timeslot_per_exams[positions[j]] = -1; // mark the unscheduled exams
        // now we need to change also this timeslots in the conflict matrix of his conflictuals
         for (auto k : sol->all_exams[positions[j]]->conflict_exams){ 
                h=0; 
                while(sol->all_exams[k]->conflict_exams[h] != sol->all_exams[positions[j]]->id_exam){ 
                    h++;
                } 
                sol->all_exams[k]->conflict_times[h]= -1; 
            }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool rescheduling(Solution* sol, int totTimeslots){

    int n_exams=sol->n_exams;
    int fail=0;
    int max_fail=n_exams/3;
    std::vector<Exam*> unsched_exams=std::vector<Exam*>();
    unsched_exams.reserve(n_exams);
    int k;
    int counter = 0; 

    for (int i =0; i<n_exams; i++){
        if (sol->timeslot_per_exams[i] == -1){
            unsched_exams.push_back(sol->all_exams[i]);
        }
    }   

    std::vector<int> possible_timeslots=std::vector<int>(totTimeslots);
    for (int i=0; i<totTimeslots;i++){  // possible timeslot rimane uguale per tutti, e il vettore [1, ntimeslot]
            possible_timeslots.push_back(i+1);
    }

    std::vector<int> not_available=std::vector<int>();
    not_available.reserve(totTimeslots);
    std::vector<int> possible_mutation=std::vector<int>();
    possible_mutation.reserve(totTimeslots);
    std::vector<size_t> order_not_schedule=std::vector<size_t>();
    order_not_schedule.reserve(n_exams);
    std::vector<int> not_available_time=std::vector<int>();
    not_available_time.reserve(totTimeslots);
    std::vector<int> available_time=std::vector<int>();
    available_time.reserve(totTimeslots);
    
    int pos;
    int kk;
    int rand_index;
    int pos_conf;
    Exam* exam;
    
    while (unsched_exams.size()!=0){
        if (fail>=max_fail){
            return false;
        }

        not_available.clear();
        possible_mutation.clear();
        not_available_time.clear();
        available_time.clear();
        order_not_schedule.clear();

        for(auto un_ex: unsched_exams){
            not_available_time=un_ex->conflict_times;
            std::sort(not_available_time.begin(), not_available_time.end());  
            std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_time.begin(),  
                    not_available_time.end(),inserter(available_time, available_time.begin()));
                    un_ex->available_times=available_time.size();
        }

        std::sort(unsched_exams.begin(), unsched_exams.end(), [](Exam* a, Exam* b) {
            if ( a->available_times!= b->available_times) return a->available_times < b->available_times;
                return a->num_conflict > b->num_conflict;
        });

        exam= unsched_exams[0];
        pos = exam->id_exam;
        
        // vado a vedere quali timeslot sono disponibili
        for (int j = 0; j <exam->conflict_times.size(); j++){
            if (exam->conflict_times[j] != -1){
                not_available.push_back(exam->conflict_times[j]);  // mi segno i timeslot non disponibili
            }    
        }
        std::sort(not_available.begin(), not_available.end());  
        std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available.begin(),  
                    not_available.end(),inserter(possible_mutation, possible_mutation.begin()));
        if (possible_mutation.size()> 0){
            rand_index = rand() % possible_mutation.size();
            sol->timeslot_per_exams[pos] = possible_mutation[rand_index];
            for (auto h : exam->conflict_exams){ 
                k=0; 
                while(sol->all_exams[h]->conflict_exams[k] != exam->id_exam){ 
                    k++;
                } 
                sol->all_exams[h]->conflict_times[k]=possible_mutation[rand_index];
            } 
            unsched_exams.erase(unsched_exams.begin());
        }else{
            fail++;
            for (int j = 0; j <exam->conflict_times.size(); j++){                
                if(exam->conflict_times[j]!=-1){                  
                    exam->conflict_times[j]=-1;
                    pos_conf=exam->conflict_exams[j];
                    unsched_exams.push_back(sol->all_exams[pos_conf]);
                    sol->timeslot_per_exams[pos_conf]=-1;
                    // dico a tutti gli esami conflittuali dell'esame unschedulato che l'ho unschedulato
                    for(auto conf_exam : sol->all_exams[pos_conf]->conflict_exams){
                        kk=0; 
                        while(sol->all_exams[conf_exam]->conflict_exams[kk] != pos_conf){ 
                            kk++;
                        } 
                        sol->all_exams[conf_exam]->conflict_times[kk]=-1;                        
                    }
                }
            }    
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool neighbours_by_swapping_single(Solution* solution, int ii, int jj, double obj_pre_swap){
    std::vector<int> exams = solution->timeslot_per_exams;
    
    int t1 = ii+1;
    int t2 = jj+1;
    
    for (int i = 0; i<exams.size(); i++){
        if (exams[i] == t1){
            exams[i]=t2;
        }else{
            if (exams[i] == t2){
                exams[i]=t1;
            }
        }
    }
    solution->update_timeslots();
    solution->update_weights();
    if(solution->objective_function()<obj_pre_swap){
        return true;
    }else{
        return false;
    }
}