#pragma once
#include <string>

enum class SpreadType
{
    wide,
    tight,
    NA
};

class AnalysisData
{
    public:
        std::string product; 
        double avgAsk; 
        double askVolume;
        double avgBid; 
        double bidVolume;


        AnalysisData(std::string product, double avgAsk, double askVolume, double avgBid, double bidVolume);

        static SpreadType stringToSpreadType(std::string s);

        static std::string spreadTypeToString(SpreadType st);
};
