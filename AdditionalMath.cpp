#include "AdditionalMath.h"

long long factorial(long long n)
{
    if(n<0)
        return(-1); /*Wrong value*/
    if(n==0)
        return(1);  /*Terminating condition*/
    else
        return(n*factorial(n-1));
}
