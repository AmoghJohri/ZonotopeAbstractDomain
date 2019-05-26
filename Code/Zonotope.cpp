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

        //// prints the value of the central matrix
        //std::cout << abstractValue->centralMatrix(0,pos);
        //for(int counter = 1; counter < (abstractValue->centralMatrix).n_rows; counter++)
        //{
        //    std::cout << " + " << abstractValue->centralMatrix(counter,pos) << "e" << counter;
        //}
//
        //// prints the value of the perturbed matrix
        //for(int counter = 0; counter < (abstractValue->perturbedMatrix).n_rows; counter++)
        //{
        //    std::cout << " + " << abstractValue->perturbedMatrix(counter,pos) << "n" << counter+1;
        //}

        // printing directly through vectors
        std::cout << stackValue->centralVector[0].second;
        for(int counter = 1; counter < stackValue->centralVector.size(); counter++)
            std::cout << " + " << stackValue->centralVector[counter].second << "e" << stackValue->centralVector[counter].first;
        for(int counter = 0; counter < stackValue->perturbedVector.size(); counter++)
            std::cout << " + " << stackValue->perturbedVector[counter].second << "n" << stackValue->perturbedVector[counter].first;
        std::cout << std::endl;


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
        output->centralVector.push_back(std::make_pair("0", (int)value));
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
        output->centralVector.push_back(std::make_pair("0", value));
        return output;
    }
    else
    {
        std::cout << "Unknown Type" << type << std::endl;
        assert(false);
    }
}

LatticeCompare Zonotope::compare(AbstractValue* abs1, AbstractValue* abs2)
{
    int max_col = abs(abs1->p - abs2->p); // difference between the number of variables
    int max_row_C = abs(abs1->n - abs2->n); // differnce between the number of central noise symbols
    int max_row_P = abs(abs1->m - abs2->m); // difference between the number of perturbed noise symbols

    arma::Mat<double> Ca1 = abs1->centralMatrix;
    arma::Mat<double> Ca2 = abs2->centralMatrix;
    arma::Mat<double> Pa1 = abs1->perturbedMatrix;
    arma::Mat<double> Pa2 = abs2->perturbedMatrix;

    // making the number of cols equal in all the matrices
    if(Ca1.n_cols < max_col + abs2->p)
    {
        Ca1.insert_cols(abs1->p, max_col);
        Pa1.insert_cols(abs1->p, max_col);
    }
    else if(Ca2.n_cols < max_col + abs1->p)
    {
        Ca2.insert_cols(abs2->p, max_col);
        Pa2.insert_cols(abs2->p, max_col);
    }


    // making the number of rows in Central matrix equal
    if(Ca1.n_rows < max_row_C + abs2->n)
    {
        Ca1.insert_rows(abs1->p, max_row_C);
    }
    else if(Ca2.n_rows < max_row_C + abs1->n)
    {
        Ca2.insert_rows(abs2->n, max_row_C);
    }
    
    // making the number of rows in Perturbed matrix equal
    if(Pa1.n_rows < max_row_P + abs1->m)
    {
        Pa1.insert_rows(abs1->m, max_row_P);
    }
    else if(Pa2.n_rows < max_row_P, abs1->m)
    {
        Pa2.insert_rows(abs2->m, max_row_P);
    }
    
    

    arma::arma_rng::set_seed_random();
    arma::Col<double> u;

    // checking if abs1 <= abs2;

    int counter = 0;
    int tag = 0;
    while (counter < 100)
    {
        u = arma::randn(Ca1.n_cols);
        double x = arma::accu((arma::abs((Ca2 - Ca1)*u)));
        double y = 0;
        if(Pa2.n_rows != 0)
         y = arma::accu(arma::abs(Pa2*u));
        double z = 0;
        if(Pa1.n_rows != 0)
         z = arma::accu(arma::abs(Pa1*u));
        x = x + z - y;
        if(x > 0)
        {
            tag = 1;
            break;
        }
        counter = counter + 1;
    }
    if(tag == 0)
        return LT;
    
    // checking if abs2 <= abs1

    counter = 0;
    tag = 0;
    while (counter < 100)
    {
        u = arma::randn(Ca1.n_cols);
        double x = arma::accu((arma::abs((Ca2 - Ca1)*u)));
        double y = 0;
        if(Pa2.n_rows != 0)
         y = arma::accu(arma::abs(Pa2*u));
        double z = 0;
        if(Pa1.n_rows != 0)
         z = arma::accu(arma::abs(Pa1*u));
        x = x + y - z;
        if(x > 0)
        {
            tag = 1;
            break;
        }
        counter = counter + 1;
    }
    if(tag == 0)
        return GT;

    return UC;

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
    StackValue* s = new StackValue;
    s = rhsStackValue;
    s->varName = variableName;
    this->addCustomVariable(s, currentAbstractValue);
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
    std::cout << std::endl;

    std::map<std::string, StackValue*>::iterator itr2;
    for(itr2 = (currentAbstractValue->affineSet).begin(); itr2 != (currentAbstractValue->affineSet).end(); ++itr2)
    {
        this->printStackValue(currentAbstractValue, (itr2->second));
    }
    std::cout << "Number of Variables are : " << currentAbstractValue->p << std::endl;
    std::cout << "Number of Central Noise Symbols are : " << currentAbstractValue->n - 1 << std::endl;
    std::cout << "Number of Perturbed Noise Symbols are : " << currentAbstractValue->m << std::endl;


}


StackValue* Zonotope::evaluateBinaryOperation(std::string opcode, std::string return_type, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* currentAbstractValue)
{
    if((isBotStackValue(lhs_stack_value) || isBotStackValue(rhs_stack_value)))
        return &botStackVal;
    else if((isTopStackValue(lhs_stack_value) || isTopStackValue(rhs_stack_value)))
        return &topStackVal;

    if(strcmp(opcode.c_str(),"+") == 0) // adds two number and stores it into a different variable
    {
        StackValue* temp = new StackValue;
        temp->varName = std::to_string(globalCounter);
        temp->flag = s_NONE;
        globalCounter = globalCounter + 1;
        if((lhs_stack_value->lv == VARIABLE) || (rhs_stack_value->lv == VARIABLE))
            temp->lv = VARIABLE;
        else{
            temp->lv == LITERAL;
            temp->concreteValue = std::make_pair((lhs_stack_value->concreteValue.first + rhs_stack_value->concreteValue.first), (lhs_stack_value->concreteValue.first + rhs_stack_value->concreteValue.first));
            return temp;
        }
        std::vector<std::pair<std::string, double>> tempCentralVector;
        std::vector<std::pair<std::string, double>> tempPerturbedVector;
        for(int i = 0; i < lhs_stack_value->centralVector.size(); i++)
        {
            tempCentralVector.push_back(lhs_stack_value->centralVector[i]);
        }
        int tag = 0;
        for(int i = 0; i < rhs_stack_value->centralVector.size(); i++)
        {
            tag = 0;
            for(int j = 0; j < tempCentralVector.size(); j++)
            {
                if(tempCentralVector[j].first.compare(rhs_stack_value->centralVector[i].first) == 0)
                {
                    tempCentralVector[j].second = tempCentralVector[j].second + rhs_stack_value->centralVector[i].second;
                    tag = 1;
                    break;
                }

            }
            if(tag == 0)
                tempCentralVector.push_back(rhs_stack_value->centralVector[i]);
        }
        temp->centralVector = tempCentralVector;
        return temp;
    }
    /*else if(strcmp(opcode.c_str(),"*") == 0)
    {
        StackValue* temp = new StackValue;
        temp->varName = std::to_string(globalCounter);
        temp->flag = s_NONE;
        globalCounter = globalCounter + 1;
        if((lhs_stack_value->lv == VARIABLE) || (rhs_stack_value->lv == VARIABLE))
            temp->lv = VARIABLE;
        else{
            temp->lv == LITERAL;
            temp->concreteValue = std::make_pair((lhs_stack_value->concreteValue.first * rhs_stack_value->concreteValue.first), (lhs_stack_value->concreteValue.first * rhs_stack_value->concreteValue.first));
            return temp;
        }
        if(lhs_stack_value->lv == LITERAL)
        {
            for(int i = 0; i < rhs_stack_value->centralVector.size(); i++)
                rhs_stack_value->centralVector[i].second = (rhs_stack_value->centralVector[i].second) * (lhs_stack_value->centralVector[0].second);
        }
        else if(rhs_stack_value->lv == LITERAL)
        {
            for(int i = 0; i < lhs_stack_value->centralVector.size(); i++)
                lhs_stack_value->centralVector[i].second = (lhs_stack_value->centralVector[i].second) * (rhs_stack_value->centralVector[0].second);
        }
        else
        {
            // the case where both are variables
            return lhs_stack_value;
        }
    }*/
    else
        return lhs_stack_value;
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
    variable->centralVector.push_back(std::make_pair("0", (p.first + p.second)/2));

    currentAbstractValue->n = currentAbstractValue->n + 1;
    currentAbstractValue->centralMatrix(currentAbstractValue->n, currentAbstractValue->p) = (p.second - p.first)/2;
    variable->centralVector.push_back(std::make_pair(std::to_string(currentAbstractValue->n), (-p.first + p.second)/2));

    currentAbstractValue->p = currentAbstractValue->p + 1;
    currentAbstractValue->constraintOverCentralMatrix.push_back(std::make_pair(-1.0,1.0));
    return currentAbstractValue;
}

AbstractValue* Zonotope::addCustomVariable(StackValue* stValue, AbstractValue* abValue) // should not be a BOT or a TOP
{
    abValue->centralMatrix.insert_cols(abValue->p,1);
    int max1 = 0;
    for(int counter = 0; counter < stValue->centralVector.size(); counter++)
    {
        if(std::stoi(stValue->centralVector[counter].first) > max1)
            max1 = std::stoi(stValue->centralVector[counter].first);
    }
    if(max1 > 0 && abValue->n != 0 && (max1 - abValue->n > 0))
        abValue->centralMatrix.insert_rows(abValue->n+1, max1 - abValue->n + 1);
    else if(max1 > 0 && abValue->n == 0 && (max1 - abValue->n > 0))
        abValue->centralMatrix.insert_rows(0, max1 - abValue->n + 1);
    else if((max1 == 0) && (abValue->n == 0)) // adding a literal variable
        abValue->centralMatrix.insert_rows(0, 1);
    for(int counter = 0; counter < stValue->centralVector.size(); counter++)
    {
        abValue->centralMatrix(std::stoi(stValue->centralVector[counter].first),abValue->p) = stValue->centralVector[counter].second;
    }
    stValue->varPos = abValue->p;
    abValue->p = abValue->p + 1;
    if(abValue->centralMatrix.n_rows > abValue->n)
    {
        for(int i = 0; i < abValue->centralMatrix.n_rows - abValue->n - 1; i++)
            abValue->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
        abValue->n = abValue->centralMatrix.n_rows;
    }
    abValue->affineSet.insert(make_pair(stValue->varName, stValue));
    
    return abValue;
    // Included the perturbed form as well
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main()
{
    Zonotope zonotope;

    AbstractValue X = zonotope.createAffineSet("X");
    AbstractValue Y = zonotope.createAffineSet("Y");

    std::vector<std::pair<std::string, double>> vec;

    //creating x1 variable
    vec = {std::make_pair("0",1.0), std::make_pair("1", 1.0)};
    StackValue x1
    {
    .varName = "x1",
    };
    x1.flag = s_NONE;
    x1.centralVector = vec;
    x1.lv = VARIABLE;

    //creating x2 variable
    vec = {std::make_pair("0",1.0)};
    StackValue x2
    {
    .varName = "x2",
    };
    x2.flag = s_NONE;
    x2.centralVector = vec;
    x2.lv = VARIABLE;

    //creating y1 variable
    vec = {std::make_pair("0",1.0)};
    StackValue y1
    {
    .varName = "y1",
    };
    y1.flag = s_NONE;
    y1.centralVector = vec;
    y1.lv = VARIABLE;

    //creating y2 variable
    vec = {std::make_pair("0",1.0)};
    StackValue y2
    {
    .varName = "y2",
    };
    y2.flag = s_NONE;
    y2.centralVector = vec;
    y2.lv = VARIABLE;
    

    zonotope.addCustomVariable(&x1, &X);
    zonotope.addCustomVariable(&x2, &X);
    zonotope.addCustomVariable(&y1, &Y);
    zonotope.addCustomVariable(&y2, &Y);

    X.perturbedMatrix = {{0,1},{0,0}}; // this does not change other values related to perturbation like m of the system 
    Y.perturbedMatrix = {{0,1},{1,0}}; // this does not change other values related to perturbation like m of the system


    if(zonotope.compare(&X, &Y) == 0)
        std::cout << "X < Y";
    else if(zonotope.compare(&X,&Y) == 1)
        std::cout << "Y < X";
    else
        std::cout << "Incomparable!";
    std::cout << std::endl;

    zonotope.printAbstractValue(&X);
    zonotope.printAbstractValue(&Y);

    return 0;

}
