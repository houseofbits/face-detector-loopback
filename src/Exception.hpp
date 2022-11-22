#pragma once

#include <stdio.h>

class Exception
{
protected:
    std::string message;

public:
    Exception(std::string m)
    {
        message = m;
    }
    std::string getMessage()
    {
        return message;
    }
};