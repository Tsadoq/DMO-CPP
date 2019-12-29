#include "read_file_slo.hpp"
#include <fstream>
#include <iostream>
#include <string>

int read_file_slo(std::string file_name)
{   
    int n_timeslot;    
    std::ifstream myfile (file_name);
        myfile >> n_timeslot;
    return n_timeslot;
}



