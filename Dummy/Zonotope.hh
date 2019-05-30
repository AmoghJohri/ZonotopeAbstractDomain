#ifndef ZONOTOPE_H_
#define ZONOTOPE_H_

#include<iostream>
#include<armadillo>
#include<map>
#include<vector>
#include<string>
#include "float.h"
#include "assert.h"

int globalCounter = INT_MIN; // in order to set the names for variables created as strings of integers;

enum LatticeCompare // used in the compare operation, tells whether an affine-set is less than, greater than or equal to another affine-sets
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
    a_BOT = -1, // when any stackvalue in the affine-set takes on invalid values
    a_TOP = 1, // when the affine_set is empty
    a_NONE = 0,
};

enum litOrVar // tells whether a variable is a LITERAL or a VARIABLE
{
    LITERAL = 0,
    VARIABLE = 1,
};

struct StackValue
{
    std::string varName = "unnamed"; // string to set the name of the stack-variable
    litOrVar lv = VARIABLE; // tells whether the value corresponds to a literal or a variable
    double litValue = 0; // in case of StackValue being a literal, the value is stored here
    int varPos = -10; // int to set the position (on the column) in which the affine-variable is present in the affine-set
    stackValueFlag flag = s_TOP; // tells whether the stackValue is a TOP, BOT or neither of the two

    // literals don't get saved here
    std::map<std::string, double> centralVector; // all the non-zero central noise symbols of a variable
    std::map<std::string, double> perturbedVector; // all the non-zero perturbed noise symbols of a variable

};

struct AbstractValue // defining an affine-set
{
    std::string affineSetName = "unnamed";// string to save the name of the affine-set 

    int n = 0; // number of central noise symbols in the affine-set + 1
    int m = 0; // number of perturbed noise symbols in the affine-set
    int p = 0; // number of variables in the affine-set


    abstractValueFlag flag = a_TOP; // for each affine-set tells whether the entire set is impossible or if it has no constaints

    arma::Mat<double> centralMatrix; // saves Cx - size = (n+1)X(p)
    arma::Mat<double> perturbedMatrix; // saves Px - size = (m)X(p) 
    std::vector<std::pair<double,double>> constraintOverCentralMatrix; // saves the constains over Cx
    std::vector<std::pair<double,double>> constraintOverPerturbedMatrix; // saves the constains over Px

    std::map<std::string, StackValue*> affineSet; // contains a map from the name of the variables to the memory location of variables // literals are not stored in the affine set

};


class Zonotope 
{

    public:

        Zonotope(); // constructor for the class

        StackValue* topStackValue(); // returns pointer to top stack value
        StackValue* botStackValue(); // returns pointer to bot stack value

        bool isTopStackValue(StackValue*); // given a pointer to a stack value tells if its top
        bool isBotStackValue(StackValue*); // given a pointer to a stack value tells if its bot
        
        // PRINT FUNCTIONS
        void printStackValue(std::string, AbstractValue*); // pretty prints the stack value based on its location in the affine set
        void printStackValue(StackValue*); // pretty prints the stack value 
        void printAbstractValue(AbstractValue*); // pretty prints the abstract value

        // COPY Functions
        StackValue* copyStackValue(StackValue*); // copies the stack value into another stack value and returns it's pointer
        AbstractValue* copyAbstractValue(AbstractValue*); // copies the abstract value into another abstract value and returns it's pointer

        StackValue* getStackValueOfLiteral(std::string, double, AbstractValue*); // gets the pointer to a stack-value with the literal - MAKE USE OF APRON LIBRARY
        StackValue* getStackValueOfVariable(std::string, std::string, AbstractValue*); // gets the stack value of a variable in the affine-set, copies it into another StackValue and returns the later's pointer- MAKE USE OF APRON LIBRARY
        AbstractValue* assignStackValue(std::string, std::string, StackValue*, AbstractValue*); // INCOMPLETE : uses APRON LIBRARY

        LatticeCompare compare(AbstractValue*, AbstractValue*); // takes two affine-set and returns the possible comparisions between them
        AbstractValue* join(AbstractValue*, AbstractValue*); // takes two affine-set, joins them and returns the pointer to the joined affine set
        AbstractValue* meet(AbstractValue*, AbstractValue*); // STILL HAVE TO IMLPEMENT
        
        AbstractValue createAffineSet(std::string); // creates an empty affine-set with no variables
        AbstractValue* addCustomVariable(std::string, std::pair<double,double>, AbstractValue*); // takes the variable name and its interval and a pointer to an affine set to add the variable in the affine set
        
        StackValue* getStackValue(AbstractValue*, int); // gets the stack value from an affine set the the required position
        AbstractValue* removeStackValue(AbstractValue*, int); // removes a stack value from the affine-set, required in order to carry out matrix based operations
        AbstractValue* addCustomVariable(StackValue*, AbstractValue*); // adds a stack_value to the abstract set
        void fillAffineSet(AbstractValue*); // fills the mapping of affine set with the matrix values

        // HAVE TO IMPLEMENT THIS
        
        
        StackValue* evaluateBinaryOperation(std::string, std::string, StackValue*, StackValue*, AbstractValue*); // MAKE APRON BASED MODIFICATIONS
        StackValue* evaluateUnaryOperation(std::string, std::string, StackValue*, AbstractValue*); // MAKE APRON BASED MODIFICATIONS

        // secondary functions
        std::pair<double,double> concretize(StackValue*, AbstractValue*); // takes a pointer to the stack value of an affine set and returns it's concretization
        std::pair<double,double> concretize(int, AbstractValue*); // takes the position of a stack value in the abstract matrix and returns its conceretization

};
#endif