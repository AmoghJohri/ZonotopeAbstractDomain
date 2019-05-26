#ifndef ZONOTOPE_H_
#define ZONOTOPE_H_

#include<iostream>
#include<armadillo>
#include<map>
#include<vector>
#include<string>
#include "float.h"
#include "assert.h"


int globalCounter = INT_MIN;

enum LatticeCompare
{
    UC = -1,
    LT = 0,
    GT = 1,
};

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
    std::pair<double,double> concreteValue; // stores the concrete value, does not do it automatically for variables
    int varPos; // int to set the position (on the column) in which the affine-variable is present in the affine-set
    stackValueFlag flag; // tells whether the stackValue is a TOP, BOT or neither of the two

    // literals don't get saved here
    std::vector<std::pair<std::string, double>> centralVector;
    std::vector<std::pair<std::string, double>> perturbedVector;
};

struct AbstractValue // defining an affine-set
{
    std::string affineSetName; // string to save the name of the affine-set 

    int n = 0; // number of central noise symbols in the affine-set + 1
    int m = 0; // number of perturbed noise symbols in the affine-set
    int p = 0; // number of variables in the affine-set


    abstractValueFlag flag; // for each affine-set tells whether the entire set is impossible or if it has no constaints

    arma::Mat<double> centralMatrix; // saves Cx - size = (n+1)X(p)
    arma::Mat<double> perturbedMatrix; // saves Px - size = (m)X(p) 
    std::vector<std::pair<double,double>> constraintOverCentralMatrix; // saves the constains over Cx
    std::vector<std::pair<double,double>> constraintOverPerturbedMatrix; // saves the constains over Px

    std::map<std::string, StackValue*> affineSet; // contains a map from the name of the variables to the memory location of variables

};


class Zonotope 
{

    private:
        StackValue topStackVal = {.varName = "TOP", .lv = VARIABLE, .concreteValue = std::make_pair(DBL_MIN, DBL_MAX), .varPos = -1, .flag = s_TOP, };
        StackValue botStackVal = {.varName = "BOT", .lv = VARIABLE, .concreteValue = std::make_pair(1.0,0.0), .varPos = -1, .flag = s_BOT, };

    public:

        Zonotope(); // constructor for the class

        StackValue topStackValue(); // returns top stack value
        StackValue botStackValue(); // returns bot stack value

        bool isTopStackValue(StackValue*); // given a pointer to a stack value tells if its top
        bool isBotStackValue(StackValue*); // given a pointer to a stack value tells if its bot
        void printStackValue(AbstractValue*, StackValue*); // pretty prints the stack value
        void printAbstractValue(AbstractValue*); // pretty prints the abstract value

        StackValue* getStackValueOfLiteral(std::string, double, AbstractValue*); // gets the stack value of a literal - MAKE USE OF APRON LIBRARY
        StackValue* getStackValueOfVariable(std::string, std::string, AbstractValue*); // gets the stack value of a variable - MAKE USE OF APRON LIBRARY

        AbstractValue* assignStackValue(std::string, std::string, StackValue*, AbstractValue*); // INCOMPLETE : uses APRON LIBRARY
        LatticeCompare compare(AbstractValue*, AbstractValue*);

        AbstractValue createAffineSet(std::string); // creates an empty affine-set with no variables
        AbstractValue* addCustomVariable(std::string, std::pair<double,double>, AbstractValue*);
        AbstractValue* addCustomVariable(StackValue*, AbstractValue*);

        // see variable + literal case
        StackValue* evaluateBinaryOperation(std::string, std::string, StackValue*, StackValue*, AbstractValue*); // MAKE APRON BASED MODIFICATIONS
};



#endif