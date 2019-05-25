#include "Zonotope.hh"

StackValue* Zonotope::topStackValue()
{
    // creates a top stack value and returns its address
    StackValue s = 
    {
        .varName = "TOP",
        .varPos = -1,
        .flag = s_TOP,
    };
    return &s;
}

StackValue* Zonotope::botStackValue()
{
    // creates a bot stackv value and returns its address
    StackValue s = 
    {
        .varName = "BOT",
        .varPos = -1,
        .flag = s_BOT,
    };
    return &s;
}

bool isTopStackValue(StackValue* s)
{
    if(s->flag == s_TOP)
        return true;
    return false;
}

bool isBotStackValue(StackValue* s)
{
    if(s->flag == s_BOT)
        return true;
    return false;
}