#ifndef Zonotope_DOMAIN_HH
#define Zonotope_DOMAIN_HH

#include <iostream>
#include <map>
#include <limits>
#include<armadillo>

#include "AbstractDomain.hh"

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

struct ZonotopeStackValue
{
    std::string varName = "unnamed"; // string to set the name of the stack-variable
    int varPos = -10; // int to set the position (on the column) in which the affine-variable is present in the affine-set
    stackValueFlag flag = s_TOP; // tells whether the stackValue is a TOP, BOT or neither of the two

    // literals don't get saved here
    std::map<std::string, double> centralVector; // all the non-zero central noise symbols of a variable
    std::map<std::string, double> perturbedVector; // all the non-zero perturbed noise symbols of a variable
};


struct ZonotopeAbstractValue
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

    std::map<std::string, ZonotopeStackValue*> affineSet; // contains a map from the name of the variables to the memory location of variables // literals are not stored in the affine set
  
};

class Zonotope : public AbstractDomain
{
public:
    Zonotope(clang::CompilerInstance* ci);
    
    Zonotope(c_safe::CAnalyzerManager* manager);

    AbstractValue* topValue() override;
    
    AbstractValue* botValue() override;
    
    void printAbstractValue(AbstractValue* abstract_value) override;
    
    AbstractValue* copyAbstractValue(AbstractValue* abs) override;
    
    AbstractValue* meet(AbstractValue* op1, AbstractValue* op2) override;
    
    AbstractValue* join(AbstractValue* op1, AbstractValue* op2) override;

    AbstractValue* widen(AbstractValue* op1, AbstractValue* op2) override;

    LatticeCompare compare(AbstractValue* op1, AbstractValue* op2) override;


    AbstractValue* assignStackValue(std::string variable_name, std::string variable_type, StackValue* rhs_stack_value, AbstractValue* current_abstract_value) override;

    StackValue* getStackValueOfLiteral(std::string type, double value, AbstractValue* current_abstract_value) override;

    StackValue* getStackValueOfVariable(std::string variable_name, std::string variabble_type, AbstractValue* current_abstract_value) override;

    StackValue* evaluateBinaryOperation(std::string opcode, std::string return_type, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value) override;

    StackValue* evaluateUnaryOperation(std::string opcode, std::string return_type, StackValue* stack_value, AbstractValue* current_abstract_value) override;

    StackValue* castStackValue(std::string src_type, std::string dest_type, StackValue* stack_value, AbstractValue* current_abstract_value) override;
    
    std::pair<AbstractValue*, AbstractValue*> assumeConstraint(std::string opcode, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value) override;


    void printStackValue(ZonotopeStackValue* stackValue);

    ZonotopeStackValue* copyStackValue(ZonotopeStackValue* s);
    bool isBotStackValue(ZonotopeStackValue*);
    bool isTopStackValue(ZonotopeStackValue*);
    std::pair<double, double> concretize(ZonotopeStackValue*, ZonotopeAbstractValue*);
    ZonotopeAbstractValue* addVariableToAffineSet(ZonotopeStackValue*, ZonotopeAbstractValue*);
    ZonotopeAbstractValue* removeStackValue(ZonotopeAbstractValue* , int);

    template <typename T1, typename T2>
    bool checkOverflow(std::string, std::pair<T1,T2>);
    
    template <typename T1, typename T2, typename T3, typename T4>
    bool intervalCompare(std::pair<T1,T2>, std::pair<T3,T4>);
    

};    


#endif // Zonotope_DOMAIN_HH
