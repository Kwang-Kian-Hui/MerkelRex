#pragma once
#include <string>

class PredictedB0B1
{
    public:
        std::string product;
        double b0; 
        double b1;

        PredictedB0B1(std::string product, double b0, double b1);
};
