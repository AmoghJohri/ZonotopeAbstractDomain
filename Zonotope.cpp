#include "Zonotope.hh"
#include "SecondaryFunctions.cpp"

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

void Zonotope::printStackValue(AbstractValue* abstractValue, StackValue* stackValue) // CHANGE THE FUNCTION
{
    std::map<std::string, StackValue*>::iterator itr;

    // looks in the affine_set for the required variable
    for(itr = abstractValue->affineSet.begin(); itr != abstractValue->affineSet.end(); ++itr)
    {
        if((itr->second)->varName.compare(stackValue->varName) == 0)
            break;
    }

    // if the variable is not present, returns TOP
    //if(itr == abstractValue->affineSet.end())
    //    std::cout << "TOP" << std::endl;
    //else 
    //{
        //if((itr->second)->lv == LITERAL)
        //{
        //    std::cout << (itr->second)->varName << " = " << (itr->second)->concreteValue.first;
        //}
        //int pos = (itr->second)->varPos; // the col. of the matrices where the variable is present
//
        //std::cout << (itr->second)->varName << " = ";

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


   // }
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

    sort(abs1->constraintOverCentralMatrix.begin(),abs1->constraintOverCentralMatrix.end(),vecCompare1);
    sort(abs2->constraintOverCentralMatrix.begin(),abs2->constraintOverCentralMatrix.end(),vecCompare1);
    sort(abs1->constraintOverPerturbedMatrix.begin(),abs1->constraintOverPerturbedMatrix.end(),vecCompare1);
    sort(abs2->constraintOverPerturbedMatrix.begin(),abs2->constraintOverPerturbedMatrix.end(),vecCompare1);

    std::vector<std::pair<double,double>> Phi1CX = abs1->constraintOverCentralMatrix;
    std::vector<std::pair<double,double>> Phi2CX = abs2->constraintOverCentralMatrix;
    std::vector<std::pair<double,double>> Phi1PX = abs1->constraintOverPerturbedMatrix;
    std::vector<std::pair<double,double>> Phi2PX = abs2->constraintOverPerturbedMatrix;

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
        Phi1CX.insert(Phi1CX.end(), std::make_pair(-1.0,1.0));
    }
    else if(Ca2.n_rows < max_row_C + abs1->n)
    {
        Ca2.insert_rows(abs2->n, max_row_C);
        Phi2CX.insert(Phi2CX.end(), std::make_pair(-1.0,1.0));
    }
    
    // making the number of rows in Perturbed matrix equal
    if(Pa1.n_rows < max_row_P + abs1->m)
    {
        Pa1.insert_rows(abs1->m, max_row_P);
        Phi1PX.insert(Phi1PX.end(), std::make_pair(-1.0,1.0));
    }
    else if(Pa2.n_rows < max_row_P, abs1->m)
    {
        Pa2.insert_rows(abs2->m, max_row_P);
        Phi2PX.insert(Phi2PX.end(), std::make_pair(-1.0,1.0));
    }
    
    

    arma::arma_rng::set_seed_random();
    arma::Col<double> u;

    // checking if abs1 <= abs2;

    int counter = 0;
    int tag = 0;
    while (counter < 100)
    {
        u = arma::randn(Ca1.n_cols);

        arma::Col<double> v = (Ca2 - Ca1)*u;
        for(int i = 0; i < Phi1CX.size(); i++)
            v(i) = v(i)* (std::max(abs(Phi1CX[i].first), abs(Phi1CX[i].second)));
        double x = arma::accu(arma::abs(v));


        double y = 0;
        if(Pa2.n_rows != 0)
            v = Pa2*u;
            for(int i = 0; i < Phi2PX.size(); i++)
                v(i) = v(i)* (std::max(abs(Phi2PX[i].first), abs(Phi2PX[i].second)));
            y = arma::accu(arma::abs(v));
        
        
        double z = 0;
        if(Pa1.n_rows != 0)
            v = Pa1*u;
            for(int i = 0; i < Phi1PX.size(); i++)
                v(i) = v(i)* (std::max(abs(Phi1PX[i].first), abs(Phi1PX[i].second)));
            z = arma::accu(arma::abs(v));
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

        arma::Col<double> v = (Ca2 - Ca1)*u;
        for(int i = 0; i < Phi2CX.size(); i++)
            v(i) = v(i)* (std::max(abs(Phi2CX[i].first), abs(Phi2CX[i].second)));
        double x = arma::accu(arma::abs(v));


        double y = 0;
        if(Pa2.n_rows != 0)
            v = Pa2*u;
            for(int i = 0; i < Phi2PX.size(); i++)
                v(i) = v(i)* (std::max(abs(Phi2PX[i].first), abs(Phi2PX[i].second)));
            y = arma::accu(arma::abs(v));
        
        
        double z = 0;
        if(Pa1.n_rows != 0)
            v = Pa1*u;
            for(int i = 0; i < Phi1PX.size(); i++)
                v(i) = v(i)* (std::max(abs(Phi1PX[i].first), abs(Phi1PX[i].second)));
            z = arma::accu(arma::abs(v));
        x = x - z  + y;
       
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

AbstractValue* Zonotope::join(AbstractValue* X, AbstractValue* Y)
{
    if((X->flag == a_TOP) || (Y->flag == a_TOP))
        return X;
    else if(X->flag == a_BOT)
        return Y;
    else if(Y->flag == a_BOT)
        return X;
    
    // arranging all the noise symbols
    
    // initiliazing the new affine set
    AbstractValue* Z = new AbstractValue;
    Z->affineSetName = "Z";
    Z->n = std::max(X->n, Y->n);
    Z->p = std::max(X->p, Y->p);
    Z->m = std::max(X->m, Y->m);

    Z->flag = a_NONE;

    Z->centralMatrix = arma::zeros(Z->n,Z->p);
    Z->perturbedMatrix = arma::zeros(Z->m,Z->p);

    for(int i = 0; i < Z->n; i++)
        Z->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    for(int i = 0; i < Z->m; i++)
        Z->constraintOverPerturbedMatrix.push_back(std::make_pair(-1,1));
    //
    // not setting the stack value for the new affine-set

    // setting the rest of the matrices to be dimensionally equal
    int max_col = abs(X->p - Y->p); // difference between the number of variables
    int max_row_C = abs(X->n - Y->n); // differnce between the number of central noise symbols
    int max_row_P = abs(X->m - Y->m); // difference between the number of perturbed noise symbols

    arma::Mat<double> Ca1 = X->centralMatrix;
    arma::Mat<double> Ca2 = Y->centralMatrix;
    arma::Mat<double> Pa1 = X->perturbedMatrix;
    arma::Mat<double> Pa2 = Y->perturbedMatrix;

    // making the number of cols equal in all the matrices
    if(Ca1.n_cols < max_col + Y->p)
    {
        Ca1.insert_cols(X->p, max_col);
        Pa1.insert_cols(X->p, max_col);
    }
    else if(Ca2.n_cols < max_col + X->p)
    {
        Ca2.insert_cols(Y->p, max_col);
        Pa2.insert_cols(Y->p, max_col);
    }


    // making the number of rows in Central matrix equal
    if(Ca1.n_rows < max_row_C + Y->n)
    {
        Ca1.insert_rows(X->p, max_row_C);
    }
    else if(Ca2.n_rows < max_row_C + X->n)
    {
        Ca2.insert_rows(Y->n, max_row_C);
    }
    
    // making the number of rows in Perturbed matrix equal
    if(Pa1.n_rows < max_row_P + X->m)
    {
        Pa1.insert_rows(X->m, max_row_P);
    }
    else if(Pa2.n_rows < max_row_P, X->m)
    {
        Pa2.insert_rows(Y->m, max_row_P);
    }
    //

    // Need to Joine (Ca1 + Pa1) and (Ca2 + Pa2)

    Z->perturbedMatrix.insert_rows(Z->m,Z->p);


    for(int k = 0; k < Z->p ;k++) // over all the variables
    {
        Z->centralMatrix(0,k) = (intervalJoin(concretize(k,X),concretize(k,Y)).first + intervalJoin(concretize(k,X),concretize(k,Y)).second)/2;

        for(int i = 1; i < Z->n; i++)
            Z->centralMatrix(i,k) = argmin(Ca1(i,k),Ca2(i,k));

        for(int i = 0; i < Z->m; i++)
            Z->perturbedMatrix(i,k) = argmin(Pa1(i,k),Pa2(i,k));
        
        Z->perturbedMatrix(Z->m+k,k) = intervalJoin(concretize(k,X),concretize(k,Y)).second - Z->centralMatrix(0,k);
        double aux1 = 0;
        double aux2 = 0;
        for(int i = 1; i < Z->n; i++)
            aux1 = aux1 + abs(Z->centralMatrix(i,k));
        for(int i = 0; i < Z->m; i++)
            aux2 = aux2 + abs(Z->perturbedMatrix(i,k));
        Z->perturbedMatrix(Z->m+k,k) = Z->perturbedMatrix(Z->m+k,k) + (-aux1);
        Z->perturbedMatrix(Z->m+k,k) = Z->perturbedMatrix(Z->m+k,k) + (-aux2);
    }

    return Z;
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
    std::cout << "///////////////////////////////////////////// REPORT ///////////////////////////////////////////////" << std::endl << std::endl;
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
    //for(itr2 = (currentAbstractValue->affineSet).begin(); itr2 != (currentAbstractValue->affineSet).end(); ++itr2)
    //{
    //    this->printStackValue(currentAbstractValue, (itr2->second));
    //}
    std::cout << "Number of Variables are : " << currentAbstractValue->p << std::endl;
    std::cout << "Number of Central Noise Symbols are : " << currentAbstractValue->n - 1 << std::endl;
    std::cout << "Number of Perturbed Noise Symbols are : " << currentAbstractValue->m << std::endl;
    std::cout << "//////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;


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
        abValue->centralMatrix.insert_rows(abValue->n, max1 - abValue->n + 1);

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
        abValue->n = abValue->centralMatrix.n_rows;
        for(int i = 0; i <= (abValue->constraintOverCentralMatrix.size() - abValue->n) + 1; i++)
            abValue->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    }
    abValue->affineSet.insert(make_pair(stValue->varName, stValue));
    
    return abValue;
    // Included the perturbed form as well
}

std::pair<double, double> Zonotope::concretize(StackValue* s, AbstractValue* a)
{
    double ldev = 0;
    double rdev = 0;

    for(int i = 1; i < s->centralVector.size(); i++)
    {
        if(s->centralVector[i].second > 0)
        {
            ldev = ldev + s->centralVector[i].second*a->constraintOverCentralMatrix[std::stoi(s->centralVector[i].first) - 1].first;
            rdev = rdev + s->centralVector[i].second*a->constraintOverCentralMatrix[std::stoi(s->centralVector[i].first) - 1].second;
        }
        else 
        {
            ldev = ldev + s->centralVector[i].second*a->constraintOverCentralMatrix[std::stoi(s->centralVector[i].first) - 1].second;
            rdev = rdev + s->centralVector[i].second*a->constraintOverCentralMatrix[std::stoi(s->centralVector[i].first) - 1].first; 
        }
    }

    for(int i = 0; i < s->perturbedVector.size(); i++)
    {
        if(s->perturbedVector[i].second > 0)
        {
            ldev = ldev + s->perturbedVector[i].second*a->constraintOverPerturbedMatrix[std::stoi(s->perturbedVector[i].first) - 1].first;
            rdev = rdev + s->perturbedVector[i].second*a->constraintOverPerturbedMatrix[std::stoi(s->perturbedVector[i].first) - 1].second;
        }
        else 
        {
            ldev = ldev + s->perturbedVector[i].second*a->constraintOverPerturbedMatrix[std::stoi(s->perturbedVector[i].first) - 1].second;
            rdev = rdev + s->perturbedVector[i].second*a->constraintOverPerturbedMatrix[std::stoi(s->perturbedVector[i].first) - 1].first; 
        }
    }

    return std::make_pair(s->centralVector[0].second + ldev, s->centralVector[0].second + rdev);
}

std::pair<double, double> Zonotope::concretize(int k, AbstractValue* a)
{
    double ldev = 0;
    double rdev = 0;

    for(int i = 1; i < a->n; i++)
    {
        if(a->centralMatrix(i,k) > 0)
        {
            ldev = ldev + a->centralMatrix(i,k)*a->constraintOverCentralMatrix[i - 1].first;
            rdev = rdev + a->centralMatrix(i,k)*a->constraintOverCentralMatrix[i - 1].second;
        }
        else 
        {
            ldev = ldev + a->centralMatrix(i,k)*a->constraintOverCentralMatrix[i - 1].second;
            rdev = rdev + a->centralMatrix(i,k)*a->constraintOverCentralMatrix[i - 1].first;
        }
    }
    for(int i = 0; i < a->m; i++)
    {
        if(a->perturbedMatrix(i,k) > 0)
        {
            ldev = ldev + a->perturbedMatrix(i,k)*a->constraintOverPerturbedMatrix[i].first;
            rdev = rdev + a->perturbedMatrix(i,k)*a->constraintOverPerturbedMatrix[i].second;
        }
        else 
        {
            ldev = ldev + a->perturbedMatrix(i,k)*a->constraintOverPerturbedMatrix[i].second;
            rdev = rdev + a->perturbedMatrix(i,k)*a->constraintOverPerturbedMatrix[i].first; 
        }
    }


    return std::make_pair(a->centralMatrix(0,k) + ldev, a->centralMatrix(0,k) + rdev);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main()
{
    Zonotope zonotope;

    AbstractValue* X = new AbstractValue;
    X->affineSetName = "X";
    X->centralMatrix = {{1,1}, {1,0}, {0,2}, {1,1}};
    X->n = 4;
    X->p = 2;
    X->m = 0;
    X->flag = a_NONE;
    for(int i = 0; i < X->n-1; i++)
        X->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    
    zonotope.printAbstractValue(X);

    AbstractValue* Y = new AbstractValue;
    Y->affineSetName = "Y";
    Y->centralMatrix = {{-2,-2},{1,0}, {0,2}};
    Y->n = 3;
    Y->p = 2;
    Y->m = 0;
    Y->flag = a_NONE;
    for(int i = 0; i < Y->n-1; i++)
        Y->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    
    zonotope.printAbstractValue(Y);

    AbstractValue* Z = new AbstractValue;
    Z = zonotope.join(X,Y);

    zonotope.printAbstractValue(Z);

    
    return 0;

}
