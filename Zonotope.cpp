#include "Domains/Zonotope/Zonotope.hh"

Zonotope::Zonotope(clang::CompilerInstance* ci):AbstractDomain(ci)
{  
}

Zonotope::Zonotope(c_safe::CAnalyzerManager* manager):AbstractDomain(manager)
{  
    std::cout << "Zonotope Constructor" << std::endl;
}

ZonotopeStackValue* Zonotope::topStackValue() // returns pointer to a TOP stackvalue
{
    std::cout << "I am in Zonotope::topStackValue" << std::endl;
    ZonotopeStackValue* n = new ZonotopeStackValue;
    n->flag = s_TOP;
    return n;
}

ZonotopeStackValue* Zonotope::botStackValue() // returns a pointer to a BOT stackValue
{
    std::cout << "I am in Zonotope::botStackValue" << std::endl;
    ZonotopeStackValue* n = new ZonotopeStackValue;
    n->flag = s_BOT;
    return n;
}

bool Zonotope::isTopStackValue(ZonotopeStackValue* s) // checks for a pointer whether it points to a TOP stack value
{
    std::cout << "I am in Zonotope::isTopStackValue" << std::endl;
    if(s->flag == s_TOP)
        return true;
    return false;
}

bool Zonotope::isBotStackValue(ZonotopeStackValue* s) // checks for a pointer whether it ponits to a BOT stack value
{
    std::cout << "I am in Zonotope::isBotStackValue" << std::endl;
    if(s->flag == s_BOT)
        return true;
    return false;
}

AbstractValue* Zonotope::topValue() // returns a pointer to the top abstract value
{
    std::cout << "I am in Zonotope::topValue" << std::endl;
    ZonotopeAbstractValue *top = new ZonotopeAbstractValue;
    top->flag = a_TOP;
    
    // Initialize the required  fields to make it an abstract value with no constraints.

    return top;
}

AbstractValue* Zonotope::botValue() // returns a pointer to the bot abstract value
{
    std::cout << "I am in Zonotope::botValue" << std::endl;
    ZonotopeAbstractValue *bot = new ZonotopeAbstractValue;
    bot->flag = a_BOT;
    // Initialize the required  fields to make it an abstract value with unsatisfiable constraints.

    return bot;
}

void Zonotope::printAbstractValue(AbstractValue* abstract_value) // pretty prints the abstract value
{
    std::cout << "I am in Zonotope::printAbstractValue" << std::endl;
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

    std::cout << "Interval Constraints: (n - 1 + m)" << std::endl;
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
        //std::cout << itr2->first << " = ";
        this->printStackValue((itr2->second));
        std::pair<double,double> p = concretize(itr2->second, currentAbstractValue); // storing the concretized value
        std::cout << "Concretized Value = [" << p.first << " , " << p.second << "] ";
        std::cout << std::endl << std::endl;
    }
    std::cout << "////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;

}

void Zonotope::printStackValue(ZonotopeStackValue* stackValue) // pretty-prints a stack value into its affine form
{
    std::cout << "I am in Zonotope::printStackValue" << std::endl;

    if(stackValue->flag == s_TOP) // prints TOP if the stack value corresponds to a TOP stack value
        std::cout << "TOP" << std::endl;
    else if(stackValue->flag == s_BOT) // rints BOT if the stack value corresponds to a BOT stack value
        std::cout << "BOT" << std::endl;
    else // if the stack value is niether a TOP or a BOT
    {
        std::cout << stackValue->varName << " = ";
        // printing directly through vectors
        if(stackValue->centralVector.find("0") != stackValue->centralVector.end())
            std::cout << stackValue->centralVector["0"] << "e0"; // printing the central stackvalue
        std::map<std::string, double>::iterator itr; // printing the central noise terms
        for(itr = stackValue->centralVector.begin(); itr!= stackValue->centralVector.end(); ++itr)
        {
            if(itr == stackValue->centralVector.begin())
                continue;
            std::cout << " + " << itr->second << "e" << itr->first;
        }
        for(itr = stackValue->perturbedVector.begin(); itr!= stackValue->perturbedVector.end(); ++itr)// printing the perturved noise terms
            std::cout << " + " << itr->second << "n" << itr->first;
        std::cout << std::endl;
    }
}
ZonotopeStackValue* Zonotope::copyStackValue(ZonotopeStackValue* s) // takes a stack value, copies it and returns a pointer to the copy
{
    std::cout << "I am in Zonotope::copyStackValue" << std::endl;
    // copies s into o

    // checks for TOP and BOT stack values
    if(s->flag == s_TOP)
        return topStackValue();
    else if (s->flag == s_BOT)
        return botStackValue();

    // proceeds only when the stackValue is neither TOP nor BOT
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
    std::cout << "I am in Zonotope::copyAbstractValue" << std::endl;
    ZonotopeAbstractValue *a = (ZonotopeAbstractValue*) abstract_value;
    ZonotopeAbstractValue *x = new ZonotopeAbstractValue;

    // checks for TOP and BOT abstract values
    if(a->flag == a_TOP)
        return topValue();
    else if(a->flag == a_BOT)
        return botValue();

    // proceeds only when the abstract value is neither TOP nor BOT
    // DeepCopy a into x
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

// just sends out the abstract_value_2
AbstractValue* Zonotope::meet(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    std::cout << "I am in Zonotope::meet" << std::endl;
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    result = (ZonotopeAbstractValue*) copyAbstractValue(abstract_value_2);

    return result;
}

AbstractValue* Zonotope::join(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    std::cout << "I am in Zonotope::join" << std::endl;
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    // Do join operation on op1 and op2 to get result.

    result = componentWiseJoin(op1,op2);
    //printAbstractValue(result);
    return result;
}

AbstractValue* Zonotope::widen(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    std::cout << "I am in Zonotope::widen" << std::endl;
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;
    ZonotopeAbstractValue *result = new ZonotopeAbstractValue;

    // Do widen operation on op1 and op2 to get result.

    return result;
}


LatticeCompare Zonotope::compare(AbstractValue* abstract_value_1, AbstractValue* abstract_value_2)
{
    std::cout << "I am in Zonotope::compare" << std::endl;
    ZonotopeAbstractValue *op1 = (ZonotopeAbstractValue*) abstract_value_1;
    ZonotopeAbstractValue *op2 = (ZonotopeAbstractValue*) abstract_value_2;

    // Compare op1 and op2 and return the result.
    // if op1 == op2, return EQ
    // if op1 > op2, return GT
    // if op1 < op2, return LT
    // if op1 and op2 are uncomparable, return UC

    return UC;
}

AbstractValue* Zonotope::assignStackValue(std::string variable_name, std::string variable_type, StackValue* rhs_stack_value, AbstractValue* current_abstract_value) // assigns a stackValue and adds it to the affineSet
{
    std::cout << "I am in Zonotope::assignStackValue" << std::endl;
    ZonotopeStackValue *rhs = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);

    // Add the constraint variable_name = rhs, to abstract_value.
    if(isTopStackValue(rhs)) //    printStackValue(rhs); if the stack value is a top, it removes it from the affine-set (this is the same as the stack value having no constraints over it)
    {
        if(abstract_value->affineSet.find(variable_name) != abstract_value->affineSet.end()) // checks if the variable is present in the affine-set, if yes, then removes it
        {
            rhs = abstract_value->affineSet[variable_name];
            abstract_value = removeStackValue(abstract_value, rhs->varPos);
        }
        return abstract_value;
    }
    else if(isBotStackValue(rhs)) // if the stack value if BOT, it returns a BOT abstract value
    {
        delete abstract_value;
        return botValue();
    }
    rhs->varName = variable_name;
    // otherwise, adds the stack value into the abstract value
    abstract_value = addVariableToAffineSet(rhs,abstract_value); // adding to the number of variables
    abstract_value->affineSet[variable_name] = rhs;
    abstract_value->flag = a_NONE;
    return abstract_value;
}

// NEEDS CORRECTION - implicit conversion before the function is even called
StackValue* Zonotope::getStackValueOfLiteral(std::string type, double value, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::getStackValueOfLiteral" << std::endl;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // result = stackvalue of the value
        // the name of the literal is stored as string denoting its position in the matrix

    // makes a new stack value for the appropriate literal and outputs the same
    std::pair<double,double> p = std::make_pair(-1*value,value);
    if(checkOverflow(type, p))
    {
        ZonotopeStackValue *output = new ZonotopeStackValue;
        output->centralVector["0"] = value;
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else // if the output type does not match - this does not function correctly!!
    {
        std::cout << "Unsupported Type : " << type << std::endl;
        assert("false");
        return topStackValue();
    }
}


StackValue* Zonotope::getStackValueOfVariable(std::string variableName, std::string variable_type, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::getStackValueOfVariable" << std::endl;
    ZonotopeStackValue *s = new ZonotopeStackValue;
    ZonotopeAbstractValue *currentAbstractValue = (ZonotopeAbstractValue*) current_abstract_value;

    // result = stackvalue of the variable.

    // looks in the affine_set for the required variable
    // if the variable is not present, returns TOP
    auto itr = currentAbstractValue->affineSet.find(variableName);

    if(itr == currentAbstractValue->affineSet.end()) // if the variable is not present in the affine-set
    {
        s->flag = s_TOP;
        return s;
    }
    else // if the variable is present in the affine-set
    {
        s->varPos = itr->second->varPos;
        s->flag = itr->second->flag;
        s->centralVector = itr->second->centralVector;
        s->perturbedVector = itr->second->perturbedVector;
        return s;
    }

}

// make usage of return type conversion
StackValue* Zonotope::evaluateBinaryOperation(std::string opcode, std::string return_type, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::evaluateBinaryOperation" << std::endl;
    ZonotopeStackValue *op1 = (ZonotopeStackValue*) lhs_stack_value;
    ZonotopeStackValue *op2 = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // Do binary operation on op1 and op2 to get result
    // Includes all binary arithematic operations.

    // checks whether the two numbers are bot or top
    if((isBotStackValue(op1) || isBotStackValue(op2)))
        return botStackValue();
    else if((isTopStackValue(op1) || isTopStackValue(op2)))
        return topStackValue();

    if(strcmp(opcode.c_str(),"+") == 0) // performs binary addition
    {
        ZonotopeStackValue* s = new ZonotopeStackValue; // initiliazing the new variable
        //s->varName = std::to_string(globalCounter); globalCounter = globalCounter + 1; // setting the name of the variable
        s->flag = s_NONE; // setting the flag to show it is not a TOP or a BOT value

        std::map <std::string, double> temp; // to set the centralVector
        std::map <std::string, double>::iterator itr;
        for(itr = op1->centralVector.begin(); itr != op1->centralVector.end(); ++itr)
        {
            temp.insert(*itr);
        }
        for(itr = op2->centralVector.begin(); itr != op2->centralVector.end(); ++itr)
        {
            if(temp.find(itr->first) != temp.end())
            {
                temp[itr->first] = temp[itr->first] + itr->second;
            }
            else
            {
                temp.insert(*itr);
            }
        }
        s->centralVector = temp;
        temp.clear();
        for(itr = op1->perturbedVector.begin(); itr != op1->perturbedVector.end(); ++itr)
        {
            temp.insert(*itr);
        }
        for(itr = op2->perturbedVector.begin(); itr != op2->perturbedVector.end(); ++itr)
        {
            if(temp.find(itr->first) != temp.end())
            {
                temp[itr->first] = temp[itr->first] + itr->second;
            }
            else
            {
                temp.insert(*itr);
            }
        }
        s->perturbedVector = temp;
        return s;
    }

    else if(strcmp(opcode.c_str(),"-") == 0) // performs binary subtraction
    {
        ZonotopeStackValue* s = new ZonotopeStackValue; // initiliazing the new variable
        //s->varName = std::to_string(globalCounter); globalCounter = globalCounter + 1; // setting the name of the variable
        s->flag = s_NONE; // setting the flag to show it is not a TOP or a BOT value

        std::map <std::string, double> temp; // to set the centralVector
        std::map <std::string, double>::iterator itr;
        for(itr = op1->centralVector.begin(); itr != op1->centralVector.end(); ++itr)
        {
            temp.insert(*itr);
        }
        for(itr = op2->centralVector.begin(); itr != op2->centralVector.end(); ++itr)
        {
            if(temp.find(itr->first) != temp.end())
            {
                temp[itr->first] = temp[itr->first] - itr->second;
            }
            else
            {
                temp.insert(*itr);
            }
        }
        s->centralVector = temp;
        temp.clear();
        for(itr = op1->perturbedVector.begin(); itr != op1->perturbedVector.end(); ++itr)
        {
            temp.insert(*itr);
        }
        for(itr = op2->perturbedVector.begin(); itr != op2->perturbedVector.end(); ++itr)
        {
            if(temp.find(itr->first) != temp.end())
            {
                temp[itr->first] = temp[itr->first] - itr->second;
            }
            else
            {
                temp.insert(*itr);
            }
        }
        s->perturbedVector = temp;
        return s;
    }
    else if(strcmp(opcode.c_str(),"*") == 0) // performs binary multiplication // check if this is correct
    {
        ZonotopeStackValue* s = new ZonotopeStackValue; // initiliazing the new variable
        s->flag = s_NONE;
        
        // setting the new central value
        double x0 = 0;
        double y0 = 0;
        double x = 0;
        double y = 0;
        if(op1->centralVector.find("0") != op1->centralVector.end())
            x0 = op1->centralVector["0"];
        if(op2->centralVector.find("0") != op2->centralVector.end())
            y0 = op2->centralVector["0"];
        s->centralVector["0"] = x0 * y0;

        double new_term = 0;

        for(int i = 1; i < abstract_value->n; i++)
        {
            x = 0;
            y = 0;
            
            if(op1->centralVector.find(std::to_string(i)) != op1->centralVector.end())
                x = op1->centralVector[std::to_string(i)];
            if(op2->centralVector.find(std::to_string(i)) != op2->centralVector.end())
                y = op2->centralVector[std::to_string(i)];
            s->centralVector["0"] = s->centralVector["0"] + (0.5)*abs(x*y);
            new_term = new_term + (0.5)*abs(x*y);
        }
        for(int i = 0; i < abstract_value->m; i++)
        {
            x = 0;
            y = 0;
            if(op1->perturbedVector.find(std::to_string(i)) != op1->perturbedVector.end())
                x = op1->perturbedVector[std::to_string(i)];
            if(op2->perturbedVector.find(std::to_string(i)) != op2->perturbedVector.end())
                y = op2->perturbedVector[std::to_string(i)];
            s->centralVector["0"] = s->centralVector["0"] + (0.5)*abs(x*y);
            new_term = new_term + (0.5)*abs(x*y);
        }

        // setting the central noise terms
        for(int i = 1; i < abstract_value->n; i++)
        {
            x = 0;
            y = 0;
            if(op1->centralVector.find(std::to_string(i)) != op1->centralVector.end())
                x = op1->centralVector[std::to_string(i)];
            if(op2->centralVector.find(std::to_string(i)) != op2->centralVector.end())
                y = op2->centralVector[(std::to_string(i))];
            if(x0*y + y0*x != 0)
                s->centralVector[std::to_string(i)] = x0*y + y0*x;
        }
        
        // setting the perturbed noise terms
        for(int i = 0; i < abstract_value->m; i++)
        {
            x = 0;
            y = 0;
            if(op1->perturbedVector.find(std::to_string(i)) != op1->perturbedVector.end())
                x = op1->perturbedVector[std::to_string(i)];
            if(op2->perturbedVector.find(std::to_string(i)) != op2->perturbedVector.end())
                y = op2->perturbedVector[(std::to_string(i))];
            if(x0*y + y0*x != 0)
                s->centralVector[std::to_string(i)] = x0*y + y0*x;
        }

        double x1 = 0;
        double x2 = 0;
        double y1 = 0;
        double y2 = 0;

        // adding the new noise term
        for(int i = 1; i < abstract_value->n; i++)
        {
            x1 = 0;
            y1 = 0;
            if((op1->centralVector.find(std::to_string(i)) != op1->centralVector.end()) || (op2->centralVector.find(std::to_string(i)) != op2->centralVector.end()))
            {
                if(op1->centralVector.find(std::to_string(i)) != op1->centralVector.end())
                    x1 = op1->centralVector[std::to_string(i)];
                if(op2->centralVector.find(std::to_string(i)) != op2->centralVector.end())
                    y1 = op2->centralVector[std::to_string(i)]; 
                for(int j = i + 1; j < abstract_value->n; j++)
                {
                    x2 = 0;
                    y2 = 0;
                    if(op1->centralVector.find(std::to_string(j)) != op2->centralVector.end())
                        x2 = op1->centralVector[std::to_string(j)];
                    if(op2->centralVector.find(std::to_string(j)) != op2->centralVector.end())
                        y2 = op2->centralVector[std::to_string(j)];
                    
                    new_term = new_term + abs(x1*y2 + x2*y1);
                    
                }
            }
        }
        for(int i = 0; i < abstract_value->m; i++)
        {
            x1 = 0;
            y1 = 0;
            if((op1->perturbedVector.find(std::to_string(i)) != op1->perturbedVector.end()) || (op2->perturbedVector.find(std::to_string(i)) != op2->perturbedVector.end()))
            {
                if(op1->perturbedVector.find(std::to_string(i)) != op1->perturbedVector.end())
                    x1 = op1->perturbedVector[std::to_string(i)];
                if(op2->perturbedVector.find(std::to_string(i)) != op2->perturbedVector.end())
                    y1 = op2->perturbedVector[std::to_string(i)];
                for(int j = i + 1; j < abstract_value->n; j++)
                {
                    x2 = 0;
                    y2 = 0;
                    if(op1->perturbedVector.find(std::to_string(j)) != op2->perturbedVector.end())
                        x2 = op1->perturbedVector[std::to_string(j)];
                    if(op2->perturbedVector.find(std::to_string(j)) != op2->perturbedVector.end())
                        y2 = op2->perturbedVector[std::to_string(j)];
                    
                    new_term = new_term + abs(x1*y2 + x2*y1);
                    
                }
            }
        }
        if(new_term != 0)
            s->perturbedVector[std::to_string(abstract_value->m)] = new_term;
        return s;
    }
    
    return topStackValue();
}

StackValue* Zonotope::evaluateUnaryOperation(std::string opcode, std::string return_type, StackValue* stack_value, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::evaluateUnaryOperation" << std::endl;
    ZonotopeStackValue *operand = (ZonotopeStackValue*) stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    // Do unary operation on operand to get resultstd::pair<AbstractValue*, AbstractValue*> assumeConstraint(std::string opcode, StackValue* lhs_stack_value, StackValue* op2, AbstractValue* current_abstract_value)
    // Includes all unary arithematic operations.

    return result;
}

StackValue* Zonotope::castStackValue(std::string src_type, std::string dest_type, StackValue* stack_value, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::castStackValue" << std::endl;
    ZonotopeStackValue *operand = (ZonotopeStackValue*) stack_value;
    ZonotopeStackValue *result = new ZonotopeStackValue;
    ZonotopeAbstractValue *abstract_value = (ZonotopeAbstractValue*) current_abstract_value;

    std::pair<double,double> interval = concretize(operand, abstract_value);
    // Do cast operation on operand to get result
    // Includes all unary arithematic operations.

    if(checkOverflow(dest_type, interval))
    {
        result = copyStackValue(operand);
        return result;
    }
    else
    {
        std::cout << "OVER-FLOW DETECTED" << std::endl;
        result->flag = s_TOP;
        return result;
    }
    
}

std::pair<AbstractValue*, AbstractValue*> Zonotope::assumeConstraint(std::string opcode, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* current_abstract_value)
{
    std::cout << "I am in Zonotope::assumeConstraint" << std::endl;
    ZonotopeStackValue *lhs = (ZonotopeStackValue*) lhs_stack_value;
    ZonotopeStackValue *rhs = (ZonotopeStackValue*) rhs_stack_value;
    ZonotopeAbstractValue *result_true = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);
    ZonotopeAbstractValue *result_false = (ZonotopeAbstractValue*) copyAbstractValue(current_abstract_value);

    // the greater-equal case and the lesser-equal case are not being dealt seperately as of now

    // haven't changed the false branch as of now

    if(strcmp(opcode.c_str(),"<") == 0) // lhs_stack_value is less than rhs_stack_value
    {
        int trueIsBot = 0;
        int falseIsBot = 0;
        std::vector<std::pair<double,double>> constraintOverCentralMatrix; // saves the constains over Cx
        std::vector<std::pair<double,double>> constraintOverPerturbedMatrix; // saves the constains over Px
        std::pair<double,double> p;
        for(int i = 1; i < result_true->n; i++)
        {
            if(trueIsBot == 1)
                break;
            p = getConstraint(0,i,lhs,rhs,result_true);
            if(p.second < p.first)
                trueIsBot = 1;
            constraintOverCentralMatrix.push_back(p);
        }
        for(int i = 0; i < result_true->m; i++)
        {
            if(trueIsBot == 1)
                break;
            p = getConstraint(1,i,lhs,rhs,result_true);
            if(p.second < p.first)
                trueIsBot = 1;
            constraintOverPerturbedMatrix.push_back(p);
        }
        if(trueIsBot == 0)
        {
            result_true->constraintOverCentralMatrix = constraintOverCentralMatrix;
            result_true->constraintOverPerturbedMatrix = constraintOverPerturbedMatrix;
        }
        else
        {
            result_true = (ZonotopeAbstractValue*) botValue();
        }
    }
    else if(strcmp(opcode.c_str(),">") == 0)
    {
        return assumeConstraint("<",rhs_stack_value,lhs_stack_value,current_abstract_value);
    }


    // Add the constraint lhs opcode rhs to result_true
    // Add the constraint !(lhs opcode rhs) to result_false
    
    return std::make_pair<AbstractValue*, AbstractValue*>(result_true, result_false);
}

std::pair<double, double> Zonotope::concretize(ZonotopeStackValue* s, ZonotopeAbstractValue* a) // returns the concretized value of the stack value
{
    std::cout << "I am in Zonotope::concretize" << std::endl;
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
    std::cout << "I am in Zonotope::addVariableToAffineSet" << std::endl;
    // if stack value is neither a top or a bot
    if(stValue->flag == s_TOP)
        return abValue;

    // augmenting cols in the central and perturbed matrices
    if(abValue->affineSet.find(stValue->varName) == abValue->affineSet.end()) // if the variable is not already present
    {
        abValue->centralMatrix.insert_cols(abValue->p,1);
        abValue->perturbedMatrix.insert_cols(abValue->p,1);
        stValue->varPos = abValue->p;
        abValue->p = abValue->p + 1;
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
    max1 = -1;
    // finding the max perturbed noise symbol
    if(stValue->perturbedVector.rbegin() != stValue->perturbedVector.rend())
    {
        if(std::stoi(stValue->perturbedVector.rbegin()->first) > max1)
            max1 = std::stoi(stValue->perturbedVector.rbegin()->first);
    }
    if(max1 > 0 && abValue->m != 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(abValue->m, max1 - abValue->m);
    else if(max1 == 0 && abValue->m == 0)
        abValue->perturbedMatrix.insert_rows(0, max1 - abValue->m + 1);
    else if(max1 > 0 && abValue->m == 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(0, max1 - abValue->m);


    for(auto itr = stValue->perturbedVector.begin(); itr != stValue->perturbedVector.end(); ++itr)
    {
        abValue->perturbedMatrix(std::stoi(itr->first),stValue->varPos) = itr->second;
    }
    // augmenting the vector of constraints
    if(abValue->perturbedMatrix.n_rows > abValue->m)
    {
        abValue->m = abValue->perturbedMatrix.n_rows;
        for(int i = 0; i <= (abValue->constraintOverPerturbedMatrix.size() - abValue->m); i++)
            abValue->constraintOverPerturbedMatrix.push_back(std::make_pair(-1,1));
    }

    abValue->flag = a_NONE;
    abValue->n = abValue->centralMatrix.n_rows;
    abValue->m = abValue->perturbedMatrix.n_rows;
    abValue->p = abValue->centralMatrix.n_cols;
    return abValue;
    // Included the perturbed form as well
}

ZonotopeAbstractValue* Zonotope::removeStackValue(ZonotopeAbstractValue* a, int k) // removes the variable at position k in the matrix from the affine-set
{
    std::cout << "I am in Zonotope::removeStackValue" << std::endl;
    if(a->p < k) // the stack_value doesn't exist
        return a;
    // removing it from the affine-set
    std::map<std::string, ZonotopeStackValue*>::iterator itr;
    for(itr = a->affineSet.begin(); itr != a->affineSet.end(); ++itr)
    {
        if(itr->second->varPos == k)
            break;
    }

    // something wrong with the structure
    if(itr == a->affineSet.end())
        return a;
    else
        a->affineSet.erase(itr->first); // removing the stack value from the affine set
    if(a->n != 0) // checking for a 0 matrix
        a->centralMatrix.shed_col(k);
    if(a->m != 0)// checking for a 0 matrix
        a->perturbedMatrix.shed_col(k);
    for(itr = a->affineSet.begin(); itr != a->affineSet.end(); ++itr)
    {
        if(itr->second->varPos > k) // correcting the variable position for all the stack variables
            itr->second->varPos = itr->second->varPos - 1;
    }
    a->p = a->p - 1;
    
    // if there are no variables that is the same as the abstract set being a TOP
    if(a->p == 0)
        a->flag = a_TOP;

    return a;
    
}

template <typename T1, typename T2, typename T3, typename T4>
bool Zonotope::intervalCompare(std::pair<T1,T2> p1, std::pair<T3,T4> p2) // returns true if p2 <= p1
{
    std::cout << "I am in Zonotope::intervalCompare" << std::endl;
    if(p2.second <= p1.second && p2.first >= p1.first)
        return true;
    return false;
}


template <typename T1, typename T2>
bool Zonotope::checkOverflow(std::string datatype, std::pair<T1, T2> interval) // returns false if there is overflow
{
    std::cout << "I am in Zonotope::checkOverFlow" << std::endl;

    if(interval.second < interval.first)
        return false;

    if(datatype.compare("short") == 0) return intervalCompare(std::make_pair(-1*std::numeric_limits<short>::max() - 1, std::numeric_limits<short>::max()), interval);
    else if(datatype.compare("int") == 0) return intervalCompare(std::make_pair(-1*std::numeric_limits<int>::max() - 1, std::numeric_limits<int>::max()), interval);
    else if(datatype.compare("long") == 0) return intervalCompare(std::make_pair(-1*std::numeric_limits<long>::max() - 1, std::numeric_limits<long>::max()), interval);
    else if(datatype.compare("long long") == 0) return intervalCompare(std::make_pair(-1*std::numeric_limits<long long>::max() - 1, std::numeric_limits<long long>::max()), interval);
    else if(datatype.compare("unsigned short") == 0) return intervalCompare(std::make_pair(0, std::numeric_limits<unsigned short>::max()), interval);
    else if(datatype.compare("unsigned int") == 0) return intervalCompare(std::make_pair(0, std::numeric_limits<unsigned int>::max()), interval);
    else if(datatype.compare("unsigned long") == 0) return intervalCompare(std::make_pair(0, std::numeric_limits<unsigned long>::max()), interval);
    else if(datatype.compare("unsigned long long") == 0) return intervalCompare(std::make_pair(0, std::numeric_limits<unsigned long long>::max()), interval);
    else return false;
}

double Zonotope::argmin(double a, double b) // gives the min. interval length in which both a and b can be covered
{
    return abs(std::max(a,b) - std::min(a,b));
}

ZonotopeStackValue* Zonotope::oneDimensionalJoin(ZonotopeStackValue* s1, ZonotopeAbstractValue* a1, ZonotopeStackValue* s2, ZonotopeAbstractValue* a2, int counter) // performs a one-dimensional join and returns the pointer to the resulting ZonotopeStackValue
{
    std::cout << "I am in Zonotope::oneDimensionalJoin" << std::endl;
    // counter is incremented depending upon how many times the function is called in order to maintain the uniqueness in the affine symbols
    //std::cout << s1->flag << std::endl << s2->flag << std::endl << a1->flag << std::endl << a2->flag << std::endl;
    // considering the cases with TOP values
    if(a1->flag == a_TOP || a2->flag == a_TOP) // if the either of the Affine sets are empty
        return topStackValue();
    if((a1->affineSet.find(s1->varName) == a1->affineSet.end()) || (a2->affineSet.find(s2->varName) == a2->affineSet.end())) // if either of the variables are TOP
        return topStackValue();   
    if(s1->varName.compare(s2->varName) != 0) // if both variables are different, this does not join
        return topStackValue();

    // considering the cases with BOT values
    if(a1->flag == a_BOT && a2->flag == a_BOT)
        return botStackValue();
    else if(a1->flag == a_BOT) // if only a1 is BOT
    {
        if(a2->affineSet.find(s2->varName) != a2->affineSet.end())
            return s2;
        else
            return topStackValue();
    }
    else if(a2->flag == a_BOT)
    {
        if(a1->affineSet.find(s1->varName) != a1->affineSet.end())
            return s1;
        else
            return topStackValue();
    }

    // if none of the values are TOP or BOT and both the variables exist
    else
    {
        // forming the required vectors
        arma::Col<double> cx = a1->centralMatrix.col(s1->varPos);
        arma::Col<double> px = a1->perturbedMatrix.col(s1->varPos);
        arma::Col<double> cy = a2->centralMatrix.col(s2->varPos);
        arma::Col<double> py = a2->perturbedMatrix.col(s2->varPos);

        arma::Col<double> cz = arma::zeros(std::max(cx.n_rows, cy.n_rows));
        arma::Col<double> pz = arma::zeros(std::max(px.n_rows, py.n_rows) + counter);

        
        // setting the central term
        cz[0] = (std::max(concretize(s1, a1).second, concretize(s2,a1).second) + std::min(concretize(s1,a1).first, concretize(s2,a2).first))*(0.5);
        
        // setting the central noise terms
        int i = 0;
        for(i = 1; i < std::min(cx.n_rows, cy.n_rows); i++)
            cz[i] = argmin(cx[i], cy[i]);

        if(cx.n_rows > cy.n_rows)
        {
            for(;i<cx.n_rows;i++)
                cz[i] = cx[i];
        }
        else if(cy.n_rows > cx.n_rows)
        {
            for(;i<cy.n_rows;i++)
                cz[i] = cy[i];
        }

        // setting the perturbed noise terms
        for(i = 0; i < std::min(px.n_rows, py.n_rows); i++)
            pz[i] = argmin(px[i], py[i]);
        
        if(px.n_rows > py.n_rows)
        {
            for(;i<px.n_rows;i++)
                pz[i] = px[i];
        }
        else if(py.n_rows > px.n_rows)
        {
            for(;i<py.n_rows;i++)
                pz[i] = py[i];
        }

        // additional noise terms
        double z0 = std::max(concretize(s1,a1).second, concretize(s2,a2).second) - cz[0];
        for(int i = 1; i < cz.n_rows; i++)
            z0 = z0 - abs(cz[i]);
        for(int i = 0; i < pz.n_rows; i++)
            z0 = z0 - abs(pz[i]);
        
        // making the output stackValue
        ZonotopeStackValue* z = new ZonotopeStackValue;
        z->varName = s1->varName;
        z->varPos = counter;
        z->flag = s_NONE;
        for(int i = 0; i < cz.n_rows; i++)
        {
            if(cz[i] != 0)
                z->centralVector[std::to_string(i)] = cz[i];
        }

        for(int i = 0; i < (pz.n_rows - 1); i++)
        {
            if(pz[i] != 0)
                z->perturbedVector[std::to_string(i)] = pz[i];
        }
        
        if(z0 != 0)
            z->perturbedVector[std::to_string(i)] = z0;
        return z;

    }

    return topStackValue();
}

ZonotopeAbstractValue* Zonotope::componentWiseJoin(ZonotopeAbstractValue* X, ZonotopeAbstractValue* Y) // finds the component wise join using the one dimensional join
{
    std::cout << "I am in Zonotope::componentWiseJoin" << std::endl;
    // cases where one of the values is top
    if(X->flag == a_TOP || Y->flag == a_TOP)
        return (ZonotopeAbstractValue*) topValue();
    
    // cases where one of the values are bot
    if(X->flag == a_BOT && Y->flag == a_BOT)
        return (ZonotopeAbstractValue*) botValue();
    else if(X->flag == a_BOT)
        return Y;
    else if(Y->flag == a_BOT)
        return X;

    // cases where both the affineSets have the flag = NONE
    ZonotopeAbstractValue* Z = new ZonotopeAbstractValue;
    Z->affineSetName = X->affineSetName;
    Z->flag = a_NONE;
    int counter = 1;
    for(auto itr1 = X->affineSet.begin(); itr1 != X->affineSet.end(); ++itr1)
    {
        if(Y->affineSet.find(itr1->first) != Y->affineSet.end())
        {
            addVariableToAffineSet(oneDimensionalJoin(itr1->second, X, Y->affineSet[(itr1->first)], Y, counter),Z);
            counter = counter + 1;
        }
    }
    return Z;
}

std::pair<double, double> Zonotope::getConstraint(int CorP, int pos, ZonotopeStackValue* lhs, ZonotopeStackValue* rhs, ZonotopeAbstractValue* a)
{
    std::cout << "I am in Zonotope::getConstraint" << std::endl;
    // gets the constraint for the the case lhs < rhs
    // CorP = 0 - constraint on central, CorP = 1 - constraint on perturbed

    arma::Col<double> Cx = (a->centralMatrix.col(lhs->varPos)) - (a->centralMatrix.col(rhs->varPos));
    arma::Col<double> Px = (a->perturbedMatrix.col(lhs->varPos)) - (a->perturbedMatrix.col(rhs->varPos));

    if(Cx[pos] == 0 && CorP == 0)
        return std::make_pair(-1,1);
    else if(Px[pos] == 0 && CorP == 1)
        return std::make_pair(-1,1);
    else
    {
        double output = -1*Cx[0];
        if((Cx[pos] < 0 && CorP == 0) || (Px[pos] < 0 && CorP == 1)) // then we minimize the numerator
        {
            for(int i = 1; i < Cx.n_rows; i++)
            {
                if(i == pos && CorP == 0)
                    continue;
                if(Cx[i] < 0)
                    output = output + Cx[i]*a->constraintOverCentralMatrix[i-1].second;
                else    
                    output = output + Cx[i]*a->constraintOverCentralMatrix[i-1].first;
            }
            for(int i = 0; i < Px.n_rows; i++)
            {
                if(i == pos && CorP == 1)
                    continue;
                if(Px[i] < 0)
                    output = output + Px[i]*a->constraintOverPerturbedMatrix[i].second;
                else    
                    output = output + Px[i]*a->constraintOverPerturbedMatrix[i].first;
            }
        }
        else // we maximize the numerator
        {
            for(int i = 1; i < Cx.n_rows; i++)
            {
                if(i == pos && CorP == 0)
                    continue;
                if(Cx[i] < 0)
                    output = output + Cx[i]*a->constraintOverCentralMatrix[i-1].first;
                else    
                    output = output + Cx[i]*a->constraintOverCentralMatrix[i-1].second;
            }
            for(int i = 0; i < Px.n_rows; i++)
            {
                if(i == pos && CorP == 1)
                    continue;
                if(Px[i] < 0)
                    output = output + Px[i]*a->constraintOverPerturbedMatrix[i].first;
                else    
                    output = output + Px[i]*a->constraintOverPerturbedMatrix[i].second;
            }
        }

        if(CorP == 0)
            output = output / Cx[pos];
        else
            output = output / Px[pos];

        if(output < 1)
            return std::make_pair(-1,output);
        else
            return std::make_pair(-1,1);
    }
    return std::make_pair(-1,1);
}

std::pair<double,double> Zonotope::intervalMeet(std::pair<double,double> p1, std::pair<double,double> p2)
{
    // considering the cases where the meet gives an impossible value
    if((p1.second < p2.first) || (p2.second < p1.first))
        return std::make_pair(1,-1);
    // considering the case where one is a subset of the other
    return std::make_pair(std::max(p1.first, p2.first), std::min(p1.second, p2.second));
}