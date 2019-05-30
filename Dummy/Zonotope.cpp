#include "Zonotope.hh"
#include "SecondaryFunctions.cpp"

Zonotope::Zonotope(){};

StackValue* Zonotope::topStackValue() // returns a pointer to a TOP stack valu
{
    StackValue* s = new StackValue;
    s->varName = "TOP";
    s->flag = s_TOP;
    return s;
}

StackValue* Zonotope::botStackValue() // returns a pointer to the BOT stack value
{
    StackValue* s = new StackValue;
    s->varName = "BOT";
    s->flag = s_BOT;
    return s;
}

bool Zonotope::isTopStackValue(StackValue* s) // checks for a pointer whether it points to a TOP stack value
{
    if(s->flag == s_TOP)
        return true;
    return false;
}

bool Zonotope::isBotStackValue(StackValue* s) // checks for a pointer whether it ponits to a BOT stack value
{
    if(s->flag == s_BOT)
        return true;
    return false;
}

// COPY FUNCTIONS //

StackValue* Zonotope::copyStackValue(StackValue* s) // takes a stack value, copies it and returns a pointer to the copy
{
    StackValue* o = new StackValue;
    o->varName = s->varName;
    o->lv = s->lv;
    o->varPos = s->varPos;
    o->flag = s->flag;

    std::map<std::string, double>::iterator itr;
    for(itr = s->centralVector.begin(); itr != s->centralVector.end(); ++itr)
        o->centralVector.insert(*itr);
    for(itr = s->perturbedVector.begin(); itr != s->perturbedVector.end(); ++itr)
        o->perturbedVector.insert(*itr);
    return o;
}

AbstractValue* Zonotope::copyAbstractValue(AbstractValue* a) // copies the abstract value and returns a pointer to the copy
{
    AbstractValue* x = new AbstractValue;
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
    std::map<std::string, StackValue*>::iterator itr2;
    for(itr2 = a->affineSet.begin(); itr2 != a->affineSet.end(); ++itr2)
    {
        x->affineSet.insert(std::make_pair(itr2->first, copyStackValue(itr2->second)));
    }
    return x;
}

// PRINT FUNCTIONS //
void Zonotope::printStackValue(std::string  s, AbstractValue* abstractValue) // takes a variable name, searches for a stack-value with similar name in the abstract-value and pretty-prints the required stack-value
{

    std::map<std::string, StackValue*>::iterator itr; // iterator to iterate through the map which has the stack_values saved over it   

    // looks in the affine_set for the required variable on the basis of name
    for(itr = abstractValue->affineSet.begin(); itr != abstractValue->affineSet.end(); ++itr)
    {
        if((itr->second)->varName.compare(s) == 0)
            break;
    }

     //if the variable is not present in the affine-set, then it prints TOP as for a variable which is not available has no constraints
    if(itr == abstractValue->affineSet.end())
        std::cout << "TOP" << std::endl;
    
    else if((itr->second)->flag == s_TOP) // checks if the variable is a TOP
        std::cout << "TOP" << std::endl;
    
    else if((itr->second)->flag == s_BOT) // checks if the variable is a BOT
        std::cout << "BOT" << std::endl;
    
    // if a variable-of the name that is being searched for exists in the affine-set and it is not a TOP or a BOT then,
    else 
    {
        int pos = (itr->second)->varPos; // the col. of the matrices where the variable is present

        std::cout << (itr->second)->varName << " = "; // prints out the name of the variable

        std::cout << abstractValue->centralMatrix(0,pos); // prints the central-term of the variable

        // loop to print out the central noise-terms
        for(int counter = 1; counter < (abstractValue->centralMatrix).n_rows; counter++)
        {
            std::cout << " + " << abstractValue->centralMatrix(counter,pos) << "e" << counter;
        }

        // loop to print out the perturbed noise-terms
        for(int counter = 0; counter < (abstractValue->perturbedMatrix).n_rows; counter++)
        {
            std::cout << " + " << abstractValue->perturbedMatrix(counter,pos) << "n" << counter+1;
        }
    }
}

void Zonotope::printStackValue(StackValue* stackValue) // pretty-prints a stack value into its affine form
{
    if(stackValue->lv == LITERAL) // if the stack value is a literal
        std::cout << stackValue->varName << " = " <<  stackValue->litValue << std::endl;
    else
    {
        if(stackValue->flag == s_TOP) // prints TOP if the stack value corresponds to a TOP stack value
            std::cout << "TOP" << std::endl;
        else if(stackValue->flag == s_BOT) // rints BOT if the stack value corresponds to a BOT stack value
            std::cout << "BOT" << std::endl;
        else // if the stack value is niether a TOP or a BOT
        {

            // printing directly through vectors
            std::cout << stackValue->varName << " = ";
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
}

void Zonotope::printAbstractValue(AbstractValue* currentAbstractValue) // prints the entire abstract-value i.e. all the data-structures it has at a given program point
{
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
    std::map<std::string, StackValue*>::iterator itr2; // sorting the vectors in the stack value
    for(itr2 = (currentAbstractValue->affineSet).begin(); itr2 != (currentAbstractValue->affineSet).end(); ++itr2)
    {
        this->printStackValue((itr2->second));
    }

    std::cout << "//////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// takes a string, a double and the current abstract value and outputs a stackvalue of the provided name with the provided literal value, does nothing of the abstractvalue as of now
StackValue* getStackValueOfLiteral(std::string type, double value, AbstractValue* currentAbstractValue) // ADD APRON FEATURES
{
    // the name of the literal is stored as string denoting its position in the matrix

    // makes a new stack value for the appropriate literal and outputs the same
    if(strcmp(type.c_str(), "int") == 0) // if the output type is a string
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->litValue = (int)value;
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else if(strcmp(type.c_str(), "real") == 0) // if the output type is a double
    {
        StackValue *output = new StackValue;
        output->varName = "literal";
        output->lv = LITERAL;
        output->litValue = value;
        output->varPos = -2;
        output->flag = s_NONE;
        return output;
    }
    else // if the output type does not match
    {
        std::cout << "Unknown Type" << type << std::endl;
        assert(false);
    }
}

// fetches the stack value of the variable from the affine-set and returns a pointer to an identical StackValue but not the same as in the affine-set
StackValue* Zonotope::getStackValueOfVariable(std::string variableName, std::string variableType, AbstractValue* currentAbstractValue) // ADD APRON FEATURES
{

    StackValue* s = new StackValue;

    std::map<std::string, StackValue*>::iterator itr; // iterator for the affine set

    // looks in the affine_set for the required variable
    for(itr = currentAbstractValue->affineSet.begin(); itr != currentAbstractValue->affineSet.end(); ++itr)
    {
        if((itr->second)->varName.compare(variableName) == 0)
            break;
    }
    // if the variable is not present, returns TOP
    if(itr == currentAbstractValue->affineSet.end())
        return (this->topStackValue());
    else
    {
        s->varName = itr->second->varName;
        s->lv = VARIABLE;
        s->varPos = itr->second->varPos;
        s->flag = itr->second->flag;
        s->centralVector = itr->second->centralVector;
        s->perturbedVector = itr->second->perturbedVector;
        return s;
    }
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LatticeCompare Zonotope::compare(AbstractValue* abs1, AbstractValue* abs2) // compares two affine-sets and tells the ordered relation between then
{ 
    // checking for TOP and BOT combinations
    if((abs1->flag != a_NONE) || (abs2->flag != a_NONE))
    {
        if(abs2->flag == a_TOP)
            return LT;
        else if(abs2->flag == a_BOT)
            return GT;
        else if(abs1->flag == a_TOP)
            return LT;
        else if(abs1->flag == a_BOT)
            return GT;
    }

    // if neither of the two affine sets are TOP or BOT

    int max_col = abs(abs1->p - abs2->p); // difference between the number of variables
    int max_row_C = abs(abs1->n - abs2->n); // differnce between the number of central noise symbols
    int max_row_P = abs(abs1->m - abs2->m); // difference between the number of perturbed noise symbols

    // copies of the central Matrices
    arma::Mat<double> Ca1 = abs1->centralMatrix; 
    arma::Mat<double> Ca2 = abs2->centralMatrix;

    // copies of the perturbed Matrices
    arma::Mat<double> Pa1 = abs1->perturbedMatrix;
    arma::Mat<double> Pa2 = abs2->perturbedMatrix;

    // copies for the central contraint vectors
    std::vector<std::pair<double,double>> Phi1CX = abs1->constraintOverCentralMatrix;
    std::vector<std::pair<double,double>> Phi2CX = abs2->constraintOverCentralMatrix;

    // copes for the perturbed constraint vectors
    std::vector<std::pair<double,double>> Phi1PX = abs1->constraintOverPerturbedMatrix;
    std::vector<std::pair<double,double>> Phi2PX = abs2->constraintOverPerturbedMatrix;

    // making the number of cols equal in all the matrices
    if(max_col != 0)
    {
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
    }
    
    

    arma::arma_rng::set_seed_random(); // in order to generate random vectors
    arma::Col<double> u; // random vector

    // checking if abs1 <= abs2;
    int counter = 0; // to control the number of checks
    int tag = 0; // to see if the check fails
    while (counter < 100)
    {
        // checking for the constraint domain
        if(counter == 0)
        {
            for(int c = 0; c < Phi1CX.size(); c++)
            {
                if(!intervalCompare(Phi2CX[c], Phi1CX[c]))
                {
                    tag = 1;
                    break;
                }
            }
            for(int c = 0; c < Phi1PX.size(); c++)
            {
                if(!intervalCompare(Phi2PX[c], Phi1PX[c]))
                {
                    tag = 1;
                    break;
                }
            }
            if(tag == 1)
                break;
        }

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

        // checking for the constraint domain
        if(counter == 0)
        {
            for(int c = 0; c < Phi2CX.size(); c++)
            {
                if(!intervalCompare(Phi1CX[c], Phi2CX[c]))
                {
                    tag = 1;
                    break;
                }
            }
            for(int c = 0; c < Phi2PX.size(); c++)
            {
                if(!intervalCompare(Phi1PX[c], Phi2PX[c]))
                {
                    tag = 1;
                    break;
                }
            }
            if(tag == 1)
                break;
        }

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

AbstractValue* Zonotope::join(AbstractValue* X, AbstractValue* Y) // joins two affine-sets and returns the joined affine-set
{
    // checking for TOP and BOT combinations
    if((X->flag == a_TOP) || (Y->flag == a_TOP))
        return X;
    else if(X->flag == a_BOT)
        return Y;
    else if(Y->flag == a_BOT)
        return X;
    
    // if neither are TOP or BOT
    
    // initiliazing the new affine set
    AbstractValue* Z = new AbstractValue;
    Z->affineSetName = "Z";
    Z->n = std::max(X->n, Y->n);
    Z->p = std::max(X->p, Y->p);
    Z->m = std::max(X->m, Y->m);
    Z->flag = a_NONE;
    Z->centralMatrix = arma::zeros(Z->n,Z->p);
    Z->perturbedMatrix = arma::zeros(Z->m,Z->p);
    // HAVE A SECOND LOOK AT THIS
    for(int i = 0; i < Z->n; i++)
        Z->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    for(int i = 0; i < Z->m; i++)
        Z->constraintOverPerturbedMatrix.push_back(std::make_pair(-1,1));
    ////////////
    // not mapping the new affine-set to stack_values

    // setting the rest of the matrices to be dimensionally equal
    int max_col = abs(X->p - Y->p); // difference between the number of variables
    int max_row_C = abs(X->n - Y->n); // differnce between the number of central noise symbols
    int max_row_P = abs(X->m - Y->m); // difference between the number of perturbed noise symbols

    // making copies of the matrices required to join
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

    // Need to Join (Ca1 + Pa1) and (Ca2 + Pa2)

    Z->perturbedMatrix.insert_rows(Z->m,Z->p);


    for(int k = 0; k < Z->p ;k++) // over all the variables
    {
        Z->centralMatrix(0,k) = (intervalJoin(concretize(k,X),concretize(k,Y)).first + intervalJoin(concretize(k,X),concretize(k,Y)).second)/2; // setting the central value

        for(int i = 1; i < Z->n; i++) // setting the central-noise symbols
            Z->centralMatrix(i,k) = argmin(Ca1(i,k),Ca2(i,k));

        for(int i = 0; i < Z->m; i++) // setting the perturbed-noise symbols
            Z->perturbedMatrix(i,k) = argmin(Pa1(i,k),Pa2(i,k));
        
        // adding the perturbed noise symbol
        Z->perturbedMatrix(Z->m+k,k) = intervalJoin(concretize(k,X),concretize(k,Y)).second - Z->centralMatrix(0,k);
        double aux1 = 0;
        double aux2 = 0;
        for(int i = 1; i < Z->n; i++)
            aux1 = aux1 + abs(Z->centralMatrix(i,k));
        for(int i = 0; i < Z->m; i++)
            aux2 = aux2 + abs(Z->perturbedMatrix(i,k));
        Z->perturbedMatrix(Z->m+k,k) = Z->perturbedMatrix(Z->m+k,k) + (-aux1);
        Z->perturbedMatrix(Z->m+k,k) = Z->perturbedMatrix(Z->m+k,k) + (-aux2);

        Z->affineSet.insert(std::make_pair(std::to_string(k), getStackValue(Z,k))); // adds the variable pointer into the affine_set
    }
    Z->perturbedMatrix = Z->perturbedMatrix(arma::span(0,Z->m-1), arma::span(0,Z->p-1)); // removing extra rows
    std::map<std::string, StackValue*>::iterator it; // sorting the inner vectors
    return Z;
}

// HAVE TO IMPLEMENT MEET FUNCTION
// NO PROPER MEET FUNCTION NEED TO MAKE ADJUSTMENTS



StackValue* Zonotope::evaluateBinaryOperation(std::string opcode, std::string return_type, StackValue* lhs_stack_value, StackValue* rhs_stack_value, AbstractValue* currentAbstractValue)
{
    // checks whether the two numbers are bot or top
    if((isBotStackValue(lhs_stack_value) || isBotStackValue(rhs_stack_value)))
        return botStackValue();
    else if((isTopStackValue(lhs_stack_value) || isTopStackValue(rhs_stack_value)))
        return topStackValue();

    if(strcmp(opcode.c_str(),"+") == 0) // adds two number and stores it into a different variable
    {
        StackValue* s = new StackValue; // initiliazing the new variable
        s->varName = std::to_string(globalCounter); globalCounter = globalCounter + 1; // setting the name of the variable
        s->flag = s_NONE; // setting the flag to show it is not a TOP or a BOT value
        if(rhs_stack_value->lv == LITERAL && lhs_stack_value->lv == LITERAL) // if both variables are literals
            s->litValue = rhs_stack_value->litValue + lhs_stack_value->litValue; // adding the literal values
        else
        {
            std::map <std::string, double> temp; // to set the centralVector
            std::map <std::string, double>::iterator itr;

            // setting the central vector
            for(itr = lhs_stack_value->centralVector.begin(); itr != lhs_stack_value->centralVector.end(); ++itr)
            {
                temp.insert(*itr);
            }
            for(itr = rhs_stack_value->centralVector.begin(); itr != rhs_stack_value->centralVector.end(); ++itr)
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


            // setting the perturbed vector for the output
            temp.clear();
            for(itr = lhs_stack_value->perturbedVector.begin(); itr != lhs_stack_value->perturbedVector.end(); ++itr)
            {
                temp.insert(*itr);
            }
            for(itr = rhs_stack_value->perturbedVector.begin(); itr != rhs_stack_value->perturbedVector.end(); ++itr)
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
        }
        return s;
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
    // forming the new stack variable
    StackValue* variable = new StackValue; // allocating memory
    variable->varName = s; // setting its name
    variable->lv = VARIABLE; // setting it as a variable
    variable->varPos = currentAbstractValue->p; // setting it's position in the matrix
    variable->flag = s_NONE; // setting it as a normal variable (neither TOP or BOT)
    currentAbstractValue->affineSet.insert(std::make_pair(s, variable)); // adding it into the affine_set


    // setting the abstract value
    if(currentAbstractValue->n == 0)
        currentAbstractValue->centralMatrix.insert_rows(0,2);
    else
        currentAbstractValue->centralMatrix.insert_rows(currentAbstractValue->n+1,1);
    currentAbstractValue->centralMatrix.insert_cols(currentAbstractValue->p,1);

    currentAbstractValue->centralMatrix(0, currentAbstractValue->p) = (p.first + p.second)/2;
    variable->centralVector.insert(std::make_pair("0", (p.first + p.second)/2));

    currentAbstractValue->n = currentAbstractValue->n + 1;
    currentAbstractValue->centralMatrix(currentAbstractValue->n, currentAbstractValue->p) = (p.second - p.first)/2;
    variable->centralVector.insert(std::make_pair(std::to_string(currentAbstractValue->n), (-p.first + p.second)/2));

    currentAbstractValue->p = currentAbstractValue->p + 1;
    currentAbstractValue->constraintOverCentralMatrix.push_back(std::make_pair(-1.0,1.0));
    return currentAbstractValue;
}

AbstractValue* Zonotope::addCustomVariable(StackValue* stValue, AbstractValue* abValue) 
{
    // checking whether the stack value is a top or a bot
    if(stValue->flag == s_TOP)
        return abValue; // TOP value is the same as it not existing in the affine-set
    if(stValue->flag == s_BOT)
    {
        abValue->flag = a_BOT; // turns the affine set to BOT and returns it
        return abValue;
    }

    // if stack value is neither a top or a bot

    // augmenting cols in the central and perturbed matrices
    abValue->centralMatrix.insert_cols(abValue->p,1);
    abValue->perturbedMatrix.insert_cols(abValue->p,1);

    // setting the central matrix
    // obtaining the highest degree of central noise symbol
    int max1 = 0;
    std::map<std::string, double>::iterator itr;
    for(itr = stValue->centralVector.begin(); itr != stValue->centralVector.end(); ++itr)
    {
        if(std::stoi(itr->first) > max1)
            max1 = std::stoi(itr->first);
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
    for(itr = stValue->centralVector.begin(); itr != stValue->centralVector.end(); ++itr)
    {
        abValue->centralMatrix(std::stoi(itr->first),abValue->p) = itr->second;
    }
    // augmenting the vector of constraints
    if(abValue->centralMatrix.n_rows > abValue->n)
    {
        abValue->n = abValue->centralMatrix.n_rows;
        for(int i = 0; i <= (abValue->constraintOverCentralMatrix.size() - abValue->n) + 1; i++)
            abValue->constraintOverCentralMatrix.push_back(std::make_pair(-1,1));
    }

    // setting the perturbed matrix
    // obtaining the highest degree of perturbation noise symbol
    max1 = 0;
    // finding the max perturbed noise symbol
    for(itr = stValue->perturbedVector.begin(); itr != stValue->perturbedVector.end(); ++itr)
    {
        if(std::stoi(itr->first) > max1)
            max1 = std::stoi(itr->first);
    }

    if(max1 > 0 && abValue->m != 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(abValue->m, max1 - abValue->m);

    else if(max1 > 0 && abValue->m == 0 && (max1 - abValue->m > 0))
        abValue->perturbedMatrix.insert_rows(0, max1 - abValue->m);

    for(itr = stValue->perturbedVector.begin(); itr != stValue->perturbedVector.end(); ++itr)
    {
        abValue->perturbedMatrix(std::stoi(itr->first)-1,abValue->p) = itr->second;
    }
    // augmenting the vector of constraints
    if(abValue->perturbedMatrix.n_rows > abValue->m)
    {
        abValue->m = abValue->perturbedMatrix.n_rows;
        for(int i = 0; i <= (abValue->constraintOverPerturbedMatrix.size() - abValue->m); i++)
            abValue->constraintOverPerturbedMatrix.push_back(std::make_pair(-1,1));
    }





    stValue->varPos = abValue->p;
    abValue->p = abValue->p + 1;
    abValue->affineSet.insert(make_pair(stValue->varName, stValue)); // augmenting the affine set
    abValue->flag = a_NONE;
    return abValue;
    // Included the perturbed form as well
}

std::pair<double, double> Zonotope::concretize(StackValue* s, AbstractValue* a) // returns the concretized value of the stack value
{
    double ldev = 0;
    double rdev = 0;

    std::map<std::string,double>::iterator itr;

    for(itr = s->centralVector.begin(); itr != s->centralVector.end(); ++itr)
    {
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

std::pair<double, double> Zonotope::concretize(int k, AbstractValue* a) // returns the concretized value of the kth element in the affine-set
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

StackValue* Zonotope::getStackValue(AbstractValue* a, int k) // gets the kth variable in the affine set (count starts from 0)
{
    if(a->p < k) // variable not present in the Affine-Set
        return topStackValue();
    
    // creating a new stack value
    StackValue* s = new StackValue;
    s->varName = std::to_string(k); // named as the kth variable
    s->lv = VARIABLE;
    s->varPos = k;
    s->flag = s_NONE;

    // filling in the central-vector
    for(int i = 0; i < a->n; i++)
    {
        if(a->centralMatrix(i,k) != 0)
        {
            s->centralVector.insert(std::make_pair(std::to_string(i), a->centralMatrix(i,k)));
        }
    }
    // filling in ther perturbed-vector
    for(int i = 0; i < a->m; i++)
    {
        if(a->perturbedMatrix(i,k) != 0)
        {
            s->perturbedVector.insert(std::make_pair(std::to_string(i), a->perturbedMatrix(i,k)));
        }
    }
    return s;
}

AbstractValue* Zonotope::removeStackValue(AbstractValue* a, int k) // removes the variable at position k in the matrix from the affine-set
{
    if(a->p < k) // the stack_value doesn't exist
        return a;

    // removing it from the affine-set
    std::map<std::string, StackValue*>::iterator itr;
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
        if(itr->second->varPos > k) // correcting the variable for all the stack variables
            itr->second->varPos = itr->second->varPos - 1;
    }
    a->p = a->p - 1;
    
    if(a->p == 0)
        a->flag = a_BOT;

    return a;
    
}

void Zonotope::fillAffineSet(AbstractValue* a) // takes an affine-set containing of 2 matrices and completes the affine-set
{
    // setting the dimensions
    a->n = a->centralMatrix.n_rows;
    a->p = a->centralMatrix.n_cols;
    a->m = a->perturbedMatrix.n_rows;
    // setting the flag
    a->flag = a_NONE;
    // making the constaint vectors
    for(int i = 0; i < a->n-1; i++)
        a->constraintOverCentralMatrix.push_back(std::make_pair(-1.0,1.0));
    for(int i = 0; i < a->m; i++)
        a->constraintOverPerturbedMatrix.push_back(std::make_pair(-1.0,1.0));
    // filling the affine-set
    for(int k = 0; k < a->p; k++)
        a->affineSet.insert(std::make_pair(std::to_string(k), getStackValue(a,k)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main()
{
    Zonotope zonotope;
    
    AbstractValue* X = new AbstractValue;

    StackValue* var1 = new StackValue;
    var1->varName = "var1";
    var1->lv = VARIABLE;
    var1->flag = s_NONE;
    var1->centralVector = { {"0",3}, {"1", -4}, {"4", 6} };
    var1->perturbedVector = { {"1",3}, {"3", 2}, {"4", -1} };

    StackValue* var2 = new StackValue;
    var2->varName = "var2";
    var2->lv = VARIABLE;
    var2->flag = s_NONE;
    var2->centralVector = { {"0",-1}, {"2", -2}, {"3", 1} };
    var2->perturbedVector = { {"1",3}, {"3", 2}, {"6", -1} };

    zonotope.printStackValue(var1);
    zonotope.printStackValue(var2);

    StackValue* var3 = zonotope.copyStackValue(var1);
    zonotope.printStackValue(var3);

    X = zonotope.addCustomVariable(var1, X);
    X = zonotope.addCustomVariable(var2, X);
    X = zonotope.addCustomVariable(var3, X);

    X = zonotope.join(X, X);

    std::pair<double,double> c1 = zonotope.concretize(var1, X);

    zonotope.printAbstractValue(X);
    

    return 0;

}
