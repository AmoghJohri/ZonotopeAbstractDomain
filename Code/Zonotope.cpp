#include "Zonotope.hh"

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

StackValue* getStackValueOfLiteral(std::string type, double value, AbstractValue* currentAbstractValue)
{
    // the name of the literal is stored as string denoting its position in the matrix
    std::string apron_type_of_literal = getApronType(type);

    if(strcmp(apron_type_of_literal.c_str(), "int") == 0)
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->concreteValue = std::make_pair((int)value,(int)value);
        output->varPos = -2;
        output->flag = s_NONE;
    }
    else if(strcmp(apron_type_of_literal.c_str(), "real") == 0)
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->concreteValue = std::make_pair((int)value,(int)value);
        output->varPos = -2;
        output->flag = s_NONE;
    }
    else
    {
        std::cout << "Unknown Type" << type << std::endl;
        assert(false);
    }
}



int main()
{
    std::cout << "Hello World!" << std::endl;
    return 0;

}
