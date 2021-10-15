#include "AnalysisData.h"

AnalysisData::AnalysisData(std::string _product, 
                        double _avgAsk,
                        double _askVolume,
                        double _avgBid,
                        double _bidVolume)
: product(_product), 
  avgAsk(_avgAsk),
  askVolume(_askVolume),
  avgBid(_avgBid),
  bidVolume(_bidVolume)
{        
}

SpreadType AnalysisData::stringToSpreadType(std::string s)
{
  if (s == "wide")
  {
    return SpreadType::wide;
  }
  if (s == "tight")
  {
    return SpreadType::tight;
  }
  return SpreadType::NA;
}

std::string AnalysisData::spreadTypeToString(SpreadType st)
{
  if (st == SpreadType::wide)
  {
    return "wide";
  }
  if (st == SpreadType::tight)
  {
    return "tight";
  }
  return "NA";
}
