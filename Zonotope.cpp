#include "Domains/Zonotope/Zonotope.hh"

Zonotope::Zonotope(clang::CompilerInstance* ci):AbstractDomain(ci)
{  
}

Zonotope::Zonotope(c_safe::CAnalyzerManager* manager):AbstractDomain(manager)
{  
    std::cout << "Zonotope Constructor" << std::endl;
}

bool Zonotope::isTopStackValue(ZonotopeStackValue* s) // checks for a pointer whether it points to a TOP stack value
{
    if(s->flag == s_TOP)
        return true;
    return false;
}

bool Zonotope::isBotStackValue(ZonotopeStackValue* s) // checks for a pointer whether it ponits to a BOT stack value
{
    if(s->flag == s_BOT)
        return true;
    return false;
}

AbstractValue* Zonotope::topValue() // returns a pointer to the top abstract value
{
    ZonotopeAbstractValue *top = new ZonotopeAbstractValue;
    top->flag = a_TOP;
    
    // Initialize the required  fields to make it an abstract value with no constraints.

    return top;
}

AbstractValue* Zonotope::botValue() // returns a pointer to the bot abstract value
{
    ZonotopeAbstractValue *bot = new ZonotopeAbstractValue;
    bot->flag = a_BOT;
    // Initialize the required  fields to make it an abstract value with unsatisfiable constraints.

    return bot;
}

void Zonotope::printAbstractValue(AbstractValue* abstract_value) // pretty prints the abstract value
{
    ZonotopeAbstractValue *currentAbstractValue = (ZonotopeAbstractValue*) abstract_value;
    // Pretty-print abs_value.
    std::cout << "///////////////////////////////////////////// REPORT ///////////////////////////////////////////////" << std::endl << std::endl;

    // cheks if the affine-set is a TOP of a BOT
    if(currentAbstractValue->flag == a_TOP)
    {
        std::cout << "TOP AFFINE SET" << std::endl;
        return ;
    }
    else if(currentAbstractValue->flag == a_BOT)
    {
        std::cout << "BOT AFFINE SET" << std::endl;
        return ;
    }

    // if the affine-set is neither a TOP or a BOT
    std::cout << "Central Matrix: (n x p)" << std::endl;
    std::cout << currentAbstractValue->centralMatrix << std::endl; // prints the central matrix
    std::cout << "Perturbed Matrix: (m x p)" << std::endl;
    std::cout << currentAbstractValue->perturbedMatrix << std::endl;// prints the perturbed matrix

    std::cout << "Interval Constraints: (n+m-1)" << std::endl;
    std::vector<std::pair<double,double>>::iterator itr; // iterating over the vector containing interval constrains
    for(itr = currentAbstractValue->constraintOverCentralMatrix.begin(); itr != currentAbstractValue->constraintOverCentralMatrix.end(); ++itr) // printing constaints on central noise symbols
    {
        std::cout << "[" << itr->first << "," << itr->second << "]";
        if(itr != currentAbstractValue->constraintOverCentralMatrix.end() - 1)
            std::cout<<" X ";
    }
    std::cout<<std::endl;
    for(itr = currentAbstractValue->constraintOverPerturbedMatrix.begin(); itr != currentAbstractValue->constraintOverPerturbedMatrix.end(); ++itr) // printing constaints on perturbed noise symbols
    {
        std::cout << "[" << itr->first << "," << itr->second << "]";
        if(itr!=currentAbstractValue->constraintOverPerturbedMatrix.end() - 1)
            std::cout<<" X ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Number of Variables are : " << currentAbstractValue->p << std::endl; // prints the number of variables
    std::cout << "Number of Central Noise Symbols are : " << currentAbstractValue->n - 1 << std::endl; // prints the number of central noise symbols
    std::cout << "Number of Perturbed Noise Symbols are : " << currentAbstractValue->m << std::endl; // prints the number of perturbed noise symbols

    // prints all the stackvariables in the affine-set in their affine forms
    std::map<std::string, ZonotopeStackValue*>::iterator itr2; // sorting the vectors in the stack value
    for(itr2 = (currentAbstractValue->affineSet).begin(); itr2 != (currentAbstractValue->affineSet).end(); ++itr2)
    {
        std::cout << itr2->first << " = ";
        this->printStackValue((itr2->second));
        std::cout << "Concretized Value = [" << concretize(itr2->second, currentAbstractValue).first << " , " << concretize(itr2->second, currentAbstractValue).second << "] ";
        std::cout << std::endl << std::endl;
    }

    std::cout << "//////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;

}

void Zonotope::printStackValue(ZonotopeStackValue* stackValue) // pretty-prints a stack value into its affine form
{
    if(stackValue->flag == s_TOP) // prints TOP if the stack value corresponds to a TOP stack value
        std::cout << "TOP" << std::endl;
    else if(stackValue->flag == s_BOT) // rints BOT if the stack value corresponds to a BOT stack value
        std::cout << "BOT" << std::endl;
    else // if the stack value is niether a TOP or a BOT
    {
        // printing directly through vectors
        if(stackValue->centralVector.find("0") != stackValue->centralVector.end())
            stackValue->centralVector["0"]; // printing the central stackvalue
        std::map<std::string, double>::iterator itr; // printing the central noise terms
        for(itr = stackValue->centralVector.begin(); itr!= stackValue->centralVector.end(); ++itr)
            std::cout << " + " << itr->second << "e" << itr->first;
        for(itr = stackValue->perturbedVector.begin(); itr!= stackValue->perturbedVector.end(); ++itr)// printing the perturved noise terms
            std::cout << " + " << itr->second << "n" << itr->first;
        std::cout << std::endl;
    }
}
ZonotopeStackValue* Zonotope::copyStackValue(ZonotopeStackValue* s) // takes a stack value, copies it and returns a pointer to the copy
{
    // copies s into o

    ZonotopeStackValue* o = new ZonotopeStackValue;
    o->varName = s->varName;
    o->varPos = s->varPos;
    o->flag = s->flag;

    std::map<std::string, double>::iterator itr;
    for(itr = s->centralVector.begin(); itr != s->centralVector.end(); ++itr)
        o->centralVector.insert(*itr);
    for(itr = s->perturbedVector.begin(); itr != s->perturbedVector.end(); ++itr)
        o->perturbedVector.insert(*itr);
    return o;
}


AbstractValue* Zonotope::copyAbstractValue(AbstractValue* abstract_value) // makes a copy of the abstract value and returns a pointer to the copy
{
    ZonotopeAbstractValue *a = (ZonotopeAbstractValue*) abstract_value;
    ZonotopeAbstractValue *x = new ZonotopeAbstractValue;

    // DeepCopy a into xsss
    x->affineSetName = a->affineSetName;
    x->n = a->n;
    x->m = a->m;
    x->p = a->p;
    x->flag = a->flag;
    std::vector<std::pair<double,double>>::iterator itr;
    for(itr = a->constraintOverCentralMatrix.begin(); itr != a->constraintOverCentralMatrix.end(); ++itr)
        x->constraintOverCentralMatrix.push_back(*itr);
    for(itr = a->constraintOverPerturbedMatrix.begin(); itr != a->constraintOverPerturbedMatrix.end(); ++itr)
        x->constraintOverPerturbedMatrix.push_back(*itr);
    x->centralMatrix = a->centralMatrix;
    x->perturbedMatrix = a->perturbedMatrix;
    // std::map<std::string, ZonotopeStackValue*>::iterator itr2;
    for(auto itr2 = a->affineSet.begin(); itr2 != a->affineSet.end(); ++itr2)
    {
        x->affineSet.insert(std::make_pair(itr2->first, copyStackValue(itr2->second)));
    }
    return x;
}

AbstractValue* Zonotope::meet(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    // Do meet operation on op1 and op2 to get result.

    return result;
}

AbstractValue* Zonotope::join(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    // Do join operation on op1 and op2 to get result.

    return result;
}

AbstractValue* Zonotope::widen(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    // Do widen operation on op1 and op2 to get result.

    return result;
}


LatticeCompare Zonotope::compare(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;

    // Compare op1 and op2 and return the result.
    // if op1 == op2, return EQ
    // if op1 > op2, return GT
    // if op1 < op2, return LT
    // if op1 and op2 are uncomparable, return UC

    return UC;
}

AbstractValue* Zonotope::assignStackValue(std::string variable_name, std::string variable_type, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *rhs = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);

    rhs->varName = variable_name;

    // Add the constraint variable_name = rhs, to abstract_value.
    if(isTopStackValue(rhs)) //    printStackValue(rhs); if the stack value is a top, it removes it from the affine-set (this is the same as the stack value having no constraints over it)
    {
        abstract_value->affineSet.erase(variable_name);
        return abstract_value;
    }
    else if(isBotStackValue(rhs)) // if the stack value if BOT, it returns a BOT abstract value
    {
        delete abstract_value;
        return botValue();
    }
    // otherwise, adds the stack value into the abstract value
    abstract_value = addVariableToAffineSet(rhs,abstract_value); // adding to the number of variables
    abstract_value->affineSet[variable_name] = rhs;
    abstract_value->flag = a_NONE;
    return abstract_value;
}

StackValue* Zonotope::getStackValueOfLiteral(std::string type, double value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // result = stackvalue of the value
        // the name of the literal is stored as string denoting its position in the matrix

    // makes a new stack value for the appropriate literal and outputs the same
    if(strcmp(type.c_str(), "int") == 0) // if the output type is a string
    {
        ZonotopeStackValue *output = new ZonotopeStackValue;
        output->centralVector["0"] = (int)value;
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else if(strcmp(type.c_str(), "real") == 0) // if the output type is a double
    {
        ZonotopeStackValue *output = new ZonotopeStackValue;
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else // if the output type does not match
    {
        std::cout << "Unknown Type" << type << std::endl;
        assert(false);
    }

    return result;
}



StackValue* Zonotope::getStackValueOfVariable(std::string variableName, std::string variable_type, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *s = new ZonotopeStackValue;
    ZonotopeAbstractValue *currentAbstractValue = (ZonotopeAbstractValue*) current_abstract_value;

    // result = stackvalue of the variable.

    // looks in the affine_set for the required variable
    // if the variable is not present, returns TOP
    auto itr = currentAbstractValue->affineSet.find(variableName);
    if(itr == currentAbstractValue->affineSet.end())
    {
        s->flag = s_TOP;
        return s;
    }
    else
    {
        s->varPos = itr->second->varPos;
        s->flag = itr->second->flag;
        s->centralVector = itr->second->centralVector;
        s->perturbedVector = itr->second->perturbedVector;
        return s;
    }

}

StackValue* Zonotope::evaluateBinaryOperation(std::string opcode, std::string return_type, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *op1 = (ZonotopeStackValue*) lhs_stack_value;
    ZonotopeStackValue *op2 = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // Do binary operation on op1 and op2 to get result
    // Includes all binary arithematic operations.

    return result;
}

StackValue* Zonotope::evaluateUnaryOperation(std::string opcode, std::string return_type, StackValue* stack_value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *operand = (ZonotopeStackValue*) stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // Do unary operation on operand to get resultstd::pair<AbstractValue*, AbstractValue*> assumeConstraint(std::string opcode, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
    // Includes all unary arithematic operations.

    return result;
}

StackValue* Zonotope::castStackValue(std::string src_type, std::string dest_type, StackValue* stack_value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *operand = (ZonotopeStackValue*) stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // Do cast operation on operand to get result
    // Includes all unary arithematic operations.
    if(src_type == dest_type)
    {
        result = copyStackValue(operand);
    }
    return result;
}

std::pair<AbstractValue*, AbstractValue*> Zonotope::assumeConstraint(std::string opcode, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
{
    ZonotopeStackValue *lhs = (ZonotopeStackValue*) lhs_stack_value;
    ZonotopeStackValue *rhs = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeAbstractValue *result_true = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);
    ZonotopeAbstractValue *result_false = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);

    // Add the constraint lhs opcode rhs to result_true
    // Add the constraint !(lhs opcode rhs) to result_false
    
    return std::make_pair<AbstractValue*, AbstractValue*>(result_true, result_false);
}

std::pair<double, double> Zonotope::concretize(ZonotopeStackValue* s, ZonotopeAbstractValue* a) // returns the concretized value of the stack value
{
    double ldev = 0;
    double rdev = 0;

    std::map<std::string,double>::iterator itr;

    for(itr = s->centralVector.begin(); itr != s->centralVector.end(); ++itr)
    {
        if(itr == s->centralVector.begin())
            continue;
        
        if(itr->second > 0)
        {
            ldev = ldev + itr->second*a->constraintOverCentralMatrix[std::stoi(itr->first) - 1].first;
            rdev = rdev + itr->second*a->constraintOverCentralMatrix[std::stoi(itr->first) - 1].second;
        }
        else 
        {
            ldev = ldev + itr->second*a->constraintOverCentralMatrix[std::stoi(itr->first) - 1].second;
            rdev = rdev + itr->second*a->constraintOverCentralMatrix[std::stoi(itr->first) - 1].first; 
        }
    }

    for(itr = s->perturbedVector.begin(); itr != s->perturbedVector.end(); ++itr)
    {
        if(itr->second > 0)
        {
            ldev = ldev + itr->second*a->constraintOverPerturbedMatrix[std::stoi(itr->first) - 1].first;
            rdev = rdev + itr->second*a->constraintOverPerturbedMatrix[std::stoi(itr->first) - 1].second;
        }
        else 
        {
            ldev = ldev + itr->second*a->constraintOverPerturbedMatrix[std::stoi(itr->first) - 1].second;
            rdev = rdev + itr->second*a->constraintOverPerturbedMatrix[std::stoi(itr->first) - 1].first; 
        }
    }
    double x;
    if(s->centralVector.find("0") != s->centralVector.end())
        x = s->centralVector["0"];
    else
        x = 0;
    return std::make_pair(x + ldev, x + rdev);
}

ZonotopeAbstractValue* Zonotope::addVariableToAffineSet(ZonotopeStackValue* stValue, ZonotopeAbstractValue* abValue) // takes a pointer to a stack value and abstract value and adds the stack value to the affine set
{
    // if stack value is neither a top or a bot

    // augmenting cols in the central and perturbed matrices
    if(abValue->affineSet.find(stValue->varName) == abValue->affineSet.end()) // if the variable is not already present
    {
        abValue->centralMatrix.insert_cols(abValue->p,1);
        abValue->perturbedMatrix.insert_cols(abValue->p,1);
        stValue->varPos = abValue->p;
    }
    else
    {
        stValue->varPos = abValue->affineSet[stValue->varName]->varPos;
    }
    
    
    // setting the central matrix
    // obtaining the highest degree of central noise symbol
    int max1 = 0;
    if(stValue->centralVector.rbegin() != stValue->centralVector.rend())
    {
        if(std::stoi(stValue->centralVector.rbegin()->first) > max1)
            max1 = std::stoi(stValue->centralVector.rbegin()->first);
    }
    // if there are noise symbols and the central matrix is not empty and noise symbols need to be added
    if(max1 > 0 && abValue->n != 0 && (max1 - abValue->n > 0))
        abValue->centralMatrix.insert_rows(abValue->n, max1 - abValue->n + 1);

    // if there are noise symbols and the central matrix is empty
    else if(max1 > 0 && abValue->n == 0 && (max1 - abValue->n > 0))
        abValue->centralMatrix.insert_rows(0, max1 - abValue->n + 1);

    else if((max1 == 0) && (abValue->n == 0)) // adding a literal variable
        abValue->centralMatrix.insert_rows(0, 1);

    // adding terms in the central matrix;
    for(auto itr = stValue->centralVector.begin(); itr != stValue->centralVector.end(); ++itr)
    {
        abValue->centralMatrix(std::stoi(itr->first),stValue->varPos) = itr->second;      
    }
    // augmenting the vector of constraints
    int c = abValue->constraintOverCentralMatrix.size();
    if(abValue->centralMatrix.n_rows > abValue->n)
    {
        abValue->n = abValue->centralMatrix.n_rows;
        for(int i = 0; i <= (c - abValue->n); i++)
        {
            abValue->constraintOverCentralMatrix.push_back(std::make_pair(-1,1)); 
        }
    }
    // setting the perturbed matrix
    // obtaining the highest degree of perturbation noise symbol
    max1 = 0;
    // finding the max perturbed noise symbol
    if(stValue->perturbedVector.rbegin() != stValue->perturbedVector.rend())
    {
        if(std::stoi(stValue->perturbedVector.rbegin()->first) > max1)
            max1 = std::stoi(stValue->perturbedVector.rbegin()->first);
    }


    if(max1 > 0 && abValue->m != 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(abValue->m, max1 - abValue->m);

    else if(max1 > 0 && abValue->m == 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(0, max1 - abValue->m);

    for(auto itr = stValue->perturbedVector.begin(); itr != stValue->perturbedVector.end(); ++itr)
    {
        abValue->perturbedMatrix(std::stoi(itr->first)-1,stValue->varPos) = itr->second;
    }
    // augmenting the vector of constraints
    if(abValue->perturbedMatrix.n_rows > abValue->m)
    {
        abValue->m = abValue->perturbedMatrix.n_rows;
        for(int i = 0; i <= (abValue->constraintOverPerturbedMatrix.size() - abValue->m); i++)
            abValue->constraintOverPerturbedMatrix.push_back(std::make_pair(-1,1));
    }

    abValue->p = abValue->p + 1;
    abValue->flag = a_NONE;
    return abValue;
    // Included the perturbed form as well
}
