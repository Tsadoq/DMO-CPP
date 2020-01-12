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
        sol->update_single_exam(positions[j],-1);
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool rescheduling(Solution* sol){
    
    int totTimeslots=sol->n_timeslot;
    std::vector<int> possible_timeslots=sol->possible_timeslot;
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
            //std::cout<<un_ex->id_exam<<" "<<un_ex->conflict_times.size()<<std::endl;
            //std::cout<<un_ex->conflict_times.size()<<std::endl;
            not_available_time=un_ex->conflict_times;
            std::sort(not_available_time.begin(), not_available_time.end());  
            std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_time.begin(),  
                    not_available_time.end(),std::inserter(available_time, available_time.begin()));
            
            un_ex->available_times=available_time.size();
            available_time.clear();
        }

        std::sort(unsched_exams.begin(), unsched_exams.end(), [](Exam* a, Exam* b) {
            if ( a->available_times!= b->available_times) return a->available_times < b->available_times;
                return a->num_conflict > b->num_conflict;});

        exam = unsched_exams[0];
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
            sol->update_single_exam(pos,possible_mutation[rand_index]);
            unsched_exams.erase(unsched_exams.begin());
        }else{
            fail++;
            for (int j = 0; j <exam->conflict_times.size(); j++){                
                if(exam->conflict_times[j]!=-1){                  
                    exam->conflict_times[j]=-1;
                    pos_conf=exam->conflict_exams[j];
                    unsched_exams.push_back(sol->all_exams[pos_conf]);
                    sol->update_single_exam(pos_conf,-1);
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
    
    for (int i = 0; i<solution->n_exams; i++){
        if (exams[i] == t1){
            solution->update_single_exam(i,t2);
        }else{
            if (exams[i] == t2){
                solution->update_single_exam(i,t1);
            }
        }
    }
    solution->update_weights();
    if(solution->objective_function()<obj_pre_swap){
        return true;
    }else{
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void neighbours_by_mutation(Solution* solution, std::vector<size_t> order_for_mutation, int num_mutation, double perc){ 
                                          
    Exam* exam_mutate; 
    int is_void=0; 
    int totTimeslot = solution->n_timeslot;
    int n_exams=solution->n_exams;
    std::vector<int> not_available_timeslots = std::vector<int>();
    not_available_timeslots.reserve(totTimeslot);
    std::vector<int> available_timeslots=std::vector<int>();
    available_timeslots.reserve(totTimeslot);
    std::vector<int> possible_timeslot=solution->possible_timeslot;
    int k; 
    int new_timeslot; 
    int randomIndex; 
    int size_random=(int) (perc*n_exams); 
    std::vector<int> indexes= std::vector<int>(size_random); 
    std::iota(indexes.begin(), indexes.end(), 0); 
    // obtain a time-based seed:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
    std::shuffle(indexes.begin(), indexes.end(),std::default_random_engine(seed) ); 
             
    for(int i=0;i<num_mutation+is_void && i<size_random;i++){ 
        // exam I'm trying to mutate 
        available_timeslots.clear(); 
        exam_mutate=solution->all_exams[order_for_mutation[indexes[i]]];
        not_available_timeslots=exam_mutate->conflict_times; 
        not_available_timeslots.push_back(solution->timeslot_per_exams[order_for_mutation[indexes[i]]]); 
        // sort vector because set_difference works with sorted arrays 
        sort(not_available_timeslots.begin(), not_available_timeslots.end()); 
        // find available timeslot 
        set_difference(possible_timeslot.begin(), possible_timeslot.end(), not_available_timeslots.begin(),  
                        not_available_timeslots.end(),inserter(available_timeslots, available_timeslots.begin())); 
        
        if(available_timeslots.size()==0){ 
            is_void++; 
        }else{ 
            randomIndex = rand() % available_timeslots.size(); 
            new_timeslot=available_timeslots[randomIndex]; 
            // update timeslot in all exam in conflict with respect to exam_mutate
            solution->update_single_exam(exam_mutate->id_exam, new_timeslot);             
        } 
    } 
}

/////////////////////////////////////////////////////////////////////////////////////////////

void neighbours_by_swapping(Solution* solution){
    std::vector<int> exams = solution->timeslot_per_exams;
    int totTimeslots = solution->n_timeslot;
    int n_exams=solution->n_exams;
    // seleziono due diversi timeslot a caso
    int t1 = 1 + rand() % totTimeslots;
    int t2 = 1 + rand() % totTimeslots;
    while(t2==t1 && abs(t2-t1)>5) 
        t2 = 1 + rand() % totTimeslots;
    // vettori di indici di esami che stanno nello stesso timeslot rispettivamente in t1 e in t2
    std::vector<int> e1;
    e1.reserve(n_exams);
    std::vector<int> e2;
    e2.reserve(n_exams);  // preallocazione (cicciona, ma non ci sono altri upperbounds)
    for (int i = 0; i<n_exams; i++){
        if (exams[i] == t1){
            e1.push_back(i);
        }
        if (exams[i] == t2){
            e2.push_back(i);
        }
    }
    // cerco gli esami in e2 che sono in conflitto con gli esami in t1
    std::vector<int> toSwappedIn1;
    toSwappedIn1.reserve(e2.size());
    // cerco gli esami in e1 che sono in conflitto con gli esami in t2
    std::vector<int> toSwappedIn2 ;
    toSwappedIn2.reserve(e1.size());
    for (int i = 0; i< e1.size(); i++){
        // cerco un esame conflittuale piazzato in t2
        if ( std::find(solution->all_exams[e1[i]]->conflict_times.begin(), 
                solution->all_exams[e1[i]]->conflict_times.end(), t2) != solution->all_exams[e1[i]]->conflict_times.end()){
            // ho trovato un esame conflittuale in t2, quindi l'esame con indice e1[i] va inserito
            toSwappedIn2.push_back(e1[i]);
        }
    }

    for (int i = 0; i< e2.size(); i++){
        if (std::find(solution->all_exams[e2[i]]->conflict_times.begin(), 
                solution->all_exams[e2[i]]->conflict_times.end(), t1) != solution->all_exams[e2[i]]->conflict_times.end()){
            // ho trovato un esame conflittuale in t1, quindi l'esame con indice e2[i] va inserito
            toSwappedIn1.push_back(e2[i]);
        }
    }
    // TIMESLOT UPDATE
     for (int i = 0; i < toSwappedIn1.size(); i++){
        solution->update_single_exam(toSwappedIn1[i], t1);
        
    }
    for (int i = 0; i < toSwappedIn2.size(); i++){
        solution->update_single_exam(toSwappedIn2[i], t2);
    }    
    return;

}


    