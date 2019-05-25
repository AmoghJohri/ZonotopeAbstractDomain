#include "Zonotope.hh"

Zonotope::Zonotope(){};

StackValue Zonotope::topStackValue()
{
    // returns the address to top stack value
    return (this->topStackVal);
}

StackValue Zonotope::botStackValue()
{
    // returns the address to bot stack value
    return (this->botStackVal);
}


bool Zonotope::isTopStackValue(StackValue* s)
{
    if(s->flag == s_TOP)
        return true;
    return false;
}

bool Zonotope::isBotStackValue(StackValue* s)
{
    if(s->flag == s_BOT)
        return true;
    return false;
}

void Zonotope::printStackValue(AbstractValue* abstractValue, StackValue* stackValue)
{
    std::map<std::string, StackValue*>::iterator itr;

    // looks in the affine_set for the required variable
    for(itr = abstractValue->affineSet.begin(); itr != abstractValue->affineSet.end(); ++itr)
    {
        if((itr->second)->varName.compare(stackValue->varName) == 0)
            break;
    }

    // if the variable is not present, returns TOP
    if(itr == abstractValue->affineSet.end())
        std::cout << "TOP" << std::endl;
    else 
    {
        if((itr->second)->lv == LITERAL)
        {
            std::cout << (itr->second)->varName << " = " << (itr->second)->concreteValue.first;
        }
        int pos = (itr->second)->varPos; // the col. of the matrices where the variable is present

        std::cout << (itr->second)->varName << " = ";

        // prints the value of the central matrix
        std::cout << abstractValue->centralMatrix(0,pos);
        for(int counter = 1; counter < (abstractValue->centralMatrix).n_rows; counter++)
        {
            std::cout << " + " << abstractValue->centralMatrix(counter,pos) << "e" << counter;
        }

        // prints the value of the perturbed matrix
        for(int counter = 0; counter < (abstractValue->perturbedMatrix).n_rows; counter++)
        {
            std::cout << " + " << abstractValue->perturbedMatrix(counter,pos) << "n" << counter+1;
        }

    }
}

StackValue* getStackValueOfLiteral(std::string type, double value, AbstractValue* currentAbstractValue) // add the apron features
{
    // the name of the literal is stored as string denoting its position in the matrix
    if(strcmp(type.c_str(), "int") == 0)
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->concreteValue = std::make_pair((int)value,(int)value);
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else if(strcmp(type.c_str(), "real") == 0)
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->concreteValue = std::make_pair((int)value,(int)value);
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else
    {
        std::cout << "Unknown Type" << type << std::endl;
        assert(false);
    }
}


// fetches the stack value of the variable from the affine-set
StackValue* Zonotope::getStackValueOfVariable(std::string variableName, std::string variableType, AbstractValue* currentAbstractValue)
{
    std::map<std::string, StackValue*>::iterator itr;

    // looks in the affine_set for the required variable
    for(itr = currentAbstractValue->affineSet.begin(); itr != currentAbstractValue->affineSet.end(); ++itr)
    {
        if((itr->second)->varName.compare(variableName) == 0)
            break;
    }
    // if the variable is not present, returns TOP
    if(itr == currentAbstractValue->affineSet.end())
        return &(this->topStackVal); //try not to alter this
    else
        return itr->second;
}

// left for later
AbstractValue* Zonotope::assignStackValue(std::string variableName, std::string variableType, StackValue* rhsStackValue, AbstractValue* currentAbstractValue)
{
    return currentAbstractValue;
}

void Zonotope::printAbstractValue(AbstractValue* currentAbstractValue)
{
    std::cout << currentAbstractValue->centralMatrix;
    std::vector<std::pair<double,double>>::iterator itr;
    for(itr = currentAbstractValue->constraintOverCentralMatrix.begin(); itr != currentAbstractValue->constraintOverCentralMatrix.end(); ++itr)
    {
        std::cout << "[" << itr->first << "," << itr->second << "]";
        if(itr != currentAbstractValue->constraintOverCentralMatrix.end() - 1)
            std::cout<<" X ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << currentAbstractValue->perturbedMatrix;
    for(itr = currentAbstractValue->constraintOverPerturbedMatrix.begin(); itr != currentAbstractValue->constraintOverPerturbedMatrix.end(); ++itr)
    {
        std::cout << "[" << itr->first << "," << itr->second << "]";
        if(itr!=currentAbstractValue->constraintOverPerturbedMatrix.end())
            std::cout<<" X ";
    }
    std::cout << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// creates an empty affine set with name s
AbstractValue Zonotope::createAffineSet(std::string s)
{
    AbstractValue a = {
        .affineSetName = s,
        .n = 0,
        .m = 0,
        .p = 0,
        .flag = a_TOP,
        .centralMatrix = arma::zeros(0,0),
        .perturbedMatrix = arma::zeros(0,0),
    };
    return a;
}

// adding custom intervals as variables
AbstractValue* Zonotope::addCustomVariable(std::string s, std::pair<double,double> p, AbstractValue* currentAbstractValue)
{
    // setting the stack value of the variable
    StackValue* variable = new StackValue;
    variable->varName = s;
    variable->lv = VARIABLE;
    variable->varPos = currentAbstractValue->p;
    variable->flag = s_NONE;
    currentAbstractValue->affineSet.insert(std::make_pair(s, variable));

    // setting the abstract value
    if(currentAbstractValue->n == 0)
        currentAbstractValue->centralMatrix.insert_rows(0,2);
    else
        currentAbstractValue->centralMatrix.insert_rows(currentAbstractValue->n+1,1);
    currentAbstractValue->centralMatrix.insert_cols(currentAbstractValue->p,1);

    currentAbstractValue->centralMatrix(0, currentAbstractValue->p) = (p.first + p.second)/2;
    currentAbstractValue->n = currentAbstractValue->n + 1;
    currentAbstractValue->centralMatrix(currentAbstractValue->n, currentAbstractValue->p) = (p.second - p.first)/2;
    currentAbstractValue->p = currentAbstractValue->p + 1;
    currentAbstractValue->constraintOverCentralMatrix.push_back(std::make_pair(-1.0,1.0));
    return currentAbstractValue;
}


int main()
{
    Zonotope zonotope;

    AbstractValue X = zonotope.createAffineSet("X");
    zonotope.addCustomVariable("x1",std::make_pair(0,10),&X);
    zonotope.addCustomVariable("x2",std::make_pair(-5,5),&X);
    zonotope.addCustomVariable("x3",std::make_pair(-12,16),&X);
    zonotope.addCustomVariable("x4",std::make_pair(-7,12),&X);
    zonotope.printAbstractValue(&X);


    return 0;

}
