#include "helpers_bno055.h"

int signum(double value)
{
    return (value > 0) - (value < 0);
}

double absolute(double value)
{
    if (value < 0)
    {
        value = -value;
    }
    return value;
}

int quotient(double dividend, double divisor)
{
    return (int)(dividend / divisor);
}