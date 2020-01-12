#include "header.hpp"
#include <fstream>
#include <iostream>
#include <string>

int read_file_slo(std::string file_name)
{   
    int n_timeslot;    
    std::ifstream myfile;
    myfile.open(file_name);
    if(myfile.fail()){
        myfile.close();
        std::cerr << file_name << " file not found" << std::endl;
        exit(EXIT_FAILURE); 
    }
    myfile >> n_timeslot;
    myfile.close();
    return n_timeslot;
}