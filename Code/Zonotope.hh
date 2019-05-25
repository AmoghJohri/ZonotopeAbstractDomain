#ifndef ZONOTOPE_H_
#define ZONOTOPE_H_

#include<iostream>
#include<armadillo>
#include<map>
#include<vector>
#include <ap_global0.h>
#include <ap_global1.h>
#include "float.h"
#include "assert.h"


enum stackValueFlag // for each stack value tells whether the affine-form corresponds to a TOP or BOT
{
    s_BOT = -1,
    s_TOP = 1,
    s_NONE = 0,
};

enum abstractValueFlag // for each affine-set tells whether the entire set is impossible or if it has no constaints
{
    a_BOT = -1,
    a_TOP = 1,
    a_NONE = 0,
};

enum litOrVar
{
    LITERAL = 0,
    VARIABLE = 1,
};

struct StackValue
{
    std::string varName; // string to set the name of the stack-variable
    litOrVar lv; // tells whether the value corresponds to a literal or a variable
    std::pair<double,double> concreteValue; // stores the concrete value
    int varPos; // int to set the position (on the column) in which the affine-variable is present in the affine-set
    stackValueFlag flag; // tells whether the stackValue is a TOP, BOT or neither of the two
};

struct AbstractValue // defining an affine-set
{
    std::string affineSetName; // string to save the name of the affine-set 

    int n; // number of central noise symbols in the affine-set
    int m; // number of perturbed noise symbols in the affine-set
    int p; // number of variables in the affine-set


    abstractValueFlag flag; // for each affine-set tells whether the entire set is impossible or if it has no constaints

    arma::Mat<double> centralMatrix; // saves Cx - size = (n+1)X(p)
    arma::Mat<double> perturbedMatrix; // saves Px - size = (m)X(p) 
    std::vector<std::pair<double,double>> constraintOverCentralMatrix; // saves the constains over Cx
    std::vector<std::pair<double,double>> constaintOverPerturbedMatrix; // saves the constains over Px

    std::map<std::string, StackValue*> affineSet; // contains a map from the name of the variables to the memory location of variables

};


class Zonotope 
{
    private:
        StackValue topStackVal = {.varName = "TOP", .lv = VARIABLE, .concreteValue = std::make_pair(DBL_MIN, DBL_MAX), .varPos = -1, .flag = s_TOP, };
        StackValue botStackVal = {.varName = "BOT", .lv = VARIABLE, .concreteValue = std::make_pair(1.0,0.0), .varPos = -1, .flag = s_BOT, };

    public:
        StackValue topStackValue(); // returns top stack value
        StackValue botStackValue(); // returns bot stack value

        bool isTopStackValue(StackValue*); // given a pointer to a stack value tells if its top
        bool isBotStackValue(StackValue*); // given a pointer to a stack value tells if its bot
        void printStackValue(AbstractValue*, StackValue*); // pretty prints the stack value

        StackValue* getStackValueOfLiteral(std::string, double, AbstractValue*);

};



#endif