#include <string>
#include <iostream>
#include <vector>
#include "header.hpp"
#include "Exam.hpp"
#include "Solution.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)
    return p;
}

double cooling(double time_limit, double current_time,double t0)
{
    double coeff=(time_limit-current_time)/time_limit;
    return  t0*coeff;
}