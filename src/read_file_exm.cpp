#include "header.hpp"
#include <fstream>
#include <iostream>
#include <string>


int read_file_exm(std::string file_name){
    int n_exams = 0;     
    std::string line;
    std::ifstream myfile (file_name);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            if (line.compare("")!=0){
                n_exams++;
            }
        }
        myfile.close();
    }
    return n_exams;
}