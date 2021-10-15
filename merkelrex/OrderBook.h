#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "AnalysisData.h"
#include "PredictedB0B1.h"

#include <string>
#include <vector>
#include <map>

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);

        ////////////////////////////////////////////////////////
        void insertCurrentTimeData(std::string timestamp);
        void insertCurrentTimeDataForProduct(std::string product, std::string timestamp);
        void insertCurrentTimeAndRemoveOldestTimeData(std::string timestamp);
        int dataCount();
        std::string getSufficientData(std::string currentTimestamp);
        std::vector<OrderBookEntry> analyseAndGenerateEligibleOrders(std::string currentTimestamp, std::map<std::string, double> wallet);
    private:
        std::vector<OrderBookEntry> orders;

        /////////////////////////////////////////////////////
        double calculateRSI(std::vector<AnalysisData> productData, OrderBookType orderType);
        double calculateABRatio(std::vector<AnalysisData> productData);
        double getCurrentPrice(std::vector<AnalysisData> productData);
        void createEligibleOrder(std::string product, std::string currentTimestamp, OrderBookType orderType, double predictedPrice, double currentPrice);
        void insertData(std::string product, std::string timestamp);
        void generateSlopeValue(std::vector<AnalysisData> productData);


        std::vector<AnalysisData> ethBTCData;
        std::vector<AnalysisData> dogeBTCData;
        std::vector<AnalysisData> btcUSDTData;
        std::vector<AnalysisData> ethUSDTData;
        std::vector<AnalysisData> dogeUSDTData;

        std::map<std::string, double> spreadValue;
        std::vector<PredictedB0B1> slopeValue;
        std::vector<OrderBookEntry> eligibleOrders;
        std::vector<double> targetPrices;
};
