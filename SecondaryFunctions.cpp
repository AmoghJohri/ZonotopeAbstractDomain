// SECONDARY FUNCTIONS

#include<iostream>
#include<vector>
#include<iterator>
#include "Zonotope.hh"


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

std::pair<double, double> intervalMeet(std::pair<double,double> p1, std::pair<double,double> p2)
{
    if(p1.second < p2.first || p2.second < p1.first)
        return std::make_pair(1,0);
    return std::make_pair(std::max(p1.first,p2.first), std::min(p1.second,p2.second));
}  

bool intervalCompare(std::pair<double,double> p1, std::pair<double,double> p2)
{
    if(p2.second >= p1.second && p2.first <= p1.first)
        return true;
    return false;
}