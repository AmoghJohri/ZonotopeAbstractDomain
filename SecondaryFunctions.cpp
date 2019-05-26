// SECONDARY FUNCTIONS

#include<iostream>
#include<vector>
#include<iterator>
#include "Zonotope.hh"

bool vecCompare1(const std::pair<double, double>&i, const std::pair<double, double>&j)
{
    return (i.first) < (j.first);
}


bool vecCompare2(const std::pair<std::string, double>&i, const std::pair<std::string, double>&j)
{
    return (i.first) < (j.first);
}


void printVectorOfStringAndDouble(std::vector<std::pair<std::string, double>> v)
{
    std::vector<std::pair<std::string, double>>::iterator itr;
    for(itr = v.begin(); itr != v.end(); ++itr)
        std::cout << itr->first << " = " << itr->second << " ";
    std::cout << std::endl;
}

void printVectorOfDoubleAndDouble(std::vector<std::pair<double, double>> v)
{
    std::vector<std::pair<double, double>>::iterator itr;
    for(itr = v.begin(); itr != v.end(); ++itr)
        std::cout << itr->first << "-" << itr->second << " ";
    std::cout << std::endl;
}

void printAffineSet(std::map<std::string, StackValue*> v)
{
    std::map<std::string, StackValue*>::iterator itr;
    for(itr = v.begin(); itr != v.end(); ++itr)
        std::cout << itr->first << "-" << itr->second << " ";
    std::cout << std::endl;
}

double argmin(double a, double b)
{
    double x = std::min(a,b);
    double y = std::max(a,b);

    if((x < 0) && (y > 0))
        return 0;
    if((x < 0) && (y < 0))
        return abs(y);
    
    return abs(x);

}

std::pair<double,double> intervalJoin(std::pair<double,double> p1, std::pair<double,double> p2)
{
    return std::make_pair(std::min(p1.first, p2.first), std::max(p1.second, p2.second));
}